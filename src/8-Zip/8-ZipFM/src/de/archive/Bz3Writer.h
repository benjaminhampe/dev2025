#pragma once
#include <string>

// Die schnellstmögliche Kompressions-Funktion mittels asynchroner Pipeline.

bool Bz3_compress_file_max_speed(const std::string& input_uri, const std::string& output_uri, int32_t block_size_mib = 16);

/*
int main() 
{
    try 
    {
        std::string input = "huge_dataset.tar";
        std::string output = "huge_dataset.tar.bz3";

        std::cout << "Starte maximale Kompression..." << std::endl;
        if (Bz3_compress_file_max_speed(input, output, 16)) {
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
