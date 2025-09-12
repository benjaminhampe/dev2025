#pragma once
#include <de/terrain/BumpMesh.h>

namespace de {
namespace gpu {

// ===========================================================================
struct BumpShader
// ===========================================================================
{
    VideoDriver* m_driver;
    Shader* m_shader;
   
    int m_locProjViewMat;
	int m_locDiffuseMap; 
	int m_locNormalMap; 
   	int m_locBumpMap; 
	int m_locBumpScale; 
	
    BumpShader()
		: m_driver(nullptr)
		, m_shader(nullptr)
	{
	}
	
    //~BumpShader();

    void init( VideoDriver * driver );
    void setMaterial( const BumpMaterial & );
	
protected:
	void initShader();
};


} // end namespace gpu.
} // end namespace de.
