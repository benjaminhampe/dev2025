#include "Body.h"

/*
void createScene()
{
    // ===================
    // === init camera ===
    // ===================
    auto camera = m_driver->getCamera();
    camera->setScreenSize( SCREEN_WIDTH, SCREEN_HEIGHT );
    camera->setFOV( 90.0f );
    camera->setNearValue( 1.0f );
    camera->setFarValue( 38000.0f );
    camera->lookAt( glm::vec3(10,100,-100), glm::vec3(0,0,0) );
    camera->setMoveSpeed(1.0);
    camera->setStrafeSpeed(1.0);
    camera->setUpSpeed(1.0);

    // ===================
    // === init lights ===
    // ===================
    auto lights = m_driver->getLights();
    de::gpu::Light light0; // (sun)
    light0.pos = glm::vec3(0,300,0);
    light0.color = glm::vec3(1,1,0.9);
    lights->push_back( light0 );

    de::gpu::SMeshBuffer sphere;
    de::gpu::SMeshSphere::add(sphere, glm::vec3(100,100,100), 0xFFFFFFFF, 32, 32);
    // de::gpu::SMeshBufferTool::flipNormals( sphere );
    m_objMeshBuffer0 = H3_ObjMeshBuffer::toTMesh( sphere );
    m_objMeshBuffer0.upload();
    m_objMeshBuffer0.m_instanceMat.emplace_back( glm::mat4(1.0f) );
    m_objMeshBuffer0.m_instanceColor.emplace_back( 0xFFFFFFFF );
    m_objMeshBuffer0.uploadInstances();

    de::Image img;
    dbLoadImage(img, "../../media/SonnenSystem/z_starmap_2020_8k_gal.webp");
    m_objTexture = m_texManager.createTexture2D("background", img);

    initTessSphereShader(glm::vec3(-250,0,0), glm::vec3(100,100,100) );

    de::gpu::SO soMars(
        1.0f,
        de::gpu::SO::Minify::Linear,
        de::gpu::SO::Magnify::Linear,
        de::gpu::SO::Wrap::Repeat,
        de::gpu::SO::Wrap::Repeat,
        de::gpu::SO::Wrap::Repeat);

    de::Image img_mars_Kd;
    dbLoadImage(img_mars_Kd, "../../media/SonnenSystem/4_0_mars_8k.webp");
    m_marsDiffuseMap = m_texManager.createTexture2D("mars_Kd", img_mars_Kd, soMars);

    de::Image img_mars_bump;
    dbLoadImage(img_mars_bump, "../../media/SonnenSystem/4_0_mars_8k_bump.webp");
    m_marsBumpMap = m_texManager.createTexture2D("mars_bump", img_mars_bump, soMars);

    initTessSphereShader2(
        glm::vec3(250,0,0),
        glm::vec3(100,100,100),
        m_marsDiffuseMap,
        m_marsBumpMap,
        3.0f );

}

*/

