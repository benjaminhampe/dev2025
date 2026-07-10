#include <lunasvg.h>
#include "earcut.hpp"

#include <vector>
#include <array>
#include <cstdint>

// Simple vertex struct for OpenGL
struct Vertex {
    float x, y;
};

// Flatten a lunasvg path into a polyline
static std::vector<Vertex> flattenPath(const lunasvg::Path& path, float tol = 0.25f)
{
    std::vector<Vertex> out;

    lunasvg::Path p = path;
    p.flatten(tol);

    for (const auto& cmd : p.commands()) {
        if (cmd.type == lunasvg::PathCommandType::MoveTo ||
            cmd.type == lunasvg::PathCommandType::LineTo)
        {
            out.push_back({ float(cmd.x), float(cmd.y) });
        }
        // CubicTo is already flattened by p.flatten()
    }

    return out;
}

int main()
{
    // Load SVG
    auto doc = lunasvg::Document::loadFromFile("input.svg");
    if(!doc) {
        printf("Failed to load SVG\n");
        return 1;
    }

    // Get shapes
    auto svg = doc->svgElement();
    auto shapes = svg->shapes();

    // Earcut polygon format:
    // vector< vector< array<double,2> > >
    std::vector<std::vector<std::array<double,2>>> polygon;

    // Outer ring
    polygon.emplace_back();

    for(const auto& shape : shapes)
    {
        const auto& path = shape->path();

        // Flatten SVG curves into polyline
        auto polyline = flattenPath(path);

        // Convert to earcut format
        std::vector<std::array<double,2>> ring;
        ring.reserve(polyline.size());

        for(const auto& v : polyline)
            ring.push_back({ double(v.x), double(v.y) });

        polygon.push_back(ring);
    }

    // Triangulate
    auto indices = mapbox::earcut<uint32_t>(polygon);

    // Convert polygon vertices to OpenGL vertices
    std::vector<Vertex> vertices;
    for(const auto& ring : polygon)
        for(const auto& p : ring)
            vertices.push_back({ float(p[0]), float(p[1]) });

    // ---- OpenGL upload ----
    GLuint vao, vbo, ibo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(uint32_t),
                 indices.data(),
                 GL_STATIC_DRAW);

    // Draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    return 0;
}
