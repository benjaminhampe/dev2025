#pragma once
#include "Body.h"

// Mesh has only normals and texcoords:
// Mesh is normalized sphere, normals in range [-1,+1].
// I made this to save position vertices which i deemed unecessary.
// All planets and dwarf planets are round/spherical and therefore pretty symmetrical
// The planet center pos and planet size are encoded in the TRS modelMatrix
// A planet has atleast a diffuseMap and a bumpMap (aka displacementMap)
// Use another mesh for distorted moons and the like
// The planet-shader has lighting computations since a planet is illuminated by a star or dark
// The planet-shader does tesselation to make the bumpMap interesting, tess depends on distance to camera

class eBT_PlanetMesh
{
public:
    void create(int tessLon, int tessLat);
    void upload();
    void draw() const;

private:
    // GPU-side
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ibo = 0;

    // CPU-side
    std::vector<float> vertices;    // x,y,z,nx,ny,nz,u,v
    std::vector<uint16_t> indices;
    
    static glm::vec3 computeSphereNormal( float lon, float lat );    
};
