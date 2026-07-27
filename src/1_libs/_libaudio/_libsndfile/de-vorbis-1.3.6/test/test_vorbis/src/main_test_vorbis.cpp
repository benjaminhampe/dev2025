#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <vorbis/vorbisenc.h>
#include <ogg/ogg.h>

int main() {
    vorbis_info vi;
    vorbis_info_init(&vi);

    // 1 kHz Sinus, 1 Sekunde, 48 kHz, 1 Kanal
    vorbis_encode_init_vbr(&vi, 1, 48000, 0.4f);

    vorbis_comment vc;
    vorbis_comment_init(&vc);

    vorbis_dsp_state vd;
    vorbis_block vb;
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);

    ogg_stream_state os;
    ogg_stream_init(&os, 1234);

    ogg_packet h, hc, hcode;
    vorbis_analysis_headerout(&vd, &vc, &h, &hc, &hcode);
    ogg_stream_packetin(&os, &h);
    ogg_stream_packetin(&os, &hc);
    ogg_stream_packetin(&os, &hcode);

    FILE* fp = fopen("test_vorbis.ogg", "wb");
    ogg_page og;
    while (ogg_stream_flush(&os, &og)) {
        fwrite(og.header, 1, og.header_len, fp);
        fwrite(og.body, 1, og.body_len, fp);
    }

    // 1 kHz Sinus generieren
    float** buf = vorbis_analysis_buffer(&vd, 48000);
    for (int i = 0; i < 48000; i++)
        buf[0][i] = sinf(2.0f * 3.14159265f * 1000.0f * (float)i / 48000.0f);

    vorbis_analysis_wrote(&vd, 48000);

    while (vorbis_analysis_blockout(&vd, &vb) == 1) {
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        ogg_packet op;
        while (vorbis_bitrate_flushpacket(&vd, &op)) {
            op.granulepos = vd.granulepos;
            ogg_stream_packetin(&os, &op);

            while (ogg_stream_pageout(&os, &og)) {
                fwrite(og.header, 1, og.header_len, fp);
                fwrite(og.body, 1, og.body_len, fp);
            }
        }
    }

    vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1) {
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        ogg_packet op;
        while (vorbis_bitrate_flushpacket(&vd, &op)) {
            op.granulepos = vd.granulepos;
            ogg_stream_packetin(&os, &op);

            while (ogg_stream_pageout(&os, &og)) {
                fwrite(og.header, 1, og.header_len, fp);
                fwrite(og.body, 1, og.body_len, fp);
            }
        }
    }

    while (ogg_stream_flush(&os, &og)) {
        fwrite(og.header, 1, og.header_len, fp);
        fwrite(og.body, 1, og.body_len, fp);
    }

    fclose(fp);
    return 0;
}
