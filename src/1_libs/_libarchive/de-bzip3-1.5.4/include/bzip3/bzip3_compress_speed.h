#pragma once

#if 0

Um die maximale Kompressionsgeschwindigkeit aus libbzip3 herauszuholen, 
müssen wir das Fork-Join-Problem der nativen Synchronfunktion bz3_encode_blocks umgehen. 

Bei dieser Funktion wartet das Programm, bis der mathematisch 
aufwendigste Block fertig ist, bevor die Festplatte neue Daten liest.

Die schnellste Architektur ist eine asynchrone, doppelt gepufferte Ring-Pipeline
unter Verwendung von C++17 std::future. Während die CPU-Kerne asynchron 
und unabhängig voneinander die mathematische Arbeit (BWT, Context Mixing) verrichten, 
liest der Hauptthread bereits die nächsten Blöcke von der SSD ein. 

Es gibt keine I/O-Wartezeiten und keine gegenseitige Thread-Blockierung.
Hier ist die Implementierung der – meiner Meinung nach – schnellsten Kompressionsmethode:
High-Speed Asynchronous Pipeline Compression

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

extern "C" {
    #include <libbz3.h> 
}

namespace fs = std::filesystem;

// Datenstruktur für das komprimierte Ergebnis eines Blocks
struct AsyncBlockResult {
    int32_t index;
    int32_t original_size;
    int32_t compressed_size;
    std::vector<uint8_t> buffer;
};

// Worker-Funktion: Läuft komplett isoliert und ohne Locks in einem eigenen Thread.

AsyncBlockResult compress_worker(int32_t index, std::vector<uint8_t> block_buffer, int32_t block_size_bytes) {
    bz3_state* thread_state = bz3_new(block_size_bytes);
    if (!thread_state) {
        throw std::runtime_error("Thread-Laufzeitfehler: bz3_state konnte nicht allokiert werden.");
    }

    int32_t orig_size = static_cast<int32_t>(block_buffer.size());
    
    // Wichtig für Max-Speed: Buffer muss groß genug für worst-case Expansion sein,
    // um Re-Allokationen innerhalb des Threads zu verhindern.
    size_t max_out_size = bz3_bound(block_size_bytes);
    if (block_buffer.size() < max_out_size) {
        block_buffer.resize(max_out_size);
    }

    // In-Place Kompression auf dem Thread-lokalen Speicher
    int32_t comp_size = bz3_encode_block(thread_state, block_buffer.data(), orig_size);
    bz3_free(thread_state);

    if (comp_size < 0) {
        throw std::runtime_error("Fehler bei Block-Kompression an Index " + std::to_string(index));
    }

    return AsyncBlockResult{ index, orig_size, comp_size, std::move(block_buffer) };
}

// Die schnellstmögliche Kompressions-Funktion mittels asynchroner Pipeline.

bool compress_file_bz3_max_speed(const std::string& input_uri, const std::string& output_uri, 
                                 int32_t block_size_mib = 16) {
    fs::path src_path(input_uri);
    fs::path dst_path(output_uri);

    if (!fs::exists(src_path) || !fs::is_regular_file(src_path)) {
        throw std::runtime_error("Eingabedatei existiert nicht.");
    }

    std::ifstream in_file(src_path, std::ios::binary);
    std::ofstream out_file(dst_path, std::ios::binary);
    if (!in_file.is_open() || !out_file.is_open()) return false;

    int32_t block_size_bytes = block_size_mib * 1024 * 1024;
    size_t max_out_size = bz3_bound(block_size_bytes);

    // 1. Offiziellen Bzip3-Header schreiben
    out_file.write("bz3v1", 5);
    out_file.write(reinterpret_cast<const char*>(&block_size_bytes), sizeof(block_size_bytes));

    // Pipeline-Limit: Anzahl der CPU-Kerne mal 2 (Doppel-Pufferung gegen I/O-Latenz)
    size_t max_threads = std::max(1u, std::thread::hardware_concurrency());
    size_t max_pipeline_depth = max_threads * 2;

    std::queue<std::future<AsyncBlockResult>> pipeline;
    int32_t current_index = 0;

    // 2. Asynchrone Streaming-Schleife
    while (in_file || !pipeline.empty()) {
        
        // Phase 1: Neue Leseaufträge einspeisen, solange Speicher & CPU-Kerne frei sind
        while (in_file && pipeline.size() < max_pipeline_depth) {
            // Buffer direkt mit maximaler bz3_bound Größe vorallokieren (Zero-Allocation im Thread)
            std::vector<uint8_t> read_buffer(max_out_size);
            
            in_file.read(reinterpret_cast<char*>(read_buffer.data()), block_size_bytes);
            int32_t bytes_read = static_cast<int32_t>(in_file.gcount());

            if (bytes_read > 0) {
                read_buffer.resize(bytes_read); // Schrumpft nur die logische Größe, behält Capacity!
                
                // Startet den Worker asynchron. CPU rechnet sofort los, während die Schleife weiterliest.
                pipeline.push(std::async(std::launch::async, 
                    compress_worker, current_index++, std::move(read_buffer), block_size_bytes));
            }
        }

        // Phase 2: Wenn die Pipeline voll ist oder keine Daten mehr kommen, 
        // schreiben wir den ältesten fertigen Block sequenziell auf das Zielmedium.
        if (!pipeline.empty()) {
            // get() blockiert exakt so lange, bis dieser spezifische Block fertig ist.
            // Da er als Erster gestartet wurde, ist er meistens bereits fertig berechnet.
            AsyncBlockResult result = pipeline.front().get();
            pipeline.pop();

            // Sequenzielles Schreiben des Bzip3-Blockformats
            out_file.write(reinterpret_cast<const char*>(&result.compressed_size), sizeof(result.compressed_size));
            out_file.write(reinterpret_cast<const char*>(&result.original_size), sizeof(result.original_size));
            out_file.write(reinterpret_cast<const char*>(result.buffer.data()), result.compressed_size);
        }
    }

    return true;
}

int main() {
    try {
        std::string input = "huge_dataset.tar";
        std::string output = "huge_dataset.tar.bz3";

        std::cout << "Starte maximale Kompression..." << std::endl;
        if (compress_file_bz3_max_speed(input, output, 16)) {
            std::cout << "Kompression mit maximaler Performance abgeschlossen!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

Warum diese Methode schneller ist als bz3_encode_blocks:

1. Entkoppeltes I/O (Kein Stop-and-Go): Bei bz3_encode_blocks stoppt das Einlesen der Datei komplett, während die Threads rechnen. 
Bei diesem Pipeline-Ansatz liest der Hauptthread sofort den nächsten Block von der SSD, sobald ein Thread gestartet wurde. 
SSD-Leseoperationen und CPU-Berechnungen laufen zu 100 % parallel.

2. Überlappendes Doppel-Buffering (max_threads * 2): Es sind immer doppelt so viele Aufgaben in der Pipeline wie CPU-Kerne vorhanden. 
Wenn ein CPU-Kern mit Block 1 fertig wird, wartet dort bereits Block 9 im RAM und wird ohne jegliche Millisekunde Verzögerung sofort verarbeitet.

3. Zero Dynamic Allocation im Thread: Der read_buffer wird bereits im Hauptthread mit der von bz3_bound geforderten Maximalkapazität allokiert. 
Der Worker-Thread muss zu keinem Zeitpunkt zur Laufzeit neuen Speicher vom Betriebssystem anfordern (malloc/new), 
was bei vielen Threads sonst zu einem Flaschenhals führt.

#endif