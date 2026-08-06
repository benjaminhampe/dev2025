#include <de/sound/Sound2Image.h>
#include <de/image/ImagePainter.h>

namespace de {
namespace sound {

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
                        ? 1.5 * double(100 * pos.w) / double(visFrames)
                        : (1.5 / 100.0);
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

    DE_BENNI("w(",pos.w,"), "
             "h(",pos.h,"), "
             "vis(",visFrames,"), "
             "n(",n,"), "
             "alpha(",alpha,"), "
             "df(",df,"), "
             "dx(",dx,"), "
             "da(",da,")")
}


} // end namespace sound.
} // end namespace de.


