#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <zstd.h>

// Struktur für bekannte Magic Bytes
struct FileSignature {
    const char* extension;
    const unsigned char* signature;
    size_t length;
    size_t offset; // Manche Formate (wie Tar) haben ihre Signatur nicht bei Byte 0
};

// Definition gängiger Signaturen
const unsigned char TAR_SIG[] = { 0x75, 0x73, 0x74, 0x61, 0x72 }; // "ustar" bei Offset 257
const unsigned char PNG_SIG[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
const unsigned char JPG_SIG[] = { 0xFF, 0xD8, 0xFF };
const unsigned char PDF_SIG[] = { 0x25, 0x50, 0x44, 0x46 }; // "%PDF"
const unsigned char ZIP_SIG[] = { 0x50, 0x4B, 0x03, 0x04 }; // "PK.."
const unsigned char GZ_SIG[]  = { 0x1F, 0x8B };

const FileSignature KNOWN_SIGNATURES[] = {
    { ".tar", TAR_SIG, sizeof(TAR_SIG), 257 },
    { ".png", PNG_SIG, sizeof(PNG_SIG), 0 },
    { ".jpg", JPG_SIG, sizeof(JPG_SIG), 0 },
    { ".pdf", PDF_SIG, sizeof(PDF_SIG), 0 },
    { ".zip", ZIP_SIG, sizeof(ZIP_SIG), 0 },
    { ".gz",  GZ_SIG,  sizeof(GZ_SIG),  0 }
};

/**
 * Erkennt die Dateiendung anhand der dekomprimierten Magic Bytes.
 */
std::string detect_extension(const uint8_t* buffer, size_t size) {
    for (const auto& sig : KNOWN_SIGNATURES) {
        // Prüfen, ob der Puffer groß genug für den Offset und die Signatur ist
        if (size >= (sig.offset + sig.length)) {
            if (std::memcmp(buffer + sig.offset, sig.signature, sig.length) == 0) {
                return sig.extension;
            }
        }
    }
    return ".unknown"; // Unbekannter Typ oder reiner Text
}

/**
 * Liest den Anfang einer .zst-Datei und bestimmt den inneren Dateityp.
 * 
 * @param zst_path Pfad zur .zst Datei
 * @return Die erkannte Dateiendung (z.B. ".tar") oder ".unknown"
 */
std::string detect_inner_file_type(const std::string& zst_path) {
    std::ifstream file(zst_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte Datei nicht oeffnen: " << zst_path << std::endl;
        return ".error";
    }

    // Wir dekomprimieren 1 KB (1024 Bytes). Das reicht für die meisten Header völlig aus.
    // Ein Tar-Archiv benötigt mindestens 262 Bytes für die "ustar"-Kennung.
    const size_t target_decompressed_size = 1024;
    std::vector<uint8_t> decompressed_buffer(target_decompressed_size);

    // Zstd-Streaming-Kontext initialisieren
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    if (!dctx) return ".error";

    // Puffer für komprimierte Eingabedaten vorbereiten
    size_t const in_buf_size = ZSTD_DStreamInSize();
    std::vector<uint8_t> in_buffer(in_buf_size);

    ZSTD_outBuffer output = { decompressed_buffer.data(), target_decompressed_size, 0 };
    bool detection_done = false;
    std::string result = ".unknown";

    while (!file.eof() && output.pos < output.size) {
        file.read(reinterpret_cast<char*>(in_buffer.data()), in_buf_size);
        size_t read_bytes = file.gcount();
        if (read_bytes == 0) break;

        ZSTD_inBuffer input = { in_buffer.data(), read_bytes, 0 };

        // Solange dekomprimieren, bis das Eingabestück leer ist oder unser Ziel-Puffer voll ist
        while (input.pos < input.size && output.pos < output.size) {
            size_t const ret = ZSTD_decompressStream(dctx, &output, &input);
            if (ZSTD_isError(ret)) {
                std::cerr << "Zstd Dekompressionsfehler: " << ZSTD_getErrorName(ret) << std::endl;
                ZSTD_freeDCtx(dctx);
                return ".error";
            }
        }
        
        // Sobald wir genug Daten haben, brechen wir ab
        if (output.pos >= 512) { // 512 Bytes reichen dicke für einen Tar-Header
            result = detect_extension(decompressed_buffer.data(), output.pos);
            detection_done = true;
            break;
        }
    }

    // Falls die Datei sehr klein war und der Loop endete, bevor 512 Bytes erreicht wurden
    if (!detection_done && output.pos > 0) {
        result = detect_extension(decompressed_buffer.data(), output.pos);
    }

    ZSTD_freeDCtx(dctx);
    return result;
}

int main() {
    std::string filename = "archiv_umbenannt.zst"; // Eine Zst-Datei ohne verräterischen Namen
    
    std::string ext = detect_inner_file_type(filename);
    std::cout << "Der Inhalt der Datei hat den Typ: " << ext << std::endl;
    
    return 0;
}
