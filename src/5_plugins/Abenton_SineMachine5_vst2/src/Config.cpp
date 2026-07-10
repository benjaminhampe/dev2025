#include "Config.h"

size_t
PartialsCfg::numPartials() const { return m_partials.size(); }

void
PartialsCfg::init()
{
    //m_partials.resize(numPartials);

    for (size_t i = 0; i < m_partials.size(); i++)
    {
        setPartial(i, 0.0f, 0);
    }
    setPartial(0, 1.0f, 0);
    setDefaultColors();
    makeRect();
}

void PartialsCfg::setPartial(int index, float amplitude, double cent )
{
    if (index < 0 || index >= numPartials())
    {
        return;
    }

    m_partials.at(index).iPartial = index + 1;
    m_partials.at(index).fAmplitude = amplitude;
    m_partials.at(index).fDetuneCent = cent;
    //m_partials.at(index).fPhase = 0.0f;
}

void PartialsCfg::setDefaultColors()
{
    // 🌈 Rainbow coloring:
    float color_step = 0.7f / float(numPartials() - 1);

    for (size_t i = 0; i < numPartials(); i++)
    {
        auto color = de::RainbowColor::computeColor128(0.8f - color_step * i);
        m_partials.at(i).color.r = color.r;
        m_partials.at(i).color.g = color.g;
        m_partials.at(i).color.b = color.b;
        m_partials.at(i).color.a = 1.0f;
    }
}

void PartialsCfg::makeRect()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_rect<float>( partial.iPartial );
    }
}

void PartialsCfg::makeSaw()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_saw<float>( partial.iPartial );
    }
}

void PartialsCfg::makeSawRev()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_saw_rev<float>( partial.iPartial );
    }
}

void PartialsCfg::makeTriangle()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_triangle<float>( partial.iPartial );
    }
}


