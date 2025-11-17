#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

/*

int main() {
    compress_tar_file("archive.tar", "archive.tar.zst", 3);
    decompress_tar_file("archive.tar.zst", "archive_restored.tar");
    return 0;
}

*/

int decompress_tar_file(const char* input_path, const char* output_path) {
    FILE *fin = fopen(input_path, "rb");
    if (!fin) {
        perror("Failed to open input file");
        return 1;
    }

    fseek(fin, 0, SEEK_END);
    size_t compressed_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    void* compressed_buffer = malloc(compressed_size);
    if (!compressed_buffer) {
        perror("Failed to allocate compressed buffer");
        fclose(fin);
        return 1;
    }

    fread(compressed_buffer, 1, compressed_size, fin);
    fclose(fin);

    unsigned long long decompressed_size = ZSTD_getFrameContentSize(compressed_buffer, compressed_size);
    if (decompressed_size == ZSTD_CONTENTSIZE_ERROR || decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        fprintf(stderr, "Invalid or unknown decompressed size\n");
        free(compressed_buffer);
        return 1;
    }

    void* decompressed_buffer = malloc((size_t)decompressed_size);
    if (!decompressed_buffer) {
        perror("Failed to allocate decompressed buffer");
        free(compressed_buffer);
        return 1;
    }

    size_t result = ZSTD_decompress(decompressed_buffer, decompressed_size,
                                    compressed_buffer, compressed_size);
    if (ZSTD_isError(result)) {
        fprintf(stderr, "Decompression error: %s\n", ZSTD_getErrorName(result));
        free(compressed_buffer);
        free(decompressed_buffer);
        return 1;
    }

    FILE *fout = fopen(output_path, "wb");
    if (!fout) {
        perror("Failed to open output file");
        free(compressed_buffer);
        free(decompressed_buffer);
        return 1;
    }

    fwrite(decompressed_buffer, 1, decompressed_size, fout);
    fclose(fout);

    free(compressed_buffer);
    free(decompressed_buffer);
    return 0;
}
