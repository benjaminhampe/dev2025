#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

int compress_tar_file(const char* input_path, const char* output_path, int compression_level) {
    FILE *fin = fopen(input_path, "rb");
    if (!fin) {
        perror("Failed to open input file");
        return 1;
    }

    fseek(fin, 0, SEEK_END);
    size_t input_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    void* input_buffer = malloc(input_size);
    if (!input_buffer) {
        perror("Failed to allocate input buffer");
        fclose(fin);
        return 1;
    }

    fread(input_buffer, 1, input_size, fin);
    fclose(fin);

    size_t max_compressed_size = ZSTD_compressBound(input_size);
    void* compressed_buffer = malloc(max_compressed_size);
    if (!compressed_buffer) {
        perror("Failed to allocate compressed buffer");
        free(input_buffer);
        return 1;
    }

    size_t compressed_size = ZSTD_compress(compressed_buffer, max_compressed_size,
                                           input_buffer, input_size, compression_level);
    if (ZSTD_isError(compressed_size)) {
        fprintf(stderr, "Compression error: %s\n", ZSTD_getErrorName(compressed_size));
        free(input_buffer);
        free(compressed_buffer);
        return 1;
    }

    FILE *fout = fopen(output_path, "wb");
    if (!fout) {
        perror("Failed to open output file");
        free(input_buffer);
        free(compressed_buffer);
        return 1;
    }

    fwrite(compressed_buffer, 1, compressed_size, fout);
    fclose(fout);

    free(input_buffer);
    free(compressed_buffer);
    return 0;
}
