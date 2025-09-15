#pragma once
#include <de/gpu/mtl/PMesh.h>

namespace de {
namespace gpu {

/*
// ===========================================================================
struct PMeshAnimTimer
// ===========================================================================
{
    double start = 0.0;
    double last = 0.0;
    double curr = 0.0;
    bool running = true;
    bool paused = false;
};
*/

// ===========================================================================
struct PLight
// ===========================================================================
{
	typedef float T;
	typedef uint32_t C4;
	typedef glm::vec3 V3;
	
	// typ = 0 is a point light like the sun, dir is computed by pos only
	// typ = 1 is a directional cone light like a Taschenlampe,
	// typ = 2 is a directional cylinder with round caps like a neon light
	int typ;
	V3 siz;   	// world size of light source
	V3 pos;		// world pos
	V3 color;	// emission color
	V3 dir;		// for directed light sources.
	T maxdist;    	// distance m_Radius = 500.0f;
    T power;        // intensity
	//V3 clipPos; // for Shader, computed by CPU

    std::string str() const;
    std::wstring wstr() const;
};

// ===========================================================================
struct PMaterialRenderer
// ===========================================================================
{
	typedef float T;
	typedef uint32_t C4;
	typedef glm::vec3 V3;
	typedef glm::mat4 M4;
	typedef std::vector< M4 > M4_Vector;
		
    VideoDriver* m_driver;
    
    std::vector< PLight > m_lights;
	
    float m_screenGamma;
    //ParallaxMtl_AnimTimer m_animTimer;
    PMaterial m_material;

#ifdef USE_RENDER_STATS
   uint64_t m_FrameBuffers = 0;
   uint64_t m_FrameVertices = 0;
   uint64_t m_FrameIndices = 0;
   uint64_t m_FramePrimitives = 0;
   uint64_t m_TotalFrames = 0;
   uint64_t m_TotalBuffers = 0;
   uint64_t m_TotalVertices = 0;
   uint64_t m_TotalIndices = 0;
   uint64_t m_TotalPrimitives = 0;
#endif

public:
    PMaterialRenderer();
    void init( VideoDriver* driver );
    bool setMaterial(PMaterial& material);
    void unsetMaterial(PMaterial& material);

    // void draw(PMesh & mesh)
    // {
    //     for (PMeshBuffer & limb : mesh.m_limbs)
    //     {
    //         draw(limb);
    //     }
    // }
    // void draw(PMeshBuffer & limb)
    // {
    //     limb.
    //     setMaterial( limb.m_material );
    //     limb.draw();
    // }

	//void draw(ParallaxMtl_MeshBuffer & limb, uint32_t nInstances);
	//void draw(ParallaxMtl_MeshBuffer & limb, const M4_Vector& instances);
		
    void addLight(  int typ = 0,
                    V3 pos = V3(0,0,0),
                    V3 siz = V3(10,10,10),
                    T maxdist = 500,
                    V3 dir = V3(0,-1,0),
                    V3 color = dbRGBAf( 0XFFFFFFFF ) );

    void setLights(Shader* shader);

    void drawDebugLights();

protected:
    static std::string getShaderName( const PMaterial& material );
    static std::string getVS( const PMaterial & material );
    static std::string getFS( const PMaterial & material );
    
public:
   // // === Uniform Gamma ====
   // float getGamma() const { return m_ScreenGamma; }
   // void setGamma( float gamma ) { m_ScreenGamma = gamma; }

   // // === Uniform Lights ===
   // uint32_t getLightCount() const { return uint32_t(m_Lights.size()); }
   // SMaterialLight & getLight( uint32_t light ) { return m_Lights[ light ]; }
   // SMaterialLight const & getLight( uint32_t light ) const { return m_Lights[ light ]; }
   // void drawDebugLights();
   
   // New: Mostly for render stats, counts vertices, indices and primitives
   // void beginFrame();
   // void endFrame();
   // void animate( double pts ); // called once each frame, in Driver::beginFrame()
};

} // namespace gpu.
} // namespace de.
