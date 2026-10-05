#include "Bz3Writer.h"

#include <de/Core.h>

// #include <iostream>
// #include <fstream>
// #include <filesystem>
// #include <string>
// #include <vector>
// #include <cstdint>
// #include <stdexcept>
#include <future>
#include <queue>
#include <thread>
#include <algorithm>

extern "C" {
    #include <bzip3/libbz3.h>
}

namespace fs = std::filesystem;

// Datenstruktur für das komprimierte Ergebnis eines Blocks
struct AsyncBlockResult
{
    int32_t index;
    int32_t original_size;
    int32_t compressed_size;
    std::vector<uint8_t> buffer;
};

// Worker-Funktion: Läuft komplett isoliert und ohne Locks in einem eigenen Thread.

AsyncBlockResult
Bz3_compress_worker(int32_t index, std::vector<uint8_t> block_buffer, int32_t block_size_bytes)
{
    bz3_state* thread_state = bz3_new(block_size_bytes);
    if (!thread_state)
    {
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

bool Bz3_compress_file_max_speed(const std::string& src, const std::string& dst, int32_t block_size_mib)
{
    de::File m_fin(src,de::eFileMode::Read);
    de::File m_fout(dst,de::eFileMode::Write);
    if (!m_fin.is_open()) { DE_ERROR("Cannot read ",src) return false; }
    if (!m_fout.is_open()) { DE_ERROR("Cannot write ",dst) return false; }

    int32_t block_size_bytes = block_size_mib * 1024 * 1024;
    size_t max_out_size = bz3_bound(block_size_bytes);

    // 1. Offiziellen Bzip3-Header schreiben
    m_fout.write("bz3v1", 5);
    m_fout.write(reinterpret_cast<const char*>(&block_size_bytes), sizeof(block_size_bytes));

    // Pipeline-Limit: Anzahl der CPU-Kerne mal 2 (Doppel-Pufferung gegen I/O-Latenz)
    size_t max_threads = std::max(1u, std::thread::hardware_concurrency());
    size_t max_pipeline_depth = max_threads * 2;

    std::queue<std::future<AsyncBlockResult>> pipeline;
    int32_t current_index = 0;

    // std::vector<uint8_t> read_buffer(max_out_size);

    // 2. Asynchrone Streaming-Schleife
    while (!pipeline.empty())
    {
        // Phase 1: Neue Leseaufträge einspeisen, solange Speicher & CPU-Kerne frei sind
        while (pipeline.size() < max_pipeline_depth)
        {
            // Buffer direkt mit maximaler bz3_bound Größe vorallokieren (Zero-Allocation im Thread)
            std::vector<uint8_t> read_buffer(max_out_size);
            // memset(read_buffer.data(),0,read_buffer.size());

            int32_t bytes_read = m_fin.read(read_buffer.data(), block_size_bytes);
            if (bytes_read > 0)
            {
                read_buffer.resize(bytes_read); // Schrumpft nur die logische Größe, behält Capacity!

                // Startet den Worker asynchron. CPU rechnet sofort los, während die Schleife weiterliest.
                pipeline.push(std::async(std::launch::async,
                    Bz3_compress_worker, current_index++, std::move(read_buffer), block_size_bytes));
            }
        }

        // Phase 2: Wenn die Pipeline voll ist oder keine Daten mehr kommen,
        // schreiben wir den ältesten fertigen Block sequenziell auf das Zielmedium.
        if (!pipeline.empty())
        {
            // get() blockiert exakt so lange, bis dieser spezifische Block fertig ist.
            // Da er als Erster gestartet wurde, ist er meistens bereits fertig berechnet.
            AsyncBlockResult result = pipeline.front().get();
            pipeline.pop();

            // Sequenzielles Schreiben des Bzip3-Blockformats
            m_fout.write(&result.compressed_size, sizeof(result.compressed_size));
            m_fout.write(&result.original_size, sizeof(result.original_size));
            m_fout.write(result.buffer.data(), result.compressed_size);


        }
    }

    return true;
}

/*
int main()
{
    try
    {
        std::string input = "huge_dataset.tar";
        std::string output = "huge_dataset.tar.bz3";

        std::cout << "Starte maximale Kompression..." << std::endl;
        if (compress_file_bz3_max_speed(input, output, 16)) {
            std::cout << "Kompression mit maximaler Performance abgeschlossen!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
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

*/
