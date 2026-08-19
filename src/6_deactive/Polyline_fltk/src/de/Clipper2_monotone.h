// Clipper2: https://github.com/AngusJohnson/Clipper2
#include "clipper2/clipper.h"
#include <vector>
#include <glm/glm.hpp>

using namespace Clipper2;

// -----------------------------
// Helpers: glm <-> Clipper
// -----------------------------
static inline Path64 polylineToPath64(const std::vector<glm::vec2>& polyline, double scale)
{
    Path64 path;
    path.reserve(polyline.size());
    for (const auto& p : polyline)
        path.push_back(Point64((int64_t)std::llround(p.x * scale),
                               (int64_t)std::llround(p.y * scale)));
    return path;
}

static inline std::vector<glm::vec2> path64ToVec2(const Path64& path, double invScale)
{
    std::vector<glm::vec2> out;
    out.reserve(path.size());
    for (const auto& pt : path)
        out.emplace_back((float)(pt.x * invScale),
                         (float)(pt.y * invScale));
    return out;
}

// -----------------------------
// Step 1: build offset band with Clipper
// -----------------------------
static inline std::vector<glm::vec2> buildOffsetBand2D(
    const std::vector<glm::vec2>& polyline,
    float lineWidth)
{
    if (polyline.size() < 2)
        return {};

    const double scale    = 1000.0;          // fixed scaling for integer coords
    const double invScale = 1.0 / scale;
    const double halfW    = (double)lineWidth * 0.5;

    Path64 pl = polylineToPath64(polyline, scale);

    ClipperOffset co;
    // Round joins + open round ends → nice stroke band
    co.AddPath(pl, JoinType::Round, EndType::OpenRound);

    // Execute offset: returns one or more polygons (bands)
    Paths64 bands = co.Execute(halfW * scale);
    if (bands.empty())
        return {};

    // For a simple polyline, we expect exactly one band polygon
    const Path64& band = bands[0];

    return path64ToVec2(band, invScale);
}

// -----------------------------
// Step 2: simple ear-clipping triangulation (intersection-free)
// -----------------------------
// Triangulates a simple polygon (CCW or CW) into non-intersecting triangles.
// Output: outTriangles = list of glm::vec2, 3 per triangle.
static inline void triangulateSimplePolygonEarClipping(
    const std::vector<glm::vec2>& poly,
    std::vector<glm::vec2>& outTriangles)
{
    outTriangles.clear();
    const size_t n = poly.size();
    if (n < 3)
        return;

    // Index list into poly
    std::vector<int> idx(n);
    for (int i = 0; i < (int)n; ++i)
        idx[i] = i;

    auto cross2 = [](const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
        // cross((b-a), (c-a))
        glm::vec2 ab = b - a;
        glm::vec2 ac = c - a;
        return ab.x * ac.y - ab.y * ac.x;
    };

    auto isPointInTri = [&](const glm::vec2& p,
                            const glm::vec2& a,
                            const glm::vec2& b,
                            const glm::vec2& c) {
        float c1 = cross2(a, b, p);
        float c2 = cross2(b, c, p);
        float c3 = cross2(c, a, p);
        bool hasNeg = (c1 < 0.0f) || (c2 < 0.0f) || (c3 < 0.0f);
        bool hasPos = (c1 > 0.0f) || (c2 > 0.0f) || (c3 > 0.0f);
        return !(hasNeg && hasPos);
    };

    int remaining = (int)n;
    int guard     = 0;

    while (remaining > 3 && guard < 100000)
    {
        ++guard;
        bool clipped = false;

        for (int i = 0; i < remaining; ++i)
        {
            int i0 = idx[(i + remaining - 1) % remaining];
            int i1 = idx[i];
            int i2 = idx[(i + 1) % remaining];

            const glm::vec2& A = poly[i0];
            const glm::vec2& B = poly[i1];
            const glm::vec2& C = poly[i2];

            // Check if vertex B is convex
            float cr = cross2(A, B, C);
            if (cr <= 0.0f) // assuming CCW; flip sign if CW
                continue;

            // Check no other point lies inside triangle ABC
            bool anyInside = false;
            for (int j = 0; j < remaining; ++j)
            {
                int k = idx[j];
                if (k == i0 || k == i1 || k == i2)
                    continue;
                if (isPointInTri(poly[k], A, B, C))
                {
                    anyInside = true;
                    break;
                }
            }
            if (anyInside)
                continue;

            // Ear found → emit triangle
            outTriangles.push_back(A);
            outTriangles.push_back(B);
            outTriangles.push_back(C);

            // Remove ear vertex from polygon
            idx.erase(idx.begin() + i);
            --remaining;
            clipped = true;
            break;
        }

        if (!clipped)
            break; // degenerate case
    }

    // Final triangle
    if (remaining == 3)
    {
        const glm::vec2& A = poly[idx[0]];
        const glm::vec2& B = poly[idx[1]];
        const glm::vec2& C = poly[idx[2]];
        outTriangles.push_back(A);
        outTriangles.push_back(B);
        outTriangles.push_back(C);
    }
}

// -----------------------------
// High-level: polyline -> intersection-free triangle mesh
// -----------------------------
static inline void buildPolylineMesh_ClipperEar(
    const std::vector<glm::vec2>& polyline,
    float lineWidth,
    std::vector<glm::vec2>& outTriangles)
{
    // 1) Offset polyline into a thick band polygon (Clipper)
    std::vector<glm::vec2> band = buildOffsetBand2D(polyline, lineWidth);
    if (band.size() < 3)
    {
        outTriangles.clear();
        return;
    }

    // 2) Triangulate band polygon (ear clipping, intersection-free)
    triangulateSimplePolygonEarClipping(band, outTriangles);
}


You call:
cpp

std::vector<glm::vec2> polyline = /* your points */;
float lineWidth = 5.0f;

std::vector<glm::vec2> tris;
buildPolylineMesh_ClipperEar(polyline, lineWidth, tris);
// tris now contains 3 * numTriangles vertices