#include "Preview.h"
//#include "SinePlugin.h"
#include <vector> // for iAttributes in CreateContext

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif
#include <dwmapi.h>
#include <tchar.h>
#include <mmsystem.h> // For JOYCAPS

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#include <de_opengl.h>
#include <GL/wglext.h>
#include "fonts/fonts_ShareTechMonoRegular_ttf.h"
//#include "../res/resource.h"

// 📊
void Preview::init( const SynthCfg& cfg, int n )
{
    const auto & partials = cfg.m_partials.m_partials;

    DE_DEBUG("n = ",n)
    DE_DEBUG("partials.size() = ",partials.size())

    m_curves.resize( partials.size() );

    m_accum.color = nvgRGBA(255,255,255,255);
    m_accum.normalized.resize( n );
    m_accum.points.resize( n );

    // Fill colors and amplitudeSums:
    float Asum = 0.0f;
    for (size_t c = 0; c < partials.size(); c++)
    {
        float A = partials.at(c).A();
        Asum += A;
        m_curves.at(c).partial = partials.at(c).iPartial;
        m_curves.at(c).color = partials.at(c).color;
        m_curves.at(c).amplitude = A;
        m_curves.at(c).amplitudeSum = Asum;
        m_curves.at(c).original.resize(n);
        m_curves.at(c).scaled.resize(n);
        m_curves.at(c).accum.resize(n);
        m_curves.at(c).normalized.resize(n);
        m_curves.at(c).points.resize(n);
    }

    // Init original samples: Build unscaled sin() lookuptable

    for (Curve & curve : m_curves)
    {
        const float phaseInc = float(de::TWO_PI * curve.partial) / float(n-1); // 1Hz base for preview.
        float phase = 0.0;
        for (int i = 0; i < n; i++)
        {
            curve.original.at(i) = sinf(phase);
            phase += phaseInc;
        }
    }
}

void Preview::update( const SynthCfg & cfg )
{
    if (m_curves.empty())
    {
        DE_ERROR("No curves")
        return;
    }
    const auto & partials = cfg.m_partials.m_partials;
    const size_t nPartials = partials.size();
    const size_t nSamples = m_curves[0].original.size();

    // Fill colors and amplitudeSums:
    float Asum = 0.0f;
    for (size_t c = 0; c < nPartials; c++)
    {
        float A = partials.at(c).A();
        Asum += A;
        m_curves[c].amplitude = A;
        m_curves[c].amplitudeSum = Asum;
    }

    // Scale:
    for (Curve & curve : m_curves)
    {
        const float* __restrict__ O = curve.original.data();
              float* __restrict__ S = curve.scaled.data();
        DE_ASSUME_NO_OVERLAP(S,O,nSamples * sizeof(float));

        for (size_t i = 0; i < nSamples; i++) // For all samples
        {
            S[i] = O[i] * curve.amplitude;
        }
    }

    // Accumulate:
    std::copy(m_curves[0].scaled.begin(),   // src
              m_curves[0].scaled.end(),     // src
              m_curves[0].accum.begin());   // dst

    // Accumulate:
    for (size_t c = 1; c < nPartials; c++)
    {
        const float* __restrict__ S1 = m_curves[c].scaled.data();
        const float* __restrict__ A0 = m_curves[c-1].accum.data();
              float* __restrict__ A1 = m_curves[c].accum.data();
        DE_ASSUME_NO_OVERLAP(A0,A1,nSamples * sizeof(float));
        DE_ASSUME_NO_OVERLAP(A0,S1,nSamples * sizeof(float));
        DE_ASSUME_NO_OVERLAP(A1,S1,nSamples * sizeof(float));

        for (size_t i = 0; i < nSamples; i++) // For all samples
        {
            A1[i] = S1[i] + A0[i];
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
            const float* __restrict__ A = curve.accum.data();
                  float* __restrict__ N = curve.normalized.data();
            DE_ASSUME_NO_OVERLAP(N,A,nSamples * sizeof(float));

            for (size_t i = 0; i < nSamples; i++) // For all samples
            {
                N[i] = A[i] * invA;
            }
        }
        else
        {
            std::memset(curve.normalized.data(), 0,
                        curve.normalized.size() * sizeof(float));
            //std::fill(curve.normalized.begin(), curve.normalized.end(), 0.0f);
        }
    }

    // Copy to white curve m_accum
    std::copy(m_curves.back().normalized.begin(),   // src
              m_curves.back().normalized.end(),     // src
              m_accum.normalized.begin());          // dst
}

void Preview::saveCurvePoints(const Curve & curve, std::string uri)
{
    const uint64_t n = curve.normalized.size();
    const float fScaleX = 1.0f / float(n-1);
    const float fScaleY = 1.0f;

    std::vector<glm::vec2> save(n);

    for (uint64_t i = 0; i < n; ++i)
    {
        const float x = fScaleX * i;
        const float y = curve.normalized[i];
        save[i] = glm::vec2(x, y);
    }

    std::ostringstream o;
    o << n << "\n";
    for (uint64_t i = 0; i < n; ++i)
    {
        o << save[i].x << " " << save[i].y << "\n";
    }
    o << "\n";

    de::File file(uri, de::eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot write ",uri)
        return; // false;
    }

    const auto s = o.str();
    file.write(s.data(),s.size());
    DE_OK("Write file ",uri)
    return; // false;
}

void Preview::updatePoints( de::Recti pos, int n )
{
    const float ox = pos.x;
    const float oy = pos.y + pos.h/2; // Zero center line is middle of rect height.
    const float sx = float(pos.w) / float(n-1);
    const float sy = -float(pos.h) / 2.0f;

    for (int c = 0; c < m_curves.size(); c++)
    {
        Curve & curve = m_curves.at(c);

        for (int i = 0; i < curve.normalized.size(); i++)
        {
            const float y = curve.normalized.at(i);
            const float sample_x = sx * i + ox;
            const float sample_y = sy * y + oy;
            curve.points.at(i) = glm::vec2(sample_x, sample_y);
        }
    }

    Curve & curve = m_accum;

    for (int i = 0; i < curve.normalized.size(); i++)
    {
        const float y = curve.normalized.at(i);
        const float sample_x = sx * i + ox;
        const float sample_y = sy * y + oy;
        curve.points.at(i) = glm::vec2(sample_x, sample_y);
    }

    // saveCurvePoints(curve,"SineMachine_accum.vec2");
}


void Preview::drawCurve(NVGcontext* vg, const Curve & curve, float strokeWidth)
{
    if (curve.points.size() < 2)
    {
        DE_ERROR("Not enough points ", curve.points.size())
        return;
    }

    nvgBeginPath(vg);
    nvgMoveTo(vg, curve.points[0].x, curve.points[0].y);
    for (size_t i = 1; i < curve.points.size(); i++)
    {
        nvgLineTo(vg, curve.points[i].x, curve.points[i].y);
    }

    nvgStrokeColor(vg, curve.color);
    nvgStrokeWidth(vg, strokeWidth); // Thick line to emphasize joins
    nvgLineJoin(vg, NVG_ROUND); // Options: NVG_MITER, NVG_ROUND, NVG_BEVEL
    nvgStroke(vg);
}

void Preview::draw(NVGcontext* vg, de::Recti pos, int n )
{
    updatePoints( pos, n );

    for (int c = 0; c < m_curves.size(); c++)
    {
        drawCurve(vg, m_curves[c], 5.0f);
    }

    drawCurve(vg, m_accum, 3.0f);
}
