#pragma once

#if 0

Möchten Sie den Code für die Gegenoperation (bz3_decode_blocks) sehen, um die Datei parallel wieder zu entpacken, oder soll das CMake-Buildsystem für dieses Multithreading-Setup optimiert werden?
KI-Antworten können Fehler enthalten. Weitere Informationen
• 
Docs.rs
libbzip3_sys - Rust - Docs.rs
Returns a bzip3 error code; The function will launch a thread for each block. Every buffer is overwritten and none of them can ove...
• 
GitHub
factor/extra/compression/bzip3/ffi/ffi.factor at master - GitHub
int bz3_decompress ・ int32_t bz3_encode_block ( bz3_decode_block ・ void bz3_encode_blocks ・ void bz3_decode_blocks ( bz3_state* st...
• 
encode.su
BZip3 - Page 3 - ENCODE.SU Forum
07.03.2025 — A better, faster and stronger spiritual successor to BZip2. Features higher compression ratios and better performance thanks to a ...
Alle anzeigen
ok decompress function, erstmal ohne multithreading und low level
Hier ist die Dekompressions-Funktion (decompress_file_bz3), implementiert in C++17 und unter direkter Verwendung der Low-Level C-API von libbzip3.
Die Funktion arbeitet rein sequenziell (Single-Threaded) und liest die Datei Block für Block ein, genau so, wie es das offizielle .bz3-Format vorgibt.
C++17 Low-Level Dekompressions-Funktion
cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>

extern "C" {
    #include <libbz3.h> 
}

namespace fs = std::filesystem;

/**
 * Dekomprimiert eine .bz3 Datei sequenziell mit der Low-Level-API.
 * 
 * @param input_uri   Pfad zur komprimierten Datei (.bz3).
 * @param output_uri  Pfad, unter dem die entpackte Datei gespeichert wird.
 * @return true bei Erfolg, false oder Ausnahme bei Fehlern.
 */
bool decompress_file_bz3(const std::string& input_uri, const std::string& output_uri) {
    fs::path src_path(input_uri);
    fs::path dst_path(output_uri);

    if (!fs::exists(src_path) || !fs::is_regular_file(src_path)) {
        throw std::runtime_error("Quelldatei existiert nicht oder ist ungültig: " + src_path.string());
    }

    std::ifstream in_file(src_path, std::ios::binary);
    std::ofstream out_file(dst_path, std::ios::binary);

    if (!in_file.is_open() || !out_file.is_open()) {
        return false;
    }

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

    // 2. Bzip3-Zustand mit der gelesenen Blockgröße initialisieren
    bz3_state* state = bz3_new(block_size_bytes);
    if (!state) {
        throw std::runtime_error("Fehler beim Erstellen des bz3_state. Blockgröße korrupt?");
    }

    // 3. Buffer allokieren
    // Die maximale Ausgabegröße eines Blocks entspricht exakt der im Header definierten Blockgröße.
    // Der Eingabebuffer muss groß genug für worst-case expandierte Daten sein (bz3_bound).
    size_t max_input_buffer_size = bz3_bound(block_size_bytes);
    std::vector<uint8_t> in_buffer(max_input_buffer_size);
    std::vector<uint8_t> out_buffer(block_size_bytes);

    try {
        // 4. Block-Schleife: Metadaten lesen -> Block dekomprimieren
        while (in_file) {
            int32_t compressed_size = 0;
            int32_t original_size = 0;

            // Versuche, die Metadaten des nächsten Blocks zu lesen
            in_file.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
            if (in_file.gcount() == 0) {
                break; // Sauberes Dateiende (EOF) erreicht
            }
            
            in_file.read(reinterpret_cast<char*>(&original_size), sizeof(original_size));

            if (compressed_size <= 0 || original_size <= 0) {
                throw std::runtime_error("Korrupte Block-Metadaten in der Datei.");
            }

            // Die komprimierten Rohdaten des Blocks einlesen
            in_file.read(reinterpret_cast<char*>(in_buffer.data()), compressed_size);
            if (in_file.gcount() != compressed_size) {
                throw std::runtime_error("Unerwartetes Dateiende beim Lesen der Blockdaten.");
            }

            // 5. Low-Level Dekomprimierung
            // Wichtig: bz3_decode_block schreibt das Ergebnis in den Ausgabe-Buffer (`out_buffer`).
            // Der Eingabe-Buffer (`in_buffer`) wird dabei intern als Scratchpad modifiziert!
            int32_t decoded_bytes = bz3_decode_block(state, in_buffer.data(), out_buffer.data(), 
                                                     compressed_size, original_size);

            if (decoded_bytes < 0) {
                throw std::runtime_error("Fehler bei der Block-Dekomprimierung (bz3_decode_block lieferte Fehlercode).");
            }

            // 6. Entpackte Daten schreiben
            out_file.write(reinterpret_cast<const char*>(out_buffer.data()), decoded_bytes);
        }
    } catch (...) {
        bz3_free(state);
        throw; // Fehler weiterreichen
    }

    // 7. Ressourcen freigeben
    bz3_free(state);
    return true;
}

int main() {
    try {
        std::string input = "data.txt.bz3";
        std::string output = "data_restored.txt";
        
        if (decompress_file_bz3(input, output)) {
            std::cout << "Datei erfolgreich dekomprimiert: " << output << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
Verwende Code mit Vorsicht.
Besonderheiten der Low-Level Dekomprimierung:
1. Der Magic-Byte-Check: Jede valide Bzip3-Datei beginnt zwingend mit "bz3v1". Fehlt dieser String oder ist er beschädigt, bricht die Funktion sofort ab, um Speicherfehler zu verhindern.
2. Buffer-Verhalten bei bz3_decode_block: Anders als beim Komprimieren (bz3_encode_block), wo in-place gearbeitet wird, erwartet die Dekomprimierung zwei getrennte Puffer. Der Zielbuffer (out_buffer) fängt die entpackten Daten auf, während der Quellbuffer (in_buffer) die komprimierten Daten bereitstellt.
3. Zustands-Sicherheit: Tritt mitten in der Schleife ein Fehler auf (z.B. Festplatte voll oder Datei abrupt abgeschnitten), sorgt der try-catch-Block dafür, dass bz3_free(state) aufgerufen wird, um Speicherlecks in der C-Bibliothek zu vermeiden.
Möchten Sie diesen Code als Nächstes auf Multithreading (entweder mit C++ Pipelines oder bz3_decode_blocks) umstellen, oder wollen wir das CMakeLists.txt schreiben, um Kompression und Dekompression zusammen zu bauen?
KI-Antworten können Fehler enthalten. Weitere Informationen
ok jetzt mit multithreading, so schnell wie möglich
Um die Dekomprimierung so schnell wie möglich zu machen, nutzen wir die native C-Funktion bz3_decode_blocks zusammen mit einem Parallel Pipeline / Batch Hybrid-Ansatz in C++17.
Das Einlesen der Metadaten geschieht sequenziell, aber das Laden der Blöcke und die mathematisch intensive Dekomprimierung erfolgen parallel im RAM. Um I/O-Flaschenhälse zu minimieren, werden die Blöcke paketweise (in Batches) parallel dekomprimiert und direkt im Anschluss im Hauptthread auf die Festplatte gestreamt.
High-Speed Multithreaded Dekomprimierung
cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <thread>

extern "C" {
    #include <libbz3.h> 
}

namespace fs = std::filesystem;

/**
 * Dekomprimiert eine .bz3 Datei mit maximaler Geschwindigkeit über native C-Threads.
 * 
 * @param input_uri   Pfad zur Quelldatei (.bz3).
 * @param output_uri  Pfad zur entpackten Zieldatei.
 * @param batch_size  Anzahl der Blöcke, die gleichzeitig dekomprimiert werden (0 = Auto-Erkennung via CPU-Kerne).
 */
bool decompress_file_bz3_mt(const std::string& input_uri, const std::string& output_uri, int32_t batch_size = 0) {
    fs::path src_path(input_uri);
    fs::path dst_path(output_uri);

    if (!fs::exists(src_path) || !fs::is_regular_file(src_path)) {
        throw std::runtime_error("Quelldatei existiert nicht.");
    }

    // Automatische Thread-Zuweisung basierend auf CPU-Kernen
    if (batch_size <= 0) {
        batch_size = static_cast<int32_t>(std::max(1u, std::thread::hardware_concurrency()));
    }

    std::ifstream in_file(src_path, std::ios::binary);
    std::ofstream out_file(dst_path, std::ios::binary);
    if (!in_file.is_open() || !out_file.is_open()) return false;

    // 1. Header auslesen & validieren
    char magic[5];
    in_file.read(magic, 5);
    if (std::memcmp(magic, "bz3v1", 5) != 0) {
        throw std::runtime_error("Ungültiges Dateiformat (Magic Bytes fehlen).");
    }

    int32_t block_size_bytes = 0;
    in_file.read(reinterpret_cast<char*>(&block_size_bytes), sizeof(block_size_bytes));
    if (!in_file || block_size_bytes <= 0) {
        throw std::runtime_error("Fehler beim Lesen der Blockgröße.");
    }

    // 2. Speicher vorallokieren für Threads und Puffer
    size_t max_input_buffer_size = bz3_bound(block_size_bytes);
    
    std::vector<bz3_state*> states(batch_size);
    std::vector<std::vector<uint8_t>> in_buffers(batch_size, std::vector<uint8_t>(max_input_buffer_size));
    std::vector<std::vector<uint8_t>> out_buffers(batch_size, std::vector<uint8_t>(block_size_bytes));
    
    // Steuer-Arrays für die C-API
    std::vector<int32_t> compressed_sizes(batch_size);
    std::vector<int32_t> original_sizes(batch_size);
    std::vector<uint8_t*> in_pointers(batch_size);
    std::vector<uint8_t*> out_pointers(batch_size);

    for (int i = 0; i < batch_size; ++i) {
        states[i] = bz3_new(block_size_bytes);
        if (!states[i]) throw std::runtime_error("Fehler beim Allokieren des bz3_state.");
        in_pointers[i] = in_buffers[i].data();
        out_pointers[i] = out_buffers[i].data();
    }

    // Lambda für sicheres Ressourcen-Cleanup bei Fehlern oder regulärem Ende
    auto cleanup = [&states]() {
        for (auto* state : states) { if (state) bz3_free(state); }
    };

    // 3. Hauptschleife (Batch-Streaming)
    while (in_file) {
        int32_t blocks_in_batch = 0;

        // I/O Phase 1: Metadaten & komprimierte Rohdaten für den gesamten Batch sequenziell einlesen
        for (int i = 0; i < batch_size; ++i) {
            int32_t c_size = 0;
            int32_t o_size = 0;

            in_file.read(reinterpret_cast<char*>(&c_size), sizeof(c_size));
            if (in_file.gcount() == 0) break; // EOF erreicht

            in_file.read(reinterpret_cast<char*>(&o_size), sizeof(o_size));

            if (c_size <= 0 || o_size <= 0) {
                cleanup();
                throw std::runtime_error("Korrupte Block-Metadaten.");
            }

            // Komprimierte Daten direkt in den Thread-Buffer einlesen
            in_file.read(reinterpret_cast<char*>(in_buffers[i].data()), c_size);
            
            compressed_sizes[i] = c_size;
            original_sizes[i] = o_size;
            blocks_in_batch++;
        }

        if (blocks_in_batch == 0) break; // Keine Blöcke mehr zu verarbeiten

        // Computation Phase: Native parallele Dekomprimierung über alle CPU-Kerne parallel
        // libbzip3 teilt die Arbeit hier vollautomatisch intern auf.
        bz3_decode_blocks(states.data(), 
                          in_pointers.data(), 
                          out_pointers.data(), 
                          compressed_sizes.data(), 
                          original_sizes.data(), 
                          blocks_in_batch);

        // I/O Phase 2: Dekomprimierte Blöcke sofort am Stück auf Platte schreiben
        for (int i = 0; i < blocks_in_batch; ++i) {
            // bz3_decode_blocks gibt bei Fehlern negative Werte in compressed_sizes zurück
            if (compressed_sizes[i] < 0) {
                cleanup();
                throw std::runtime_error("Fehler bei der parallelen Block-Dekomprimierung.");
            }
            
            // Die tatsächliche Ausgabegröße entspricht der ursprünglichen Originalgröße
            out_file.write(reinterpret_cast<const char*>(out_buffers[i].data()), original_sizes[i]);
        }
    }

    cleanup();
    return true;
}

int main() {
    try {
        std::string input = "large_data.txt.bz3";
        std::string output = "large_data_restored.txt";
        
        std::cout << "Starte High-Speed Dekomprimierung..." << std::endl;
        if (decompress_file_bz3_mt(input, output, 0)) { // 0 nutzt alle CPU-Kerne
            std::cout << "Datei erfolgreich mit maximaler Geschwindigkeit entpackt!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Kritischer Fehler: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
Verwende Code mit Vorsicht.
Warum dieses Setup die maximale Geschwindigkeit erzielt:
1. Zero Allocations in der Schleife: Alle Vektoren (in_buffers, out_buffers, Pointer-Arrays) werden exakt einmal vor dem Schleifeneintritt allokiert. Innerhalb der Schleife finden keine dynamischen RAM-Allokationen statt, was CPU-Zyklen spart.
2. Hardware Concurrency Alignment: Wenn Sie batch_size = 0 übergeben, skaliert das System exakt mit Ihren CPU-Kernen (z. B. 8 oder 16 Blöcke gleichzeitig). Das verhindert Context-Switching-Overhead durch zu viele Threads.
3. Optimiertes Speicher-Layout: Durch das Array aus Roh-Pointern (in_pointers.data()) füttern wir die C-API direkt mit den Adressen der C++ std::vector-Strukturen. Es findet kein Kopieren von Speicher statt (Zero-Copy zwischen C++ und C-Bibliothek).
Jetzt haben wir die ultraschnelle Kompression und Dekommission fertig. Sollen wir als finalen Schritt das CMakeLists.txt schreiben, um beide Komponenten inklusive Multithreading-Support sauber zu kompilieren?

#endif