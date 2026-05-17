#pragma once

/*
You picked the tricky one—FLAC is pull‑based in libFLAC, while your MP4 iterator is push‑style. The way to bridge that is: make libFLAC’s read callback pull from your iterator.

I’ll keep it tight but complete.
1. What MP4 + FLAC looks like

In MP4, FLAC usually appears as:

    SampleEntry type: fLaC (or similar)

    A FLAC‑specific box containing:

        STREAMINFO (mandatory)

        Optional metadata blocks (Vorbis comments, seektable, etc.)

    Each MP4 sample = one FLAC frame (or at least a chunk of the FLAC bitstream)

You must parse the FLAC config box and extract the FLAC stream header (the fLaC marker + metadata blocks).
2. High‑level idea

libFLAC wants:

    A read callback: size_t read(void* buffer, size_t bytes, void* client_data)

    A write callback: gives you decoded PCM

    You call FLAC__stream_decoder_process_until_end_of_stream()

Your read callback will:

    Serve the FLAC header bytes first (from the FLAC config box)

    Then serve FLAC frames by pulling MP4 samples from your iterator

3. State for the read callback
cpp

struct FlacMp4Source {
    // FLAC header (STREAMINFO + metadata blocks) as a full FLAC stream prefix
    std::vector<uint8_t> header;
    size_t header_pos = 0;

    SampleIterator* it;   // your MP4 iterator
    FileReader* file;     // your file reader

    std::vector<uint8_t> sample_buf;
    size_t sample_pos = 0;
    bool   eof = false;
};

4. Read callback
cpp

FLAC__StreamDecoderReadStatus flac_read_cb(
    const FLAC__StreamDecoder*,
    FLAC__byte buffer[],
    size_t* bytes,
    void* client_data)
{
    FlacMp4Source* src = (FlacMp4Source*)client_data;
    size_t to_read = *bytes;
    size_t written = 0;

    if (to_read == 0) {
        *bytes = 0;
        return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }

    // 1) Serve header first
    while (to_read && src->header_pos < src->header.size()) {
        buffer[written++] = src->header[src->header_pos++];
        to_read--;
    }

    // 2) Then serve samples as FLAC frames
    while (to_read && !src->eof) {
        if (src->sample_pos >= src->sample_buf.size()) {
            // need next sample
            Sample s;
            if (!src->it->next(s)) {
                src->eof = true;
                break;
            }

            src->sample_buf.resize(s.size);
            src->file->seek(s.file_offset);
            src->file->read_bytes(src->sample_buf.data(), s.size);
            src->sample_pos = 0;
        }

        size_t avail = src->sample_buf.size() - src->sample_pos;
        size_t chunk = (avail < to_read) ? avail : to_read;

        memcpy(buffer + written,
               src->sample_buf.data() + src->sample_pos,
               chunk);

        src->sample_pos += chunk;
        written += chunk;
        to_read -= chunk;
    }

    *bytes = written;

    if (written == 0) {
        return src->eof
            ? FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM
            : FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

5. Write callback (PCM out)
cpp

FLAC__StreamDecoderWriteStatus flac_write_cb(
    const FLAC__StreamDecoder*,
    const FLAC__Frame* frame,
    const FLAC__int32* const buffer[],
    void* client_data)
{
    // buffer[c][i] = sample i of channel c, 32-bit signed
    // frame->header.blocksize = samples per channel

    // You can interleave into your own float/int buffer here.
    // Example: interleave to float32
    FlacMp4Source* src = (FlacMp4Source*)client_data;
    uint32_t channels = frame->header.channels;
    uint32_t n = frame->header.blocksize;

    // user-provided output buffer / callback would go here

    // Example interleave (pseudo):
    // for i in 0..n-1:
    //   for c in 0..channels-1:
    //     out[(i*channels)+c] = buffer[c][i] * (1.0f / 2147483648.0f);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

You also need trivial metadata and error callbacks (can be no‑ops or logging).
6. Decoder setup
cpp

FLAC__StreamDecoder* dec = FLAC__stream_decoder_new();

FlacMp4Source src;
src.header = flac_header_bytes; // built from FLAC config box
src.it     = &iterator;
src.file   = &file;

FLAC__stream_decoder_init_stream(
    dec,
    flac_read_cb,
    nullptr, // seek
    nullptr, // tell
    nullptr, // length
    nullptr, // eof
    flac_write_cb,
    nullptr, // metadata
    nullptr, // error
    &src
);

// Decode all
FLAC__stream_decoder_process_until_end_of_stream(dec);
FLAC__stream_decoder_delete(dec);

7. The only “magic” piece

You must construct flac_header_bytes correctly:

    Start with "fLaC" magic

    Append STREAMINFO block (from MP4 FLAC box)

    Append any other metadata blocks (Vorbis comments, seektable, etc.)

Then libFLAC sees a normal FLAC stream, and your read callback simply concatenates:

    header bytes + all MP4 FLAC samples

and libFLAC does the rest.

If you want, next I can show you how to build flac_header_bytes from the MP4 FLAC box layout.
*/