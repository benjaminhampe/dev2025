#pragma once
#include "OpenGLBridge.h"

extern de::gl::Bridge* m_driver;

extern std::string m_mediaDir;

enum eBodyType
{
    eBT_None = 0,
    eBT_Galaxy,     // Rendered in background as a small billboard ( e.g. Milkyway, Andromeda )
    eBT_FixStar,    // Rendered in background as a small billboard ( e.g. Orion )
    eBT_Star,       // Needs it's own renderer, is a light source
    eBT_Planet,     // Another renderer that is influenced by light sources, uses sphere mesh.
    eBT_Asteroid,   // Same renderer as Planet but needs custom mesh, probably not round.
    eBT_Station,    //
    eBT_Ship,
};


struct Body
{
    // Catalog-data:
    std::string parent;     // [rend] Gravitational parent
    std::string name;       // Body name
    glm::vec3 radius;       // [rend] in [km] in all 3 dimensions
    glm::vec3 center;       // [rend] in [km] in all 3 dimensions
    eBodyType type;         // [rend] Renderer Type, Light source or not, billboard or not.

    float startAngle;       // [rend] in [°] Start condition of the simulation
    float distToParent;     // [rend] in [km] Start condition of the simulation
    float distToParentMin;  // in [km]
    float distToParentMax;  // in [km]
    float T_parent;         // in [d] A year. Time duration of rotation around parent
    float T_itself;         // in [d] A day. Time duration of rotation around itself
    float tilt;             // in [°] Angle of rotation around itself to the central bodies orbital plane
    float inclination;      // in [°] Inclination to the central star’s orbital plane
    float mass;             // in [kg] Body mass in kg
    float albedo;           // in [%]
    float surfaceTemp;      // in [K] Surface temp in Kelvin
    float surfaceGravity;   // in [m/s^2] Surface fall acceleration

    de::TRSf trs;
    glm::vec3 pos;
    glm::vec3 size;
    de::gl::Tex diffuseMap;
    de::gl::Tex bumpMap;
    float bumpScale;        // [rend] Factor
    uint32_t tessMax;

    std::string diffuseUri; // [rend] FileName (without mediaDir)
    std::string bumpUri;    // [rend] FileName (without mediaDir)
    de::Image diffuseImg;
    de::Image bumpImg;

    Body()
        : parent()
        , name()
        , radius(6.4e4f,6.3e4f,6.4e4f)
        , center(6.4e4f,6.3e4f,6.4e4f)
        , type(eBT_Planet)
        , startAngle(0.0f)
        , distToParent(1.0e6f * 150.0f)
        , distToParentMin(1.0e6f * 150.0f)
        , distToParentMax(1.0e6f * 150.0f)
        , T_parent(365.25f)
        , T_itself(1.0f)
        , tilt(23.4f)
        , inclination(0.0f)
        , mass(1.0e24f)
        , albedo(0.1f)
        , surfaceTemp(300.0f)
        , surfaceGravity(9.81f)
        , bumpScale(1.0f)
        , tessMax(64)
    {

    }

};

struct BodyDatabase
{   
    std::unordered_map< std::string, Body > m_data;

    Body& get(std::string name)
    {
        de::StringUtil::lowerCase( name );

        auto it = m_data.find( name );
        if (it == m_data.end())
        {
            DE_ERROR("No body name ", name)
            throw std::runtime_error(dbStr("No body name ",name));
        }

        return it->second;
    }

    void add(std::string parent, std::string name, float size, float dist,
             std::string diffuseUri, std::string bumpUri, float bumpScale)
    {
        de::StringUtil::lowerCase(parent);
        std::string key = name;
        de::StringUtil::lowerCase(key);
        Body& body = m_data[key];
        body.parent = parent;
        body.name = name;
        body.distToParent = dist;
        body.distToParentMin = dist;
        body.distToParentMax = dist;
        body.size = glm::vec3(size,size,size);

        float parentDist = 0.0f;
        if (!parent.empty())
        {
            parentDist = get(parent).distToParent;
            dist += parentDist;
        }
        body.pos = glm::vec3(dist,0,0);
        body.diffuseUri = diffuseUri;
        body.bumpUri = bumpUri;
        body.bumpScale = bumpScale;
    }

