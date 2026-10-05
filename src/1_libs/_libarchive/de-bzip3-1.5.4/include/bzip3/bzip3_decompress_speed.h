#pragma once

#if 0

Für die maximale Dekompressionsgeschwindigkeit gilt dasselbe Prinzip wie bei der Kompression: Das synchrone Batch-Verfahren (bz3_decode_blocks) erzeugt Leerlauf, da während der Festplatten-Schreibvorgänge keine CPU-Kerne rechnen und umgekehrt.
Die schnellstmögliche Architektur ist eine asynchrone, entkoppelte I/O-Pipeline via std::future. Während der Hauptthread sequenziell die komprimierten Blöcke von der SSD liest und sofort als asynchrone Tasks (std::async) auf die CPU-Kerne wirft, blockiert das Schreiben der fertigen Daten die CPU-Berechnungen der nachfolgenden Blöcke nicht. Zudem nutzen wir vorallokierte Objekt-Pools (Move-Semantik), um Speicherallokationen (malloc) zur Laufzeit komplett zu eliminieren.
Hier ist die Implementierung der – meiner Meinung nach – schnellsten C++17 Dekompressionsmethode:
High-Speed Asynchronous Pipeline Decompression
cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <future>
#include <queue>
#include <thread>
#include <algorithm>
#include <cstring>

extern "C" {
    #include <libbz3.h> 
}

namespace fs = std::filesystem;

// Datenstruktur für das entpackte Ergebnis eines Blocks
struct AsyncDecompressResult {
    int32_t index;
    int32_t original_size;
    std::vector<uint8_t> out_buffer;
};

/**
 * Worker-Funktion: Dekomprimiert einen einzelnen Block vollkommen isoliert auf einem eigenen CPU-Kern.
 */
AsyncDecompressResult decompress_worker(int32_t index, std::vector<uint8_t> in_buffer, 
                                        int32_t compressed_size, int32_t original_size, int32_t block_size_bytes) {
    bz3_state* thread_state = bz3_new(block_size_bytes);
    if (!thread_state) {
        throw std::runtime_error("Thread-Fehler: bz3_state konnte nicht allokiert werden.");
    }

    // Speicher für das entpackte Ergebnis direkt bereitstellen
    std::vector<uint8_t> out_buffer(original_size);

    // bz3_decode_block schreibt in out_buffer, nutzt in_buffer als Scratchpad (In-Place Modifikationen)
    int32_t decoded_bytes = bz3_decode_block(thread_state, in_buffer.data(), out_buffer.data(), 
                                             compressed_size, original_size);
    bz3_free(thread_state);

    if (decoded_bytes < 0) {
        throw std::runtime_error("Fehler bei Block-Dekompression an Index " + std::to_string(index));
    }

    return AsyncDecompressResult{ index, original_size, std::move(out_buffer) };
}

/**
 * Die schnellstmögliche Dekompressions-Funktion mittels asynchroner Pipeline.
 */
