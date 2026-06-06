#pragma once
#include "Common.h"

DE_FORCE_INLINE void
DSP_RESIZE(de::TAlignedVector<float> & out, size_t n)
{
    if (n > 0 && out.size() != n)
    {
        out.resize(n);
    }
}

DE_FORCE_INLINE void
DSP_ZERO(de::TAlignedVector<float> & out)
{
    std::fill(out.begin(),out.end(),0.0f);
}

/*
DE_FORCE_INLINE void
DSP_ZERO(de::TAlignedVector<float> & out, size_t n, size_t start = 0)
{

    for (size_t i = start; i < n-start; i++)
    {
        out.at(i) = 0.0f;
    }
}
*/

DE_FORCE_INLINE void
DSP_ADD(float* __restrict__ pOut, const float* __restrict__ pIn, size_t n)
{
    DE_ASSUME_NO_OVERLAP(pOut,pIn,n*sizeof(float));

    for (size_t i = 0; i < n; i++)
    {
        pOut[i] += pIn[i];
    }
}
