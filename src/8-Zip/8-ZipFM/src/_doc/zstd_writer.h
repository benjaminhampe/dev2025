#pragma once
#include <de/archive/FileNames.h>

#include <zstd.h>

// =================================================================
struct CompressOptions
// =================================================================
{
    int quality = 5; // 0 = highest, 9 = lowest

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
compress_zstd( const std::string& input_tar,
            const std::string& output_zst,
            const CompressOptions& options)
{
    de::File tarFile(input_tar, de::eFileMode::Read);
    if (!tarFile.is_open())
    {
        DE_ERROR("TAR file not readable. ", input_tar)
        return false;
    }

    de::File zstFile(output_zst, de::eFileMode::Write);
    if (!zstFile.is_open())
    {
        DE_ERROR("ZStd File not writable. ", output_zst)
        return false;
    }

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx)
    {
        DE_ERROR("No ZStd context. ", output_zst)
        return false;
    }

    size_t ret = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 5);
    if (ZSTD_isError(ret))
    {
        DE_ERROR("Invalid ZStd compression level")
        ZSTD_freeCCtx(cctx);
        return false;
    }

    ZSTD_inBuffer zin;
    ZSTD_outBuffer zout;

    de::Blob outBuf(1 << 20); // 1MB
    de::Blob inBlob(1 << 20);

    // Header schreiben:
    // U32 path length
    // VAR path
    // U64 data length -> so files are limited to 4GB each.
    uint32_t tarNameSize = input_tar.size();
    zstFile.write(&tarNameSize, sizeof(uint32_t));
    zstFile.write(input_tar.data(), input_tar.size());
    uint64_t tarDataSize = static_cast<uint64_t>(tarFile.size());
    zstFile.write(&tarDataSize, sizeof(uint64_t));

    int64_t readBytes = 0;
    while (readBytes < tarDataSize)
    {
        int64_t has = std::min<int64_t>(tarDataSize - readBytes, inBlob.size());
        int64_t got = tarFile.read(inBlob.data(), has);

        readBytes += got;

        options.onProgress( 100.0 * double(readBytes) / double(tarDataSize) );

        // Input setzen
        zin.src = inBlob.data();
        zin.size = got;
        zin.pos = 0;

        // Streamen
        while (zin.pos < zin.size)
        {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            size_t ret = ZSTD_compressStream2(
                            cctx,
                            &zout,
                            &zin,
                            ZSTD_e_continue);

            if (ZSTD_isError(ret))
            {
                DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret))
                return false;
            }

            // out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
            zstFile.write(outBuf.data(), zout.pos);
        }
    }

    // Final flush
    {
        ZSTD_inBuffer empty = { nullptr, 0, 0 };
        size_t remaining = 1;
        while (remaining)
        {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            remaining = ZSTD_compressStream2(cctx, &zout, &empty, ZSTD_e_end);
            //out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
            zstFile.write(outBuf.data(), zout.pos);
        }
    }

    ZSTD_freeCCtx(cctx);
    return true;
}


#if 0