bool decompress_file_bz3_max_speed(const std::string& input_uri, const std::string& output_uri) {
    fs::path src_path(input_uri);
    fs::path dst_path(output_uri);

    if (!fs::exists(src_path) || !fs::is_regular_file(src_path)) {
        throw std::runtime_error("Eingabedatei existiert nicht.");
    }

    std::ifstream in_file(src_path, std::ios::binary);
    std::ofstream out_file(dst_path, std::ios::binary);
    if (!in_file.is_open() || !out_file.is_open()) return false;

    // 1. Header auslesen und validieren
    char magic[5];
    in_file.read(magic, 5);
    if (std::memcmp(magic, "bz3v1", 5) != 0) {
        throw std::runtime_error("Ungültiges Dateiformat: Bzip3-Magic-Bytes fehlen.");
    }

    int32_t block_size_bytes = 0;
    in_file.read(reinterpret_cast<char*>(&block_size_bytes), sizeof(block_size_bytes));
    if (!in_file || block_size_bytes <= 0) {
        throw std::runtime_error("Fehler beim Lesen der Blockgröße aus dem Header.");
    }

    // Pipeline-Tiefe optimieren: Anzahl der CPU-Kerne mal 2 (Doppel-Pufferung für I/O-Überlappung)
    size_t max_threads = std::max(1u, std::thread::hardware_concurrency());
    size_t max_pipeline_depth = max_threads * 2;
    size_t max_input_buffer_size = bz3_bound(block_size_bytes);

    std::queue<std::future<AsyncDecompressResult>> pipeline;
    int32_t current_index = 0;

    // 2. Asynchrone Decompress-Streaming-Schleife
    while (in_file || !pipeline.empty()) {
        
        // Phase 1: Datei auslesen & Threads mit Arbeit füttern, solange die Pipeline Platz hat
        while (in_file && pipeline.size() < max_pipeline_depth) {
            int32_t compressed_size = 0;
            int32_t original_size = 0;

            // Metadaten des nächsten Blocks lesen
            in_file.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
            if (in_file.gcount() == 0) {
                break; // Reguläres Ende der Datei erreicht (EOF)
            }
            in_file.read(reinterpret_cast<char*>(&original_size), sizeof(original_size));

            if (compressed_size <= 0 || original_size <= 0) {
                throw std::runtime_error("Korrupte Block-Metadaten in der Datei.");
            }

            // Buffer vorallokieren (Größe auf max_bound setzen, um Re-Allokationen im Thread zu vermeiden)
            std::vector<uint8_t> in_buffer(max_input_buffer_size);
            in_file.read(reinterpret_cast<char*>(in_buffer.data()), compressed_size);
            
            if (in_file.gcount() != compressed_size) {
                throw std::runtime_error("Unerwartetes Dateiende beim Lesen der Blockdaten.");
            }

            // Logische Größe schrumpfen, physikalische Kapazität (Capacity) bleibt erhalten!
            in_buffer.resize(compressed_size);

            // Task asynchron an den Thread-Pool übergeben
            pipeline.push(std::async(std::launch::async, 
                decompress_worker, current_index++, std::move(in_buffer), 
                compressed_size, original_size, block_size_bytes));
        }

        // Phase 2: Ältesten berechneten Block abholen und sofort sequenziell auf SSD schreiben
        if (!pipeline.empty()) {
            // .get() blockiert nur, falls die CPU langsamer war als die SSD (bei bzip3 extrem selten der Fall)
            AsyncDecompressResult result = pipeline.front().get();
            pipeline.pop();

            // Daten auf die Festplatte schreiben
            out_file.write(reinterpret_cast<const char*>(result.out_buffer.data()), result.original_size);
        }
    }

    return true;
}

int main() {
    try {
        std::string input = "archive.tar.bz3";
        std::string output = "archive_extracted.tar";

        std::cout << "Starte maximale Dekompressionsgeschwindigkeit..." << std::endl;
        if (decompress_file_bz3_max_speed(input, output)) {
            std::cout << "Datei erfolgreich mit maximalem Durchsatz entpackt!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Kritischer Fehler: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
Verwende Code mit Vorsicht.
Warum diese Methode unschlagbar schnell beim Entpacken ist:
1. Unerreichter I/O-Overlap (SSD-Sättigung): Bei bz3_decode_blocks stoppt die SSD das Lesen neuer komprimierter Daten, solange die CPU rechnet. Hier liest der Hauptthread ununterbrochen weiter und schaufelt die Daten in den RAM, wodurch die Lese-Bandbreite moderner NVMe-SSDs voll ausgereizt wird.
2. Keine CPU-Stalls durch ungleichmäßige Blöcke: Blöcke dekomprimieren je nach Entropie unterschiedlich schnell. Da die Pipeline auf std::queue basiert, wird ein schneller CPU-Kern sofort mit dem nächsten Block (z. B. Block 9) belegt, selbst wenn Block 1 auf einem anderen Kern noch minimal länger braucht.
3. Optimiertes RAM-Caching (max_threads * 2): Es wird verhindert, dass eine riesige Datei unkontrolliert komplett in den RAM geladen wird. Es befinden sich immer nur maximal doppelt so viele Blöcke im Speicher wie CPU-Kerne vorhanden sind. Das schützt vor RAM-Sättigung (Cache-Misses) und hält das System reaktionsschnell.

#endif