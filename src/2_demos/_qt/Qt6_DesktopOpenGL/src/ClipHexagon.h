#pragma once
#include <cstdint>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ClipperLib/clipper.hpp>

#include <iostream>

// Define the span structure to represent contiguous ranges
struct Span
{
    uint32_t y;
    uint32_t x1, x2;
};

// Function to generate hexagon vertices
std::vector<ClipperLib::IntPoint> generateHexagon(int cx, int cy, int r) {
    std::vector<ClipperLib::IntPoint> hexagon;
    const double PI = 3.14159265358979323846;
    for (int i = 0; i < 6; ++i) {
        double angle = 2 * PI * i / 6;
        hexagon.push_back(ClipperLib::IntPoint(cx + r * cos(angle), cy + r * sin(angle)));
    }
    return hexagon;
}

// Function to clip heightmap to a polygon and return spans
std::vector<Span> clipHeightmapToPolygon(
    const std::vector<double>& heightmap, int w, int h, 
    const std::vector<ClipperLib::IntPoint>& polygon) {
    
    ClipperLib::Paths subject(1);
    subject[0].push_back(ClipperLib::IntPoint(0, 0));
    subject[0].push_back(ClipperLib::IntPoint(w, 0));
    subject[0].push_back(ClipperLib::IntPoint(w, h));
    subject[0].push_back(ClipperLib::IntPoint(0, h));

    ClipperLib::Paths clip(1);
    clip[0] = polygon;

    ClipperLib::Paths solution;
    ClipperLib::Clipper c;
    c.AddPaths(subject, ClipperLib::ptSubject, true);
    c.AddPaths(clip, ClipperLib::ptClip, true);
    c.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

    std::vector<Span> spans;

    for (const auto& path : solution) {
        for (int y = 0; y < h; ++y) {
            uint32_t x_start = 0, x_end = 0;
            bool in_span = false;

            for (int x = 0; x < w; ++x) {
                ClipperLib::IntPoint p(x, y);
                if (ClipperLib::PointInPolygon(p, path) == 1) {
                    if (!in_span) {
                        x_start = x;
                        in_span = true;
                    }
                    x_end = x;
                } else if (in_span) {
                    spans.push_back({static_cast<uint32_t>(y), x_start, x_end});
                    in_span = false;
                }
            }

            if (in_span) {
                spans.push_back({static_cast<uint32_t>(y), x_start, x_end});
            }
        }
    }

    return spans;
}
