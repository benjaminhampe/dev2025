#pragma once
#include "Config.h"

// class Synth;
// class Plugin;

// 📊
struct Preview
{
    struct Curve
    {
        int partial;

        float amplitude;

        float amplitudeSum;

        NVGcolor color;

        de::TAlignedVector<float> original;     // y[n]

        de::TAlignedVector<float> scaled;       // A_n * y[n]

        de::TAlignedVector<float> accum;        // sum(A_n * y[n],n,0,N)

        de::TAlignedVector<float> normalized;   // A_sum_inv * sum(A_n * y[n],n,0,N)

        de::TAlignedVector<glm::vec2> points;   // Screen coords in pixel
    };

    std::vector<Curve> m_curves;

    Curve m_accum;

    void init( const SynthCfg & cfg, int n = 1024 );

    void update( const SynthCfg & cfg );

    void updatePoints( de::Recti pos, int n = 1024 );

    void saveCurvePoints(const Curve & curve, std::string uri);

    void draw(NVGcontext* vg, de::Recti pos, int n = 1024 );

protected:
    void drawCurve(NVGcontext* vg, const Curve & curve, float strokeWidth);
};
