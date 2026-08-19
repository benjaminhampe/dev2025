#pragma once

#include <zstd.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

static uint32_t readUint32(std::ifstream& in) {
    uint32_t v;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    return v;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: decompress <archive.zst>\n";
        return 1;
    }

    fs::path archivePath = argv[1];

    // Create output folder based on archive base name
    fs::path outFolder = archivePath.stem();
    fs::create_directories(outFolder);

    std::ifstream in(archivePath, std::ios::binary);
    if (!in) {
        std::cerr << "Cannot open archive\n";
        return 1;
    }

    ZSTD_DCtx* dctx = ZSTD_createDCtx();

    std::vector<uint8_t> inBuf(1 << 16);
    std::vector<uint8_t> outBuf(1 << 16);

    while (true) {
        if (!in.good()) break;

        // Read header: path length
        uint32_t pathLen = 0;
        in.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
        if (!in.good()) break; // end of archive

        std::string relPath(pathLen, '\0');
        in.read(relPath.data(), pathLen);

        uint32_t dataLen = readUint32(in);

        // Prepare output file
        fs::path outFile = outFolder / relPath;
        fs::create_directories(outFile.parent_path());
        std::ofstream out(outFile, std::ios::binary);

        // Decompress block
        ZSTD_inBuffer zin = { nullptr, 0, 0 };
        ZSTD_outBuffer zout = { outBuf.data(), outBuf.size(), 0 };

        size_t remaining = dataLen;
        while (remaining > 0) {
            size_t toRead = std::min(remaining, (size_t)inBuf.size());
            in.read(reinterpret_cast<char*>(inBuf.data()), toRead);
            size_t got = in.gcount();
            if (got == 0) break;

            zin.src = inBuf.data();
            zin.size = got;
            zin.pos = 0;

            while (zin.pos < zin.size) {
                zout.pos = 0;
                size_t ret = ZSTD_decompressStream(dctx, &zout, &zin);
                if (ZSTD_isError(ret)) {
                    std::cerr << "ZSTD error: " << ZSTD_getErrorName(ret) << "\n";
                    return 1;
                }
                out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
            }

            remaining -= got;
        }
    }

    ZSTD_freeDCtx(dctx);
    return 0;
}