inline bool
compress_zstd( const std::string& input_tar,
            const std::string& output_zst,
            const CompressOptions& options)
{
    if (dbExistFile(input_tar))
    {
        DE_ERROR("No input_tar. ", input_tar)
        return false;
    }

    de::File file(output_zst, de::eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("File not writable. ", output_zst)
        return false;
    }

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx)
    {
        DE_ERROR("No context. ", output_zst)
        return false;
    }

    size_t ret = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 5);
    if (ZSTD_isError(ret))
    {
        DE_ERROR("Invalid compression level")
        ZSTD_freeCCtx(cctx);
        return false;
    }

    ZSTD_inBuffer zin;
    ZSTD_outBuffer zout;

    de::Blob outBuf(1 << 20); // 1MB
    de::Blob inputBlob;

    int64_t iterCount = 0;
    int64_t fileCount = 0;
    for (const auto& fileName : fileNames)
    {
        inputBlob.clear();
        if (!dbLoadBlob(inputBlob, fileName))
        {
            DE_ERROR("[",iterCount,"] ",fileName)
        }
        else
        {
            DE_DEBUG("Blob(", inputBlob.size(), "), ",fileName)
            // Header schreiben:
            // U32 path length
            // VAR path
            // U64 data length -> so files are limited to 4GB each.
            uint32_t fileNameSize = fileName.size();
            file.write(&fileNameSize, sizeof(uint32_t));
            file.write(fileName.data(), fileName.size());
            uint64_t blobSize = inputBlob.size();
            file.write(&blobSize, sizeof(uint64_t));

            // Input setzen
            zin.src = inputBlob.data();
            zin.size = inputBlob.size();
            zin.pos = 0;

            // Streamen
            while (zin.pos < zin.size)
            {
                zout.dst = outBuf.data();
                zout.size = outBuf.size();
                zout.pos = 0;

                size_t ret = ZSTD_compressStream2(
                                cctx,
                                &zout,
                                &zin,
                                ZSTD_e_continue);

                if (ZSTD_isError(ret))
                {
                    DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret))
                    return false;
                }

                // out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
                file.write(outBuf.data(), zout.pos);
            }
        }
    }

    // Final flush
    {
        ZSTD_inBuffer empty = { nullptr, 0, 0 };
        size_t remaining = 1;
        while (remaining)
        {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            remaining = ZSTD_compressStream2(cctx, &zout, &empty, ZSTD_e_end);
            //out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
            file.write(outBuf.data(), zout.pos);
        }
    }

    ZSTD_freeCCtx(cctx);
    return true;
}

#include <de/Core.h>
#include <de/AlignedMemory.h>

#include <zstd.h>

// #include <filesystem>
// #include <fstream>
// #include <vector>
// #include <iostream>

namespace fs = std::filesystem;

struct FileEntry
{
    std::string uri;
    std::vector<uint8_t> data;
};

static bool
readFile(const std::string& uri, de::TAlignedVector<uint8_t>& data)
{
    // std::ifstream f(p, std::ios::binary);
    // f.seekg(0, std::ios::end);
    // size_t size = f.tellg();
    // f.seekg(0, std::ios::beg);

    // out.data.resize(size);
    // f.read(reinterpret_cast<char*>(out.data.data()), size);

    if (dbLoadBlob(
}

static void collect(const fs::path& root, std::vector<FileEntry>& out)
{
    if (fs::is_regular_file(root))
    {
        FileEntry e;
        e.uri = root.string();
        readFile(root, e);
        out.push_back(std::move(e));
        return;
    }

    for (auto& entry : fs::recursive_directory_iterator(root))
    {
        if (fs::is_regular_file(entry.path()))
        {
            FileEntry e;
            e.uri = fs::relative(entry.path(), root).string();
            readFile(entry.path(), e);
            out.push_back(std::move(e));
        }
    }
}

static void writeUint32(std::ofstream& out, uint32_t v) {
    out.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: compress <file_or_folder> <output.zst>\n";
        return 1;
    }

    fs::path input = argv[1];
    fs::path output = argv[2];

    std::vector<FileEntry> files;
    collect(input, files);

    std::ofstream out(output, std::ios::binary);

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 5);

    ZSTD_inBuffer zin;
    ZSTD_outBuffer zout;

    std::vector<uint8_t> outBuf(1 << 16);

    for (auto& f : files)
    {
        // Write header: path length + path + data length
        writeUint32(out, (uint32_t)f.uri.size());
        out.write(f.uri.data(), f.uri.size());
        writeUint32(out, (uint32_t)f.data.size());

        zin.src = f.data.data();
        zin.size = f.data.size();
        zin.pos = 0;

        while (zin.pos < zin.size)
        {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            size_t ret = ZSTD_compressStream2(
                            cctx,
                            &zout,
                            &zin,
                            ZSTD_e_continue);

            if (ZSTD_isError(ret))
            {
                DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret))
                return 1;
            }

            out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
        }
    }

    // Final flush
    {
        ZSTD_inBuffer empty = { nullptr, 0, 0 };
        size_t remaining = 1;
        while (remaining) {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            remaining = ZSTD_compressStream2(cctx, &zout, &empty, ZSTD_e_end);
            out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
        }
    }

    ZSTD_freeCCtx(cctx);
    return 0;
}

#endif
