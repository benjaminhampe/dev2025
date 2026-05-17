#pragma once

/*

Here’s a full, concrete MP4 → H.264 decode pipeline wired to the stuff you already have: avc1/avcC parser, chunk/sample iterator, etc.

I’ll use FFmpeg/libavcodec as the decoder, but the NAL handling is the same for OpenH264 or others.
1. Inputs you already have

From your MP4 parser:

    Track tables: stsc, stco/co64, stsz/stz2, stts, ctts

    Sample iterator: yields Sample { file_offset, size, dts, cts, sample_description_index }

    avc1 sample entry with parsed avcC:

cpp

struct AVCDecoderConfigurationRecord {
    uint8_t configurationVersion;
    uint8_t AVCProfileIndication;
    uint8_t profile_compatibility;
    uint8_t AVCLevelIndication;
    uint8_t lengthSizeMinusOne; // NAL length size = (value & 3) + 1

    std::vector<std::vector<uint8_t>> sps_list;
    std::vector<std::vector<uint8_t>> pps_list;
};

2. Build global SPS/PPS (Annex B)

You want SPS/PPS as Annex B NALs:
cpp

std::vector<uint8_t> build_sps_pps_annexb(const AVCDecoderConfigurationRecord& avcC)
{
    static const uint8_t start_code[4] = {0,0,0,1};
    std::vector<uint8_t> out;

    for (auto& sps : avcC.sps_list) {
        out.insert(out.end(), start_code, start_code + 4);
        out.insert(out.end(), sps.begin(), sps.end());
    }
    for (auto& pps : avcC.pps_list) {
        out.insert(out.end(), start_code, start_code + 4);
        out.insert(out.end(), pps.begin(), pps.end());
    }
    return out;
}

3. Convert one MP4 sample → Annex B

MP4 stores NALs as:
Code

[length][NAL][length][NAL]...

Length field size = (avcC.lengthSizeMinusOne & 3) + 1.

Convert to Annex B:
cpp

void mp4_sample_to_annexb(
    const uint8_t* src,
    uint32_t size,
    int nal_length_size,
    std::vector<uint8_t>& out)
{
    static const uint8_t start_code[4] = {0,0,0,1};
    out.clear();

    uint32_t pos = 0;
    while (pos + nal_length_size <= size) {
        uint32_t nal_len = 0;
        for (int i = 0; i < nal_length_size; ++i)
            nal_len = (nal_len << 8) | src[pos + i];
        pos += nal_length_size;
        if (pos + nal_len > size)
            break;

        out.insert(out.end(), start_code, start_code + 4);
        out.insert(out.end(), src + pos, src + pos + nal_len);
        pos += nal_len;
    }
}

4. Initialize libavcodec (H.264 decoder)
cpp

AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
AVCodecContext* ctx = avcodec_alloc_context3(codec);

// Optional: set width/height from avc1
ctx->width  = avc1.width;
ctx->height = avc1.height;

// No extradata needed if we prepend SPS/PPS in-band
if (avcodec_open2(ctx, codec, nullptr) < 0) {
    // handle error
}

5. Decode loop: MP4 → Annex B → libavcodec
cpp

int nal_length_size = (avcC.lengthSizeMinusOne & 3) + 1;
std::vector<uint8_t> spspps = build_sps_pps_annexb(avcC);
std::vector<uint8_t> sample_annexb;

bool sent_spspps = false;

Sample s;
AVPacket pkt;
AVFrame* frame = av_frame_alloc();

while (iterator.next(s)) {
    // Read raw MP4 sample
    std::vector<uint8_t> sample(s.size);
    file.seek(s.file_offset);
    file.read_bytes(sample.data(), s.size);

    // Convert to Annex B
    mp4_sample_to_annexb(sample.data(), s.size, nal_length_size, sample_annexb);

    // Prepend SPS/PPS before first frame
    std::vector<uint8_t> full;
    if (!sent_spspps) {
        full.reserve(spspps.size() + sample_annexb.size());
        full.insert(full.end(), spspps.begin(), spspps.end());
        full.insert(full.end(), sample_annexb.begin(), sample_annexb.end());
        sent_spspps = true;
    } else {
        full.swap(sample_annexb);
    }

    av_init_packet(&pkt);
    pkt.data = full.data();
    pkt.size = (int)full.size();

    // Optionally set PTS/DTS (convert from track timescale to stream time_base)
    // pkt.pts = ...
    // pkt.dts = ...

    if (avcodec_send_packet(ctx, &pkt) < 0)
        continue;

    while (true) {
        int ret = avcodec_receive_frame(ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        if (ret < 0)
            break;

        // frame->data / frame->linesize contain decoded YUV
        // frame->width, frame->height, frame->format, frame->pts
        // → render or convert to RGB here
    }
}

// Flush
avcodec_send_packet(ctx, nullptr);
while (avcodec_receive_frame(ctx, frame) == 0) {
    // handle remaining frames
}

av_frame_free(&frame);
avcodec_free_context(&ctx);

6. Timing (DTS/PTS)

From your iterator:

    s.dts / s.cts are in track timescale (mdhd.timescale).

    FFmpeg stream time_base is usually {1, timescale} or something you choose.

Convert:
cpp

int64_t to_tb(int64_t ts, int timescale, AVRational tb)
{
    return av_rescale_q(ts, AVRational{1, timescale}, tb);
}

// Example:
pkt.dts = to_tb(s.dts, track_timescale, ctx->time_base);
pkt.pts = to_tb(s.cts, track_timescale, ctx->time_base);

You now have a complete MP4 → H.264 pipeline:

    MP4 demux → sample iterator

    avc1/avcC parse → SPS/PPS + NAL length size

    Length‑prefixed NALs → Annex B

    SPS/PPS prepended once

    Packets fed into libavcodec

    Decoded YUV frames out, with correct PTS/DTS

If you want, next we can tighten this into a zero‑allocation hot path (reusing buffers, no std::vector churn) or adapt it to OpenH264 instead of FFmpeg.

*/