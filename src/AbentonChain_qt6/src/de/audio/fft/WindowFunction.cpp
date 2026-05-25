#include <de/audio/fft/WindowFunction.h>
#include <sstream>
#include <algorithm>

namespace de {
namespace audio {

std::string drawSvgStippleLine(float x1, float y1, float x2, float y2, float strokeWidth,
                           const std::string& strokeColor, const std::string& dashPattern)
{
    std::ostringstream o; o << "  "
    "<polyline "
    "points=\"" << x1 << "," << y1 << " " << x2 << "," << y2 << "\" "
    "stroke=\"" << strokeColor << "\" "
    "stroke-width=\"" << strokeWidth << "\" "
    "stroke-linecap=\"round\" "
    "stroke-dasharray=\"" << dashPattern << "\" "
    "fill=\"none\" />";
    return o.str();
}

std::string lineStripToSVG(const std::vector<float>& lineStrip,
                           float w,
                           float h,
                           float strokeWidth,
                           const std::string& strokeColor)
{
    std::ostringstream o;

    if (lineStrip.empty())
        return "";

    // Compute min/max for normalization
    // auto [minIt, maxIt] = std::minmax_element(lineStrip.begin(), lineStrip.end());
    // float minY = *minIt;
    // float maxY = *maxIt;
    // float range = (maxY - minY);
    // if (range == 0.0f)
    //     range = 1.0f; // avoid division by zero

    // SVG header
    o << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
       << "width=\"" << w << "\" height=\"" << h << "\" "
       << "viewBox=\"0 0 " << w << " " << h << "\">\n";

    // o << "<rect width=\"100%\" height=\"100%\" fill=\"rgb(200,200,200)\" />\n";
    // auto r = Recti(std::lround(.05f * w),
    //                std::lround(.05f * h),
    //                std::lround(0.9f * w)+1,
    //                std::lround(0.9f * h)+1);
    // ImagePainter::drawRect(img,r,dbRGBA(238,238,238));

    float x1 = .05f * w;
    float x2 = .95f * w;
    float y1 = .05f * h;
    float y2 = .95f * h;

    // Draw Rect:
    // auto r = Recti(x1,y1,x2-x1,y2-y1);
    // o<< "   <rect fill=\"rgb(225,225,225)\" stroke=\"none\" "
    //     "x=\"" << r.x << "\" "
    //     "y=\"" << r.y - 0.25f*strokeWidth << "\" "
    //     "width=\"" << r.w + .5f * strokeWidth << "\" "
    //     "height=\"" << r.h + .5f * strokeWidth << "\" "
    //     "/>\n";

    // Compute points:
    const size_t N = lineStrip.size();
    const float dx = 1.0f / float(N - 1);
    std::vector<float> x(N, 0.0f);
    std::vector<float> y(N, 0.0f);
    for (size_t i = 0; i < N; ++i)
    {
        x[i] = x1 + (x2-x1) * dx * i;
        y[i] = y2 - (y2-y1) * lineStrip[i];
    }

    // Draw filled Polyline:
    o<< "  <polyline "
        "fill=\"rgb(225,225,225)\" "
        "stroke=\"none\" "
        "points=\"";

    o << x1 << "," << y2 << " ";
    for (size_t i = 0; i < N; ++i)
    {
        o << x[i] << "," << y[i] << " ";
    }
    o << x2 << "," << y2 << "\" />\n";

    // Draw outline Polyline:
    o<< "  <polyline "
        "fill=\"none\" "
        "stroke=\"" << strokeColor << "\" "
        "stroke-linejoin=\"round\" "
        "stroke-linecap=\"round\" "
        "stroke-width=\"" << strokeWidth << "\" "
        "points=\"";

    for (size_t i = 0; i < N; ++i)
    {
        o << x[i] << "," << y[i];
        if (i + 1 < N)
            o << " ";
    }

    o << "\" />\n";

    // Draw outline left:
    o << drawSvgStippleLine(x[0],y[0],x1,y2,1.5f,"red","0 2");

    // Draw outline right:
    o << drawSvgStippleLine(x[N-1],y[N-1],x2,y2,1.5f,"blue","0 2");

    o << "</svg>\n";

    return o.str();
}

// static
std::string WindowFunction::createSVG( eFunc func, int w, int h, int n )
{
    float lineWidth = 2.0f;

    WindowFunction wf;
    wf.setFunction(func);
    wf.resize(n);

    std::vector<float> v_in(n,1.0f);
    std::vector<float> v_out(n,1.0f);
    wf.apply(v_in.data(), v_in.size(), v_out.data(), v_out.size());

    return lineStripToSVG(v_out,w,h,lineWidth,"black");
}

// static
void WindowFunction::testSvg()
{
    //auto fillColor = 0xFFFFFFFF;
    //auto bodyColor = 0xFFFAFAFA;
    //auto penColor = 0xFFFAFAFA;

    int n = 256;
    int w = 64;
    int h = 64;
    float lineWidth = 2.0f;

    for (int i = 0; i < eFuncMax; ++i)
    {
        auto func = (eFunc)i;

        WindowFunction wf;
        wf.setFunction(func);
        wf.resize(n);

        std::vector<float> v_in(n,1.0f);
        std::vector<float> v_out(n,1.0f);
        wf.apply(v_in.data(), v_in.size(), v_out.data(), v_out.size());

        std::string uri = dbStr("WindowFunction_",getString(func),".svg");
        std::string svg = lineStripToSVG(v_out,w,h,lineWidth,"black");
        dbSaveTextA(uri,svg);
    }
}

// static
void WindowFunction::drawImage( eFunc func, Image & img, uint32_t penColor )
{
    WindowFunction wf;
    wf.setFunction(func);
    wf.resize(img.w());

    std::vector<float> v_in(img.w(),1.0f);
    std::vector<float> v_out(img.w(),1.0f);

    wf.apply(v_in.data(), v_in.size(), v_out.data(), v_out.size());

    // Transform window function coords to pixel coords
    float dx = 1.0f / float(v_out.size() - 1);
    for (int i = 0; i < v_out.size(); ++i)
    {
        // if ((i == 0) || (i == int(v_out.size()-1)))
        // {
        //     v_out[i] = 0.0f;
        // }
        float x = std::round((0.05f + 0.9f * dx * i) * img.w());
        float y = std::round((0.95f - 0.9f * v_out[i]) * img.h());
        v_in[i] = x;
        v_out[i] = y;
    }

    // Draw BorderRect:
    int w = img.w();
    int h = img.h();
    auto r = Recti(std::lround(.05f * w),
                   std::lround(.05f * h),
                   std::lround(0.9f * w)+1,
                   std::lround(0.9f * h)+1);
    ImagePainter::drawRect(img,r,dbRGBA(238,238,238));

    // Draw LineStrip:
    long x1 = std::lround(v_in[0]);
    long y1 = std::lround(v_out[0]);
    for (int i = 1; i < v_out.size(); ++i)
    {
        long x2 = std::lround(v_in[i]);
        long y2 = std::lround(v_out[i]);
        ImagePainter::drawLine(img,x1,y1,x2,y2,penColor);
        x1 = x2;
        y1 = y2;
    }
}

// static
void WindowFunction::testImg()
{
    for (int i = 0; i < eFuncMax; ++i)
    {
        de::Image img(1024,768);
        img.fill(0xFFFFFFFF);

        auto f = (eFunc)i;
        auto c = dbRGBA(0,0,0);
        if (i==0) c = dbRGBA(255,0,0);
        else if (i==1) c = dbRGBA(255,128,0);
        else if (i==2) c = dbRGBA(0,200,0);
        else if (i==3) c = dbRGBA(0,0,200);
        else if (i==4) c = dbRGBA(200,0,200);
        else if (i==5) c = dbRGBA(200,200,100);
        else if (i==6) c = dbRGBA(55,100,55);
        else if (i==7) c = dbRGBA(100,0,200);
        drawImage(f, img, c);

        std::string uri = "WindowFunction_";
        uri += getString(f);
        uri += ".webp";
        dbSaveImage(img, uri);
    }
}

// static
void WindowFunction::test()
{
    testImg();
    testSvg();
}

WindowFunction::WindowFunction()
    : m_func(Rect)
{
}

void WindowFunction::updateCoefficients()
{
    if (m_lut.size() < 2)
    {
        return; // Nothing todo.
    }

    auto my_clampf = [](float x, float lo, float hi) -> float
    {
        return fminf(fmaxf(x, lo), hi); // Branchless
    };

    if (m_func == Rect)
    {
        for (size_t i = 0; i < m_lut.size(); ++i)
        {
            m_lut[i] = 1.0f;
        }
    }
    else if (m_func == Hann)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.5f
                    - 0.5f * cosf(2.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Hamming)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.54f
                    - 0.46f * cosf(2.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Blackman)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.42f
                    - 0.5f * cosf(2.0f * M_PI * i * f)
                    + 0.08f * cosf(4.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == BlackmanHarris)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 0.35875f
                    - 0.48829f * std::cos(2.0f * M_PI * i * f)
                    + 0.14128f * std::cos(4.0f * M_PI * i * f)
                    - 0.01168f * std::cos(6.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == FlatTop)
    {
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 1.0f
                    - 1.930f * std::cos(2.0f * M_PI * i * f)
                    + 1.290f * std::cos(4.0f * M_PI * i * f)
                    - 0.388f * std::cos(6.0f * M_PI * i * f)
                    + 0.028f * std::cos(8.0f * M_PI * i * f);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Bartlet)
    {
        const float half = (m_lut.size() - 1) * 0.5f;
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w = 1.0f - std::fabs((i - half) / half);
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Kaiser)
    {
        const float beta = 7.5f; // <- a guess.

        // Approximation der modifizierten Bessel-Funktion I0
        auto I0 = [](float x) -> float
        {
            float ax = std::fabs(x);
            float y = ax * 0.5f;
            float sum = 1.0f;
            float t = 1.0f;

            for (int k = 1; k < 20; ++k)
            {
                t *= (y * y) / (k * k);
                sum += t;
            }
            return sum;
        };

        const float denom = I0(beta);
        const float f = 1.0f / (m_lut.size() - 1);
        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float r = ((2.0f * i) * f) - 1.0f;
            float w = I0(beta * std::sqrt(1.0f - r * r)) / denom;
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else if (m_func == Tukey)
    {
        float alpha = 0.5f;
        const float e = m_lut.size() - 1;
        //const float f = 1.0f / e;
        const float edge = alpha * e * 0.5f;

        for (uint32_t i = 0; i < m_lut.size(); ++i)
        {
            float w;

            if (alpha > 0.0f && i < edge)
            {
                // Rising cosine
                w = 0.5f * (1.0f + std::cos(M_PI * (2.0f * i / (alpha * e) - 1.0f)));
            }
            else if (alpha > 0.0f && i > e * (1.0f - alpha / 2.0f))
            {
                // Falling cosine
                w = 0.5f * (1.0f + std::cos(M_PI * (2.0f * i / (alpha * e) - 2.0f / alpha + 1.0f)));
            }
            else
            {
                // Flat region
                w = 1.0f;
            }
            m_lut[i] = my_clampf(w, 0.0f, 1.0f);
        }
    }
    else
    {
        DE_ERROR("Unsupported WindowFunction")
        for (size_t i = 0; i < m_lut.size(); ++i)
        {
            m_lut[i] = 0.0f;
        }
    }
}

void WindowFunction::setFunction(eFunc func)
{
    if (m_func != func)
    {
        m_func = func;
        updateCoefficients();
    }
}

void WindowFunction::resize(uint32_t n)
{
    if (n < 2)
    {
        return; // Bad n, nothing todo
    }

    if (n != m_lut.size())
    {
        m_lut.resize(n);
        updateCoefficients();
    }
}

void WindowFunction::apply(const AlignedFloatVector& v_in, AlignedFloatVector& v_out)
{
    apply(v_in.data(), v_in.size(), v_out.data(), v_out.size());
}

void WindowFunction::apply(const float* __restrict__ pIn, uint32_t nIn,
                                 float* __restrict__ pOut, uint32_t nOut)
{
    DE_ASSUME(pIn != pOut);

    if (nIn != nOut)
    {
        DE_WARN("Input.size(",nIn,") != Output.size(",nOut,")")
    }

    const size_t N = std::min(nIn, nOut);

    // if (m_func == Rect)
    // {
    //     std::memcpy(pOut, pIn, N * sizeof(float));
    //     return;
    // }

    resize( N );

    // Transform with WindowFunction
    const float* __restrict__ lut = m_lut.data();
    DE_ASSUME(pIn != lut);
    DE_ASSUME(pOut != lut);
    for (size_t i = 0; i < N; ++i)
    {
        pOut[i] = pIn[i] * lut[i];
    }

    // Fill output residue with zeroes
    for (size_t i = nOut; i < N; ++i) { pOut[i] = 0.0f; }
}

} // end namespace audio.
} // end namespace de.