    void init(de::gl::Bridge* driver)
    {
        add("",    "Sun", 700, 0, "0_sun_2k.jpg", "", 1.0f );
        add("Sun", "Mercury", 100, 500, "1_mercury_8k.webp", "1_mercury_8k_bump.webp", 1.0f );
        add("Sun", "Venus", 100, 800, "2_venus_8k.webp", "2_venus_8k_bump.webp", 1.0f );
        add("Sun", "Earth", 100, 1200, "3_0_earth_8k.webp", "3_0_earth_8k_bump.webp", 1.0f );
        add("Earth", "Moon", 80, 100, "3_1_moon_8k.webp", "3_1_moon_8k_bump.webp", 1.0f );
      //add("Earth", "Eris", 80, 100, "3_1_moon_8k.webp", "3_1_moon_8k_bump.webp", 1.0f );
        add("Sun", "Mars", 100, 1500, "4_0_mars_8k.webp", "4_0_mars_8k_bump.webp", 1.0f );
        add("Mars", "Deimos", 50, 100, "4_1_deimos_8k.png", "4_1_deimos_8k.webp", 1.0f );
        add("Mars", "Phobos", 50, 100, "4_2_phobos_8k.webp", "", 1.0f );
        add("Sun", "Jupiter", 200, 1700, "5_0_jupiter_4k.jpg", "5_0_jupiter_4k_bump.jpg", 1.0f );
        add("Jupiter", "Io", 50, 150, "5_5_io_8k.webp", "5_5_io_8k_bump.webp", 1.0f );
        add("Jupiter", "Europe", 50, 200, "5_6_europa_8k.webp", "5_6_europa_2k_bump.png", 1.0f );
        add("Jupiter", "Ganymede", 50, 250, "5_7_ganymede_8k.webp", "", 1.0f );
        add("Jupiter", "Callisto", 50, 300, "5_8_callisto_8k.webp", "5_8_calisto_4k_bump.png", 1.0f );
        add("Sun", "Saturn", 100, 2000, "6_0_saturn_4k.png", "6_0_saturn_4k_bump.png", 1.0f );

        initTextures( driver );
    }

    void initTextures(de::gl::Bridge* driver)
    {
        de::gpu::SO so(
            4.0f,
            de::gpu::SO::Minify::LinearMipmapLinear,
            de::gpu::SO::Magnify::Linear,
            de::gpu::SO::Wrap::Repeat,
            de::gpu::SO::Wrap::Repeat,
            de::gpu::SO::Wrap::Repeat);

        for (auto it = m_data.begin(); it != m_data.end(); ++it)
        {
            Body & body = it->second;

            // DiffuseMap RGB
            if (!body.diffuseUri.empty())
            {
                std::string diffuseUri = m_mediaDir + body.diffuseUri;
                if (dbExistFile(diffuseUri))
                {
                    de::ImageLoadOptions opt;
                    opt.outputFormat = de::PixelFormat::RGB_24;
                    if (dbLoadImage(body.diffuseImg, diffuseUri, opt))
                    {
                        DE_OK("Loaded diffuseImg ",body.diffuseImg.str())
                        body.diffuseMap = driver->createTex2D(diffuseUri, 0, body.diffuseImg, so);
                    }
                    else
                    {
                        DE_ERROR("Cannot load diffuseImg ", diffuseUri)
                    }
                }
                else
                {
                    DE_ERROR("No file diffuseUri ", diffuseUri)
                }
            }

            // BumpMap A
            if (!body.bumpUri.empty())
            {
                std::string bumpUri = m_mediaDir + body.bumpUri;
                if (dbExistFile(bumpUri))
                {
                    de::ImageLoadOptions opt;
                    opt.outputFormat = de::PixelFormat::R8;
                    if (dbLoadImage(body.bumpImg, bumpUri, opt))
                    {
                        DE_OK("Loaded bumpImg ",body.bumpImg.str() )
                        body.bumpMap = driver->createTex2D(bumpUri, 0, body.bumpImg, so);
                    }
                    else
                    {
                        DE_ERROR("Cannot load bumpImg ", bumpUri)
                    }
                }
                else
                {
                    DE_ERROR("No file bumpUri ", bumpUri)
                }
            }

        }
    }

};



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

