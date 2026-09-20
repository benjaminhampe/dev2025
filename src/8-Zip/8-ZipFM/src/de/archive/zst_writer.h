#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

/**
 * Compresses a file using multi-threaded Zstd.
 *
 * @param source       Path to the input file (e.g., "archive.tar")
 * @param dest         Path to the output file (e.g., "archive.tar.zst")
 * @param compressionLevel The Zstd compression level (1 to 19, 0 for default)
 * @param nbThreads    Number of threads to use (0 auto-detects based on CPU cores)
 * @return             0 on success, 1 on failure
 */
inline int compress_file_mt(const char* source, const char* dest, int compressionLevel, int nbThreads) {
    FILE* fin = fopen(source, "rb");
    FILE* fout = fopen(dest, "wb");

    if (!fin || !fout) {
        fprintf(stderr, "Error: Standard I/O opening failed.\n");
        if (fin) fclose(fin);
        if (fout) fclose(fout);
        return 1;
    }

    // Allocate multi-threaded context
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) {
        fprintf(stderr, "Error: ZSTD_createCCtx() failed.\n");
        fclose(fin);
        fclose(fout);
        return 1;
    }

    // Configure the compression parameters
    // 1. Set the compression level
    size_t const cLevelError = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compressionLevel);
    if (ZSTD_isError(cLevelError)) {
        fprintf(stderr, "Error setting compression level: %s\n", ZSTD_getErrorName(cLevelError));
        goto error_cleanup;
    }

    // 2. Enable multithreading by setting the number of workers
    // Passing 0 tells libzstd to automatically use all available CPU cores.
    size_t const threadError = ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, nbThreads);
    if (ZSTD_isError(threadError)) {
        fprintf(stderr, "Error setting thread count: %s\n", ZSTD_getErrorName(threadError));
        goto error_cleanup;
    }

    // Setup streaming buffers using recommended sizes
    size_t const buffInSize = ZSTD_CStreamInSize();
    size_t const buffOutSize = ZSTD_CStreamOutSize();
    void* const buffIn = malloc(buffInSize);
    void* const buffOut = malloc(buffOutSize);

    if (!buffIn || !buffOut) {
        fprintf(stderr, "Error: Memory allocation for buffers failed.\n");
        free(buffIn);
        free(buffOut);
        goto error_cleanup;
    }

    size_t readLen;
    int success = 1; // Tracks overall streaming success

    // Main streaming loop
    while ((readLen = fread(buffIn, 1, buffInSize, fin)) > 0) {
        ZSTD_inBuffer input = { buffIn, readLen, 0 };

        // Push data to the compressor until the input buffer is fully consumed
        while (input.pos < input.size) {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };

            size_t const toRead = ZSTD_compressStream2(cctx, &output, &input, ZSTD_e_continue);
            if (ZSTD_isError(toRead)) {
                fprintf(stderr, "Compression error: %s\n", ZSTD_getErrorName(toRead));
                success = 0;
                break;
            }

            // Write out the compressed data chunk
            if (output.pos > 0) {
                fwrite(buffOut, 1, output.pos, fout);
            }
        }
        if (!success) break;
    }

    // Flush and finish the frame
    if (success) {
        ZSTD_inBuffer input = { NULL, 0, 0 };
        size_t remainingToFlush;

        do {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
            // ZSTD_e_end signs off the frame, creating the final Zstd file trailer
            remainingToFlush = ZSTD_compressStream2(cctx, &output, &input, ZSTD_e_end);

            if (ZSTD_isError(remainingToFlush)) {
                fprintf(stderr, "Flush error: %s\n", ZSTD_getErrorName(remainingToFlush));
                success = 0;
                break;
            }

            if (output.pos > 0) {
                fwrite(buffOut, 1, output.pos, fout);
            }
        } while (remainingToFlush > 0); // Keep flushing until 0 tokens remain
    }

    // Cleanup resources
    free(buffIn);
    free(buffOut);
    ZSTD_freeCCtx(cctx);
    fclose(fin);
    fclose(fout);

    return success ? 0 : 1;

error_cleanup:
    ZSTD_freeCCtx(cctx);
    fclose(fin);
    fclose(fout);
    return 1;
}

// Example usage
int main() {
    printf("Starting multi-threaded compression...\n");
    // Compresses backup.tar to backup.tar.zst using default level (3) and all CPU cores (0)
    int result = compress_file_mt("backup.tar", "backup.tar.zst", 3, 0);

    if (result == 0) {
        printf("Compression completed successfully!\n");
    } else {
        printf("Compression failed.\n");
    }
    return result;
}
