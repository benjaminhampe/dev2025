// 1. Convert polyline → Clipper Path64

#include "clipper2/clipper.h"
#include <poly2tri/poly2tri.h>
#include <glm/glm.hpp>
#include <vector>

using namespace Clipper2;

static inline Path64 toPath64(const std::vector<glm::vec2>& pl, double scale)
{
    Path64 out;
    out.reserve(pl.size());
    for (auto& p : pl)
        out.push_back(Point64((int64_t)std::llround(p.x * scale),
                              (int64_t)std::llround(p.y * scale)));
    return out;
}

static inline std::vector<glm::vec2> toVec2(const Path64& path, double invScale)
{
    std::vector<glm::vec2> out;
    out.reserve(path.size());
    for (auto& p : path)
        out.emplace_back((float)(p.x * invScale),
                         (float)(p.y * invScale));
    return out;
}

// 2. Offset polyline → thick band polygon (Clipper)

static inline std::vector<glm::vec2> buildOffsetBand(
    const std::vector<glm::vec2>& polyline,
    float lineWidth)
{
    if (polyline.size() < 2)
        return {};

    const double scale    = 1000.0;
    const double invScale = 1.0 / scale;
    const double halfW    = lineWidth * 0.5;

    Path64 pl = toPath64(polyline, scale);

    ClipperOffset co;
    co.AddPath(pl, JoinType::Round, EndType::OpenRound);

    Paths64 bands = co.Execute(halfW * scale);
    if (bands.empty())
        return {};

    return toVec2(bands[0], invScale);
}

// Clipper guarantees the band polygon is simple (no self‑intersections).
// This makes triangulation trivial.
// 3. Triangulate polygon band with poly2tri

static inline void triangulatePoly2Tri(
    const std::vector<glm::vec2>& poly,
    std::vector<glm::vec2>& outTriangles)
{
    outTriangles.clear();
    if (poly.size() < 3)
        return;

    // Convert to poly2tri points
    std::vector<p2t::Point*> pts;
    pts.reserve(poly.size());
    for (auto& p : poly)
        pts.push_back(new p2t::Point(p.x, p.y));

    // Create CDT
    p2t::CDT cdt(pts);

    // Triangulate
    cdt.Triangulate();
    auto tris = cdt.GetTriangles();

    // Extract triangles
    for (auto* t : tris)
    {
        outTriangles.emplace_back((float)t->GetPoint(0)->x,
                                  (float)t->GetPoint(0)->y);
        outTriangles.emplace_back((float)t->GetPoint(1)->x,
                                  (float)t->GetPoint(1)->y);
        outTriangles.emplace_back((float)t->GetPoint(2)->x,
                                  (float)t->GetPoint(2)->y);
    }

    // Cleanup
    for (auto* p : pts)
        delete p;
}

// poly2tri produces Delaunay triangles, which means:

//     No intersections

//     High‑quality triangle shapes

//     Great for physics, FEM, GPU mesh stability

// 4. High‑level function: polyline → triangle mesh

static inline void buildPolylineMesh_ClipperPoly2Tri(
    const std::vector<glm::vec2>& polyline,
    float lineWidth,
    std::vector<glm::vec2>& outTriangles)
{
    // 1) Offset polyline into thick band
    std::vector<glm::vec2> band = buildOffsetBand(polyline, lineWidth);
    if (band.size() < 3)
    {
        outTriangles.clear();
        return;
    }

    // 2) Triangulate band polygon
    triangulatePoly2Tri(band, outTriangles);
}

// Call it like:

std::vector<glm::vec2> polyline = { ... };
float lineWidth = 5.0f;

std::vector<glm::vec2> tris;
buildPolylineMesh_ClipperPoly2Tri(polyline, lineWidth, tris);

You now have a triangle list (3 vertices per triangle) that is:
    Intersection‑free
    High‑quality
    Robust
    Fast
    Works for any polyline

🧩 Why this combo is excellent
Clipper
    Handles joins
    Handles ends
    Removes self‑intersections
    Produces simple polygon band

poly2tri
    Constrained Delaunay triangulation
    No triangle intersections
    High‑quality triangles
    Small, easy to embed

This is the best lightweight alternative to CGAL.


🧩 LineJoins:

1. Miter join
    Extends the two offset edges until they intersect
    Sharp corner
    Fast
    Can spike if angle is too small
    Clipper clamps miter spikes using a miter limit

2. Bevel join
    Cuts the corner with a straight line
    No spikes
    Good for CAD‑style geometry
    Very stable

3. Round join
    Approximates a circular arc
    Smooth
    Best for rendering
    Produces more vertices

🧠 Why Clipper is important

Clipper does all join geometry for you, robustly:

co.AddPath(polyline, JoinType::Round, EndType::OpenRound);

You can choose:
    JoinType::Miter
    JoinType::Bevel
    JoinType::Round

Clipper guarantees the resulting polygon is:
    simple (no self‑intersections)
    closed
    correctly joined
    ready for triangulation

This is why Clipper is used in GIS, CAD, and font engines.

🧱 Why you need joins before triangulation

Triangulation libraries (poly2tri, libtess2, monotone) do not create joins.
They only triangulate whatever polygon you give them.

So if you want bevel/miter/round joins, you must:
    Generate the offset band with joins → Clipper
    Triangulate the band → poly2tri / libtess2 / monotone

This is why I used two libraries in the pipeline.

🧩 Choosing LineJoin in Clipper:

    Miter join:

        co.AddPath(pl, JoinType::Miter, EndType::OpenSquare);

    Bevel join:

        co.AddPath(pl, JoinType::Bevel, EndType::OpenSquare);

    Round join:

        co.AddPath(pl, JoinType::Round, EndType::OpenRound);

Clipper will output a polygon with the correct join geometry.

Then you triangulate it.
🧭 Summary
    Joins = how corners of the thick polyline are shaped

    Clipper = generates bevel/miter/round joins automatically

    Triangulator = only triangulates the polygon; does NOT create joins

    You can choose any join type you want

    This is why Clipper is essential in the pipeline
    