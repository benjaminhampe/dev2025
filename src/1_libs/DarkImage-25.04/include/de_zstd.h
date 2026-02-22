#pragma once
#include <de/Core.h>

bool
compress_tar_file(
    const char* input_path,
    const char* output_path,
    int32_t compression_level
    );

bool
decompress_tar_file(
    const char* input_path,
    const char* output_path
    );

/*

int main()
{
    compress_tar_file("archive.tar", "archive.tar.zst", 3);
    decompress_tar_file("archive.tar.zst", "archive_restored.tar");
    return 0;
}

*/

struct RAII_FILE
{
    FILE* m_file;
    uint64_t m_size;
    std::string m_uri;
    std::string m_flags;

    RAII_FILE()
        : m_file(nullptr), m_size(0)
    {
        // Empty default ctr
    }
    RAII_FILE(std::string const & uri, std::string const & flags)
        : m_file(nullptr), m_size(0), m_uri(uri), m_flags(flags)
    {
        // Empty value ctr
    }

    uint64_t size() const
    {
        return m_size;
    }

    bool open(std::string const & uri, std::string const & flags)
    {
        close();

        m_file = fopen(uri.c_str(), flags.c_str());
        if (!m_file)
        {
            DE_ERROR("Failed to open input file ", uri, " with flags ",flags)
            return false;
        }

        fseek(m_file, 0, SEEK_END);
        m_size = ftell(m_file);
        fseek(m_file, 0, SEEK_SET);

        return true;
    }

    void close()
    {
        if (m_file)
        {
            DE_OK("Close file. ", m_uri)
            fclose(m_file);
            m_file = nullptr;
            m_size = 0;
        }
    }

    void write(const void* __restrict__ pSrc, uint64_t asize)
    {
        if (!m_file) { DE_ERROR("No file.") return; }
        fwrite(pSrc, 1, asize, m_file);
    }

    void read(void* __restrict__ pDst, uint64_t asize)
    {
        if (!m_file) { DE_ERROR("No file.") return; }
        fread(pDst, 1, asize, m_file);
    }

};
