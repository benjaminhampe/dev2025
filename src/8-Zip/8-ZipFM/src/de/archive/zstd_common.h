#include <de_zstd.h>
#include <zstd.h>

// ====================================================
bool
compress_tar_file(
    const char* input_path,
    const char* output_path,
    int32_t compression_level
)
// ====================================================
{
    de::File in_file;
    if (!in_file.open(input_path, de::eFileMode::Read))
    {
        DE_ERROR("Failed to open input file")
        return false;
    }

    std::vector<uint8_t> in_blob(in_file.size());
    in_file.read(in_blob.data(), in_blob.size());

    size_t max_size = ZSTD_compressBound(in_blob.size());

    std::vector<uint8_t> out_blob(max_size);

    size_t out_size = ZSTD_compress(out_blob.data(), out_blob.size(),
                                    in_blob.data(), in_blob.size(),
                                    compression_level);

    if (ZSTD_isError(out_size))
    {
        DE_ERROR("Compression error: ", ZSTD_getErrorName(out_size))
        return false;
    }

    de::File out_file;
    if (!out_file.open(output_path, de::eFileMode::Write))
    {
        DE_ERROR("Failed to open output file");
        return false;
    }

    out_file.write(out_blob.data(), out_blob.size());
    return true;
}


/*

int main() {
    compress_tar_file("archive.tar", "archive.tar.zst", 3);
    decompress_tar_file("archive.tar.zst", "archive_restored.tar");
    return 0;
}

*/
// ====================================================
bool
decompress_tar_file(
    const char* input_path,
    const char* output_path
)
// ====================================================
{
    de::File in_file;
    if (!in_file.open(input_path, de::eFileMode::Read))
    {
        DE_ERROR("Failed to open input file");
        return false;
    }

    std::vector<uint8_t> buf(in_file.size());
    in_file.read(buf.data(), buf.size());
    in_file.close();

    uint64_t out_size = ZSTD_getFrameContentSize(buf.data(), buf.size());

    if (out_size == ZSTD_CONTENTSIZE_ERROR
     || out_size == ZSTD_CONTENTSIZE_UNKNOWN)
    {
        DE_ERROR("Invalid or unknown decompressed size")
        return false;
    }

    std::vector<uint8_t> out(out_size);

    size_t result = ZSTD_decompress(out.data(), out.size(),
                                    buf.data(), buf.size());
    if (ZSTD_isError(result))
    {
        DE_ERROR("Decompression error: ", ZSTD_getErrorName(result))
        return false;
    }


    de::File out_file;
    if (!out_file.open(output_path, de::eFileMode::Write))
    {
        DE_ERROR("Failed to open output file");
        return false;
    }

    out_file.write(out.data(), out.size());
    return true;
}
