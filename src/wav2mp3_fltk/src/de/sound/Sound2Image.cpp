#include <de/sound/Sound2Image.h>
#include <de/image/ImagePainter.h>

namespace de {
namespace sound {

#if 0

void
Sound2Image::draw( const Sound& sound,
                   int32_t channel,
                   int64_t frameBeg,
                   int64_t frameEnd,
                   Image& img,
                   const Recti& pos,
                   uint32_t penColor,
                   uint32_t fillColor )
{
    if (img.w() < 2 || img.h() < 2) return;
    if (pos.w < 2 || pos.h < 2) return;
    if (sound.empty()) return;
    if (channel >= sound.channels()) return;

    frameBeg = std::max(frameBeg, 0ll);
    frameEnd = std::min(frameEnd, sound.frames());

    const int64_t n = frameEnd - frameBeg;
    if (n < 2) return;

    auto converter = SampleTypeConverter::getConverter(sound.sampleType(), SampleType::F32);
    if (!converter) return;

    ImagePainter::drawRect(img, pos, fillColor, false);

    const int64_t visFrames = std::min<int64_t>(100 * pos.w, n);
    const double df = double(n) / double(visFrames);

    TAlignedVector<float> frameBuf(sound.m_channels);

    // Decide mode: line vs min/max envelope
    const bool useLineMode = (visFrames > 3 * pos.w);

    if (useLineMode)
    {
        // --- LINE MODE (zoomed in) ---
        const double dx = double(pos.w) / double(visFrames);

        sound.read_frames(converter, frameBuf.data(), 1, frameBeg);
        float s1 = frameBuf[channel];
        int32_t x1 = pos.x;
        int32_t y1 = pos.y + std::lroundf((1.0f - s1) * float(pos.h / 2));

        // Simple fixed alpha
        const uint8_t a = 255;
        const uint32_t pen = dbRGB(dbRGB_R(penColor),
                                    dbRGB_G(penColor),
                                    dbRGB_B(penColor),
                                    a);

        for (int64_t i = 1; i < visFrames; ++i)
        {
            int64_t frameCur = frameBeg + std::llround(df * i);
            sound.read_frames(converter, frameBuf.data(), 1, frameCur);
            float s2 = frameBuf[channel];

            int32_t x2 = pos.x + std::lroundf(dx * i);
            int32_t y2 = pos.y + std::lroundf((1.0f - s2) * float(pos.h / 2));

            ImagePainter::drawLine(img, x1, y1, x2, y2, pen, true);
            x1 = x2;
            y1 = y2;
        }
    }
    else
    {
        // --- MIN/MAX ENVELOPE MODE (zoomed out) ---
        for (int32_t px = 0; px < pos.w; ++px)
        {
            const int64_t f0 = frameBeg + int64_t(double(px + 0) * df);
            const int64_t f1 = frameBeg + int64_t(double(px + 1) * df);

            float sMin = +1.0f;
            float sMax = -1.0f;

            for (int64_t f = f0; f < f1; ++f)
            {
                sound.read_frames(converter, frameBuf.data(), 1, f);
                float s = frameBuf[channel];
                sMin = std::min(sMin, s);
                sMax = std::max(sMax, s);
            }

            const int32_t x = pos.x + px;
            const int32_t yMin = pos.y + std::lroundf((1.0f - sMax) * float(pos.h / 2));
            const int32_t yMax = pos.y + std::lroundf((1.0f - sMin) * float(pos.h / 2));

            ImagePainter::drawLine(img, x, yMin, x, yMax, penColor, true);
        }
    }
}

#else

// static
void
Sound2Image::draw( const Sound& sound,
                int32_t channel,
                int64_t frameBeg,
                int64_t frameEnd,
                Image& img,
                const Recti& pos,
                uint32_t penColor,
                uint32_t fillColor)
{
    if (img.w() < 2 || img.h() < 2)
    {
        DE_ERROR("Empty image.")
        return;
    }

    if (pos.w < 2 || pos.h < 2)
    {
        DE_ERROR("Empty rect.")
        return;
    }

    if (sound.empty())
    {
        DE_ERROR("Empty sound.")
        return;
    }

    if (channel >= sound.channels())
    {
        DE_ERROR("No channel ",channel, ". ",sound.str())
        return;
    }

    frameBeg = std::max(frameBeg, 0ll);
    frameEnd = std::min(frameEnd, sound.frames());

    const int64_t n = frameEnd - frameBeg;
    if (n < 2)
    {
        DE_ERROR("Nothing todo")
        return;
    }

    auto converter = SampleTypeConverter::getConverter(sound.sampleType(),SampleType::F32);
    if (!converter)
    {
        DE_ERROR("No converter")
        return;
    }

    ImagePainter::drawRect(img, pos, fillColor, false);

    const auto visFrames = std::min<int64_t>( 100 * pos.w, n );
    //const auto visFrames = std::max<int64_t>( 30000, n/100 );
    const auto ppf = double(pos.w) / double(visFrames);
    const auto fpp = double(visFrames) / double(pos.w);
    const auto df = double(n) / double(visFrames);
    const auto dx = ppf;
    const auto da = (visFrames < 100 * pos.w)
                        ? 7.5 / 100.0 // double(visFrames) / double(100 * pos.w)
                        : (3.5 / 100.0);
    const auto alpha = std::clamp<int32_t>(std::lround(255.0 * da), 1, 255);
    const auto pen_r = dbRGB_R(penColor);
    const auto pen_g = dbRGB_G(penColor);
    const auto pen_b = dbRGB_B(penColor);
    const auto pen = dbRGB(pen_r,pen_g,pen_b,alpha);

    TAlignedVector<float> frameBuf( sound.m_channels );

    sound.read_frames( converter, frameBuf.data(), 1, frameBeg);
    float s1 = frameBuf[channel];
    int32_t x1 = pos.x;
    int32_t y1 = pos.y + std::lroundf( (1.0f - s1) * float(pos.h/2) );

#if 1
    for (int64_t i = 1; i < visFrames; ++i)
    {
        int64_t frameCur = frameBeg + std::llround(df * i);
        sound.read_frames( converter, frameBuf.data(), 1, frameCur);
        float s2 = frameBuf[channel];
        int32_t x2 = pos.x + std::lroundf( dx * i );
        int32_t y2 = pos.y + std::lroundf( (1.0f - s2) * float(pos.h/2) );
        ImagePainter::drawLine( img, x1, y1, x2, y2, pen, true);
        x1 = x2;
        y1 = y2;
    }
#else
    // Vertical lines approach:
    for (int32_t px = 0; px < pos.w; ++px)
    {
        int64_t f0 = frameBeg + int64_t((px + 0) * df);
        int64_t f1 = frameBeg + int64_t((px + 1) * df);

        float sMin = +1.0f;
        float sMax = -1.0f;

        for (int64_t f = f0; f < f1; ++f)
        {
            sound.read_frames(converter, frameBuf.data(), 1, f);
            float s = frameBuf[channel];
            sMin = std::min(sMin, s);
            sMax = std::max(sMax, s);
        }

        int32_t x = pos.x + px;
        int32_t yMin = pos.y + int((1.0f - sMax) * (pos.h / 2));
        int32_t yMax = pos.y + int((1.0f - sMin) * (pos.h / 2));

        ImagePainter::drawLine(img, x, yMin, x, yMax, penColor, true);
    }
#endif


#if 0
    DE_BENNI("w(",pos.w,"), "
             "h(",pos.h,"), "
             "vis(",visFrames,"), "
             "n(",n,"), "
             "alpha(",alpha,"), "
             "df(",df,"), "
             "dx(",dx,"), "
             "da(",da,")")
#endif
}
#endif

} // end namespace sound.
} // end namespace de.


