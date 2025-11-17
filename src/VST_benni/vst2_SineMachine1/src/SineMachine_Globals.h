#pragma once

#include "pluginterfaces/vst2.x/audioeffectx.h"
#include <cmath>
#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include "nanovg.h"
#include "nanovg_gl.h"
#include <GLFW/glfw3.h>
#include <thread>

#include "de_fast_sine.h"

#include <DarkImage.h>

// 🎼 Globals & Contants:
// #define NUM_OVERTONES 60

constexpr uint32_t NUM_PARTIALS = 32;

extern int32_t m_screenWidth;

extern int32_t m_screenHeight;

extern std::atomic<bool> shouldClose;

struct LFO
{

};

// 📊
struct Partial
{
    float phase = 0.0f;
    float phaseIncrement = 1.0f;

    int partial; // 1,2,N x Even and Odd multiples of BaseFrequency = Harmonics/Partials
    float amplitude; // in [0,1]
    float detune; // max. detune in cent
    NVGcolor color;
    bool audible = true;
    bool editable = true;

    LFO lfoAmplitude;
    LFO lfoDetune;

    std::string str() const
    {
        return std::to_string(partial);
    }

    float A() const
    {
        return amplitude;
    }
};

struct Preview
{
    struct Curve
    {
        int partial;

        float amplitude;

        float amplitudeSum;

        NVGcolor color;

        de::TAlignedVector<float> original;

        de::TAlignedVector<float> scaled;

        de::TAlignedVector<float> accum;

        de::TAlignedVector<float> normalized;

        de::TAlignedVector<glm::vec2> points;
    };

    std::vector<Curve> m_curves;

    Curve m_accum;

    // std::vector<de::AlignedFloatVector> m_curves;

    // de::AlignedFloatVector m_amplitudeSums;

    // de::TAlignedVector<NVGcolor> m_colors;


    void init( const std::vector<Partial> & partials, int n = 1024 )
    {
        m_curves.resize( partials.size() );

        m_accum.color = nvgRGBA(255,255,255,255);
        m_accum.normalized.resize( n );
        m_accum.points.resize( n );

        // Fill colors and amplitudeSums:
        float Asum = 0.0f;
        for (size_t c = 0; c < partials.size(); ++c)
        {
            float A = partials[c].A();
            Asum += A;
            m_curves[c].partial = partials[c].partial;
            m_curves[c].color = partials[c].color;
            m_curves[c].amplitude = A;
            m_curves[c].amplitudeSum = Asum;
            m_curves[c].original.resize(n);
            m_curves[c].scaled.resize(n);
            m_curves[c].accum.resize(n);
            m_curves[c].normalized.resize(n);
            m_curves[c].points.resize(n);
        }

        // Init original samples: Build unscaled sin() lookuptable

        for (Curve & curve : m_curves)
        {
            const float phaseInc = float(de::TWO_PI * curve.partial) / float(n-1); // 1Hz base for preview.
            float phase = 0.0;
            for (int i = 0; i < n; i++)
            {
                curve.original[i] = sinf(phase);
                phase += phaseInc;
            }
        }
    }

    void update( const std::vector<Partial> & partials )
    {
        const size_t nPartials = partials.size();
        const size_t nSamples = m_curves[0].original.size();

        // Fill colors and amplitudeSums:
        float Asum = 0.0f;
        for (size_t c = 0; c < nPartials; c++)
        {
            float A = partials[c].A();
            Asum += A;
            m_curves[c].amplitude = A;
            m_curves[c].amplitudeSum = Asum;
        }

        // Scale:
        for (Curve & curve : m_curves)
        {
            const float A = curve.amplitude;

            for (size_t i = 0; i < nSamples; i++) // For all samples
            {
                curve.scaled[i] = curve.original[i] * A;
            }
        }

        // Accumulate:
        std::copy(m_curves[0].scaled.begin(),   // src
                  m_curves[0].scaled.end(),     // src
                  m_curves[0].accum.begin());   // dst

        // Accumulate:
        for (size_t c = 1; c < nPartials; c++)
        {
            for (size_t i = 0; i < nSamples; i++) // For all samples
            {
                m_curves[c].accum[i] = m_curves[c].scaled[i] + m_curves[c-1].accum[i];
            }
        }

        // Normalize:
        std::copy(m_curves[0].original.begin(),   // src
                  m_curves[0].original.end(),     // src
                  m_curves[0].normalized.begin());// dst

        // Normalize:
        for (size_t c = 1; c < nPartials; c++)
        {
            Curve & curve = m_curves[c];

            const float Asum = curve.amplitudeSum;
            if (Asum > 1.e-10f)
            {
                const float invA = 1.0f / Asum;
                for (size_t i = 0; i < nSamples; i++) // For all samples
                {
                    curve.normalized[i] = curve.accum[i] * invA;
                }
            }
            else
            {
                std::memset(curve.normalized.data(), 0, curve.normalized.size() * sizeof(float));
                //std::fill(curve.normalized.begin(), curve.normalized.end(), 0.0f);
            }
        }

        // Copy to white curve m_accum
        std::copy(m_curves.back().normalized.begin(),   // src
                  m_curves.back().normalized.end(),     // src
                  m_accum.normalized.begin());          // dst
    }

    void updatePoints( de::Recti pos, int n = 1024 )
    {
        const float ox = pos.x;
        const float oy = pos.y + pos.h/2; // Zero center line is middle of rect height.
        const float sx = float(pos.w) / float(n-1);
        const float sy = -float(pos.h) / 2.0f;

        for (int c = 0; c < m_curves.size(); c++)
        {
            Curve & curve = m_curves[c];

            for (int i = 0; i < curve.normalized.size(); i++)
            {
                const float y = curve.normalized[i];
                const float sample_x = sx * i + ox;
                const float sample_y = sy * y + oy;
                curve.points[ i ] = glm::vec2(sample_x, sample_y);
            }
        }

        Curve & curve = m_accum;

        for (int i = 0; i < curve.normalized.size(); i++)
        {
            const float y = curve.normalized[i];
            const float sample_x = sx * i + ox;
            const float sample_y = sy * y + oy;
            curve.points[ i ] = glm::vec2(sample_x, sample_y);
        }
    }


    void drawCurve(NVGcontext* vg, const Curve & curve, float strokeWidth)
    {
        // Begin drawing
        nvgBeginPath(vg);
        nvgMoveTo(vg, curve.points[0].x, curve.points[0].y);
        for (size_t i = 1; i < curve.points.size(); i++)
        {
            nvgLineTo(vg, curve.points[i].x, curve.points[i].y);
        }

        // Set line style
        nvgStrokeColor(vg, curve.color);
        nvgStrokeWidth(vg, strokeWidth); // Thick line to emphasize joins
        nvgLineJoin(vg, NVG_ROUND); // Options: NVG_MITER, NVG_ROUND, NVG_BEVEL
        nvgStroke(vg);
    }

    void draw(NVGcontext* vg, de::Recti pos, int n = 1024 )
    {
        updatePoints( pos, n );

        for (int c = 0; c < m_curves.size(); c++)
        {
            drawCurve(vg, m_curves[c], 5.0f);
        }

        drawCurve(vg, m_accum, 3.0f);
    }
};

struct SineOvertone
{
    float amplitude;
    float frequency; // = octave + semitone + centDetune;
    float phase;
    float phaseIncrement;

    int octave;
    int semitone;
    float detune; // in cent

    std::string name; // e.g. A4

    std::string str() const
    {
        static const char* noteNames[12] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };

        std::string s = noteNames[semitone % 12];
        s += std::to_string(octave);
        return s;
    }
};
