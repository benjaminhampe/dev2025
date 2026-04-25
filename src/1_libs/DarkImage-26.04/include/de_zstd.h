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


