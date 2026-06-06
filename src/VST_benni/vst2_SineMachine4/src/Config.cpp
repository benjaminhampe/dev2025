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


ADSR::ADSR()
    : m_frameIndex(0)
    , m_attackSamples(480)
    , m_decaySamples(480)
    , m_sustainLevel(0.7f)
    , m_releaseSamples(4800)
    , m_phase(Idle)
    , m_currentSample(0)
    , m_noteOnVelocity(kMaxAmplitude)
    , m_noteOffVelocity(kMaxAmplitude)
    , m_amplitude(0.0f)
    , m_noteOffTriggered(false)
{
}

void
ADSR::set(  int32_t attackSamples,
            int32_t decaySamples,
            float sustainLevel,
            int32_t releaseSamples)
{
    m_attackSamples = attackSamples;
    m_decaySamples = decaySamples;
    m_sustainLevel = std::clamp(sustainLevel, 0.0f, 1.0f);
    m_releaseSamples = releaseSamples;
    reset();
}

void
ADSR::reset()
{
    m_frameIndex = 0;
    m_phase = Idle;
    m_currentSample = 0;
    m_noteOnVelocity = kMaxAmplitude;
    m_noteOffVelocity = kMaxAmplitude;
    m_amplitude = 0.0f;
    m_noteOffTriggered = false;
}

void
ADSR::noteOn(float velocity)
{
    m_frameIndex = 0;
    m_phase = Attack;
    m_currentSample = 0;
    m_noteOnVelocity = std::clamp(velocity, 0.0f, kMaxAmplitude);
    m_noteOffTriggered = false;
}

void
ADSR::noteOff(float velocity)
{
    m_noteOffVelocity = std::clamp(velocity, 0.0f, kMaxAmplitude);
    m_noteOffTriggered = true;
    if (m_phase != Release)
    {
        m_phase = Release;
        m_currentSample = 0;
    }
}

void
ADSR::setSingleShot(bool bSingleShot)
{
    m_singleShot = bSingleShot;
}

float
ADSR::nextSample()
{
    switch (m_phase)
    {
        case Idle:
        {
            m_amplitude = 0.0f;
            break;
        }
        case Attack:
        {
            // Attack:      ____ 1
            //             /|        y = m * x + n
            //            / |        m = dy / dx
            //           /  |dy      n = 0
            //     0 ___/___|       dy = m_noteOnVelocity
            //            dx        dx = m_attackSamples
            //
            if (m_currentSample < m_attackSamples)
            {
                const float dx = m_attackSamples;
                const float dy = m_noteOnVelocity;
                const float m = dy / dx;
                const float n = 0.0f;
                // Linear function: y = m * x + n
                m_amplitude = m * m_currentSample + n;
                ++m_currentSample;
                ++m_frameIndex;
            }
            else
            {
                m_amplitude = m_noteOnVelocity;
                m_phase = Decay;
                m_currentSample = 0;
            }
            break;
        }

        case Decay:
        {
            // Decay:
            //    1.0 __  dx                    y = m * x + n
            //          \                       m = dy / dx
            //           \   dy                 n = m_noteOnVelocity;
            //            \                     dy = -(m_noteOnVelocity - m_sustainLevel)
            //             \___ m_sustainLevel  dx = m_decaySamples
            //
            if (m_currentSample < m_decaySamples)
            {
                float dx = m_decaySamples;
                float dy = m_sustainLevel - m_noteOnVelocity;
                float m = dy / dx;
                float n = m_noteOnVelocity;
                // Linear function: y = m * x + n
                m_amplitude = m * m_currentSample + n;
                ++m_currentSample;
                ++m_frameIndex;
            }
            else
            {
                m_amplitude = m_sustainLevel;
                m_phase = Sustain;
                m_currentSample = 0;
            }
            break;
        }

        case Sustain:
        {
            // Constant function: y = m * x + n
            // m = 0
            // n = sustainLevel
            m_amplitude = m_sustainLevel;
            if (m_noteOffTriggered || m_singleShot)
            {
                m_phase = Release;
                m_currentSample = 0;
            }
            break;
        }

        case Release:
        {
            // Release:
            //    m_sustainLevel ___            y = m * x + n
            //                     |\           m = dy / dx
            //                     | \          n = m_sustainLevel
            //                  dy |  \        dy = -m_sustainLevel
            //                     |___\___ 0  dx = m_releaseSamples
            //                      dx

            // Linear function: y = m * x + n
            // m = -sustainLevel * noteOffVelocity / releaseSamples
            // n = sustainLevel * noteOffVelocity
            if (m_currentSample < m_releaseSamples)
            {
                float m = -m_sustainLevel / m_releaseSamples; //  * m_noteOffVelocity
                float n = m_sustainLevel; //  * m_noteOffVelocity
                m_amplitude = m * m_currentSample + n;
                ++m_currentSample;
                ++m_frameIndex;
            }
            else
            {
                m_phase = Idle;
                m_amplitude = 0.0f;
            }
            break;
        }
    }

    return std::clamp(m_amplitude, 0.0f, 1.0f); // Limiter
}

bool
ADSR::isActive() const
{
    return m_phase != Idle;
}

