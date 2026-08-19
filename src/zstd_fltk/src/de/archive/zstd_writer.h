#pragma once
#include <de/Core.h>
#include <DarkImage.h>
#include <filesystem>
#include <zstd.h>

typedef std::vector<std::string> FileNames;

inline void dumpFileNames(const FileNames & fileNames)
{
    DE_BENNI("FileNames.Count = ",fileNames.size())
    for (size_t i = 0; i < fileNames.size(); ++i )
    {
        DE_DEBUG("FileNames[",i,"] ",fileNames[i])
    }
}

namespace fs = std::filesystem;

inline void collectFileNames(const std::string& uri, FileNames& fileNames)
{
    fs::path root = fs::u8path(uri);
    if (fs::is_regular_file(root))
    {
        fileNames.push_back(root.string());
        return;
    }

    for (auto& entry : fs::recursive_directory_iterator(root))
    {
        if (fs::is_regular_file(entry.path()))
        {
            auto fileName = fs::relative(entry.path(), root).string();
            fileNames.push_back(std::move(fileName));
        }
    }

    DE_BENNI("BaseDir = ",uri)
    dumpFileNames(fileNames);
}

// static void writeUint32(std::ofstream& out, uint32_t v)
// {
//     out.write(reinterpret_cast<const char*>(&v), sizeof(v));
// }

struct SaveOptions
{
    de::ImageSaveOptions opt;
};

inline int
write_zstd( std::string input_file_or_folder,
            std::string output_zst)
{
    FileNames fileNames;
    collectFileNames(input_file_or_folder, fileNames);

    de::File file(output_zst, de::eFileMode::Write);

    //std::ofstream out(output, std::ios::binary);

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 5);

    ZSTD_inBuffer zin;
    ZSTD_outBuffer zout;

    de::Blob outBuf(1 << 23);

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
            // Write header:
            // u32 path length
            // nox path
            // u32 data length -> so files are limited to 4GB each.
            uint32_t fileNameSize = fileName.size();
            file.write(&fileNameSize, sizeof(uint32_t));
            file.write(fileName.data(), fileName.size());
            uint32_t blobSize = inputBlob.size();
            file.write(&blobSize, sizeof(uint32_t));
            zin.src = inputBlob.data();
            zin.size = inputBlob.size();
            zin.pos = 0;

            // Write header: path length + path + data length
            //writeUint32(out, (uint32_t)f.uri.size());
            //out.write(f.uri.data(), f.uri.size());
            //writeUint32(out, (uint32_t)f.data.size());

            // zin.src = f.data.data();
            // zin.size = f.data.size();
            // zin.pos = 0;

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
    return 0;
}


#if 0


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
