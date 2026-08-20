#pragma once
#include <de/archive/FileNames.h>

#include <zstd.h>

#include <filesystem>

// =================================================================
struct ExtractOptions
// =================================================================
{
    bool bDebug = false;
    bool bThrowOnFail = false;
    std::atomic<bool>* bCancelFlag = nullptr;

    // Used to update GUI (possibly from a background thread)

    // typedef std::function<void (int)> FN_onProgress;
    // typedef std::function<void (int,std::string)>
    typedef void (*FN_onProgress)(int);
    typedef void (*FN_onLogger)(int,std::string);

    enum eLogLevel { Debug = 0, Ok, Warn, Error };

    FN_onProgress onProgress = nullptr; // [](int percent){};
    FN_onLogger onLogger = nullptr; // [](int level, std::string msg){};
};

inline bool
decompress_zstd( const std::string& input_zst,
              const std::string& output_dir,
              const ExtractOptions& options)
{
    namespace fs = std::filesystem;

    fs::path archivePath = fs::u8path(input_zst);

    // Create output folder based on archive base name
    fs::path outFolder = archivePath.stem();
    if (output_dir.empty())
    {
        outFolder = fs::u8path(output_dir);
    }

    fs::create_directories(outFolder);

    if (!dbExistDirectory(outFolder.u8string()))
    {
        DE_ERROR("Cannot create output_dir ", outFolder.u8string())
        return false;
    }

    de::File iFile(input_zst, de::eFileMode::Read);
    if (!iFile.is_open())
    {
        DE_ERROR("Cannot open archive. ", input_zst)
        return false;
    }

    ZSTD_DCtx* dctx = ZSTD_createDCtx();

    de::Blob inBuf(1 << 23);
    de::Blob outBuf(1 << 23);

    while (true)
    {
        //if (!in.good()) break;

        // Read header: path length
        uint32_t pathLen = 0;
        //in.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
        //if (!in.good()) break; // end of archive

        int r = iFile.read(&pathLen,4);
        if (r < 4)
        {
            DE_WARN("Cannot read pathLen, end of archive")
            // return false;
            break;
        }

        // in.read(relPath.data(), pathLen);
        std::string relPath(pathLen, '\0');
        int r2 = iFile.read(relPath.data(), pathLen);
        if (r2 < pathLen)
        {
            DE_ERROR("Cannot read path, end of archive")
            // return false;
            break;
        }

        // uint32_t dataLen = readUint32(in);
        uint32_t dataLen = 0;
        int r3 = iFile.read(&dataLen,4);
        if (r3 < 4)
        {
            DE_WARN("Cannot read dataLen, end of archive")
            // return false;
            break;
        }

        // Prepare output file
        fs::path outFile = outFolder / relPath;
        fs::path relDir = outFile.parent_path();

        fs::create_directories(relDir);

        if (!dbExistDirectory(relDir.u8string()))
        {
            DE_ERROR("Cannot create relDir ", relDir.u8string())
            break;
        }

        // std::ofstream out(outFile, std::ios::binary);

        std::string oUri = outFile.u8string();
        de::File oFile(oUri, de::eFileMode::Write);
        if (!oFile.is_open())
        {
            DE_ERROR("Cannot write oUri. ", oUri)
            break;
        }

        // Decompress block
        ZSTD_inBuffer zin = { nullptr, 0, 0 };
        ZSTD_outBuffer zout = { outBuf.data(), outBuf.size(), 0 };

        size_t remaining = dataLen;
        while (remaining > 0)
        {
            size_t toRead = std::min(remaining, (size_t)inBuf.size());
            //in.read(reinterpret_cast<char*>(inBuf.data()), toRead);
            //size_t got = in.gcount();
            //if (got == 0) break;

            int got = iFile.read(inBuf.data(), toRead);
            if (got < 0)
            {
                DE_ERROR("got < 0, got = ",got)
                break;
            }

            if (got == 0)
            {
                DE_BENNI("EOF ",oUri)
                break;
            }

            zin.src = inBuf.data();
            zin.size = got;
            zin.pos = 0;

            while (zin.pos < zin.size)
            {
                zout.pos = 0;
                size_t ret = ZSTD_decompressStream(dctx, &zout, &zin);
                if (ZSTD_isError(ret))
                {
                    DE_ERROR("ZSTD error: ", ZSTD_getErrorName(ret))
                    break;
                }
                // out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
                size_t ret2 = oFile.write(outBuf.data(), zout.pos);
                if (ret2 != zout.pos)
                {
                    DE_ERROR("ret2(",ret2,") != zout.pos(",zout.pos,"). ", oUri)
                    break;
                }
            }

            remaining -= got;
        }
    }

    ZSTD_freeDCtx(dctx);
    return 0;
}


#if 0

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

#endif
