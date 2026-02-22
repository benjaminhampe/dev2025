#pragma once
#include "eBT_PlanetMesh.h"
#include "OpenGLBridge.h"

// Normals only tesselated and lighted planet/moon renderer, mesh must be a sphere.
struct eBT_PlanetRenderer
{
    eBT_PlanetRenderer();

    void init( de::gl::Bridge* driver );

    void draw( const Body& body );

private:
    de::gl::Bridge* m_driver;

    //eBT_PlanetMesh m_meshSphere256x128; // with tess_x64 = 16k x 8k
    eBT_PlanetMesh m_meshSphere128x64; // with tess_x64 = 8k x 4k -> default

    GLuint programId = 0;
    GLint u_modelPos = -1;
    GLint u_modelScale = -1;
    GLint u_modelMatrix = -1;
    GLint u_viewMatrix = -1;
    GLint u_projectionMatrix = -1;
    GLint u_cameraPos = -1;
    GLint u_diffuseMap = -1;
    // GLint u_diffuseMapSize = -1;
    GLint u_bumpMap = -1;
    GLint u_bumpScale = -1;
    GLint u_maxDist = -1;
    GLint u_maxTess = -1;
    
    static std::string getVertexShaderText();
    static std::string getTesselationControlShaderText();
    static std::string getTesselationEvaluationShaderText();
    static std::string getFragmentShaderText();
};
