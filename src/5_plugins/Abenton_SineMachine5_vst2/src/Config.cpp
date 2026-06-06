#include "Config.h"

Cfg::Cfg()
    : m_masterVolume(80)
    , m_partialVolumeSum(1.0f)
    , m_partialVolumeSumInv(1.0f)
{
    setNumPartials( NUM_PARTIALS );
}

Cfg::~Cfg()
{

}

size_t
Cfg::getNumPartials() const { return m_partials.size(); }

void
Cfg::setNumPartials(int numPartials)
{
    m_partials.resize(numPartials);

    for (size_t i = 0; i < m_partials.size(); i++)
    {
        setPartial(i, 0.0f, 0);
    }
    setPartial(0, 1.0f, 0);
    setDefaultColors();
    setDefaultPartialsToRect();
}

void Cfg::setPartial(int index, float amplitude, double cent )
{
    if (index < 0 || index >= getNumPartials())
    {
        return;
    }

    m_partials.at(index).iPartial = index + 1;
    m_partials.at(index).fAmplitude = amplitude;
    m_partials.at(index).fDetuneCent = cent;
    m_partials.at(index).fPhase = 0.0f;
}

void Cfg::setDefaultColors()
{
    // 🌈 Rainbow coloring:
    float color_step = 0.7f / float(getNumPartials() - 1);

    for (size_t i = 0; i < getNumPartials(); i++)
    {
        m_partials.at(i).fPhase = 0.0f;

        auto color = de::RainbowColor::computeColor128(0.8f - color_step * i);
        m_partials.at(i).color.r = color.r;
        m_partials.at(i).color.g = color.g;
        m_partials.at(i).color.b = color.b;
        m_partials.at(i).color.a = 1.0f;
    }
}

void Cfg::setDefaultPartialsToRect()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_rect<float>( partial.iPartial );
    }
}

void Cfg::setDefaultPartialsToSaw()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_saw<float>( partial.iPartial );
    }
}

void Cfg::setDefaultPartialsToSawRev()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_saw_rev<float>( partial.iPartial );
    }
}

void Cfg::setDefaultPartialsToTriangle()
{
    for (auto & partial : m_partials)
    {
        partial.fAmplitude = de::calc_amplitude_triangle<float>( partial.iPartial );
    }
}
