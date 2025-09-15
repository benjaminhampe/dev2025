
// ===========================================================================
// ======== *.mtl File description ===========================================
// ===========================================================================

// # 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware
// # File Created: 10.01.2012 12:09:34
// newmtl 12993_diffuse        // Materialnamen:
//     Ns 30.0000              // Shininess (Glanzgrad):
//     Ni 1.5000               // Optical Density (Brechungsindex):
//     Tr 0.0000 | d 1.0000    // Dissolve (Transparenz):
//     illum 2
//     Ka 1.0000 1.0000 1.0000 // Ambient-Farbe (Umgebungslicht):
//     Kd 1.0000 1.0000 1.0000 // Diffuse-Farbe (Streulicht):
//     Ks 0.3600 0.3600 0.3600 // Specular-Farbe (Spiegellicht):
//     Ke 0.0000 0.0000 0.0000 // Emissive-Farbe (Selbstleuchtung):
//     Tf 1.0000 1.0000 1.0000 //   ?
//     map_Ka fish.jpg         //	map_Ka moon_diffuse.jpg
//     map_Kd fish.jpg         //	map_Kd moon_diffuse.jpg
//     map_bump moon_normal.jpg
//     bump moon_normal.jpg

// This applies the following transformations:
// - Scaling (-s 2.0 1.0 1.0): 
// The texture is stretched horizontally (X is doubled), 
// keeps original height (Y remains 1.0), 
// and Z scale stays unchanged.
// - Offset (-o 0.5 0.0 0.0): 
// The texture moves halfway to the right (X shifts by 0.5), 
// does not shift vertically (Y remains 0.0).
// - Brightness modification (-mm 0.2 1.5):
// - The minimum brightness is raised to 0.2 (preventing full darkness).
// - The contrast gain is increased by 1.5x, making bright areas even brighter.

#pragma once
#include <de/gpu/smesh/SMesh.h>
#include <de/gpu/mtl/PMesh.h>
#include <optional>
#include <sstream>

namespace de {
namespace gpu {
namespace mtl {

// ===========================================================================
struct MtlTex
// ===========================================================================
{
    std::wstring name;
    std::wstring debug;
    std::optional<glm::dvec3> s;// = glm::dvec3(1,1,1); // `-s x y z` 		– Texture scaling.
	std::optional<glm::dvec3> o;// = glm::dvec3(0,0,0); // `-o x y z` 		– Texture offset.
	std::optional<double> mm; 	// = 1.0;	// `-mm gamma? brigthness gain` 	– Modify texture brightness.
	std::optional<double> bm; 	// = 1.0;	// `-bm material factor/exponent 	- map_Bump scale multiplier | or map_Ks Shininess exponent.
    std::string str() const;
    std::wstring wstr() const;
};

// ===========================================================================
// ** Definition of (.mtl) material files for (.obj) files. **
// These members are used in MtlObjShader.
// ===========================================================================
struct Mtl // list of all known `.mtl` file parameters
// ===========================================================================
{
	// ### **Material Color and Illumination**
    std::wstring name;	// - `newmtl [name]` – Defines a new material with the given name.

	// 	Illumination Properties that are turned on in the model Property Editor
	// 	0 Color on and Ambient off
	// 	1 Color on and Ambient on
	// 	2 Highlight on
	// 	3 Reflection on and Ray trace on
	// 	4 Transparency: Glass on //      Reflection: Ray trace on
	// 	5 Reflection: Fresnel on and Ray trace on
	// 	6 Transparency: Refraction on  //      Reflection: Fresnel off and Ray trace on
	// 	7 Transparency: Refraction on //      Reflection: Fresnel on and Ray trace on
	// 	8 Reflection on and Ray trace off
	// 	9 Transparency: Glass on //      Reflection: Ray trace off
	// 	10 Casts shadows onto invisible surfaces
	std::optional<int> illum; // = 1;	// - `illum [model]` – Illumination model (`0` to `10`, defining shading behavior).
	std::optional<double> d; // = 1.0; // - `d [value]` – Opacity = Dissolve factor (`0.0` = fully transparent, `1.0` = fully opaque).
	std::optional<double> Ni; // = 1.0; // - `Ni [value]` – Optical density (`0.001` to `10.0`, affects refraction).
	std::optional<double> Ns; // = 30.0; // - `Ns [value]` – Specular exponent (`0` to `1000`, controls shininess).	
	//double Shininess_strength = 1.5;
	std::optional<double> sharpness; // = 0; // Reflectivity - `sharpness [value]` – Sharpness of reflections (`0` to `1000`).

	std::optional<glm::dvec3> Kd; // = glm::dvec3( 1,1,1 ); 	// - `Kd r g b` – Diffuse reflectivity (RGB values between `0.0` and `1.0`).	
	std::optional<glm::dvec3> Ka; // = glm::dvec3( 0,0,0 );	// - `Ka r g b` – Ambient reflectivity (RGB values between `0.0` and `1.0`).
	std::optional<glm::dvec3> Ks; // = glm::dvec3( 1,1,1 );	// - `Ks r g b` – Specular reflectivity (RGB values between `0.0` and `1.0`).
	//std::optional<glm::dvec3>  Ke( 0,0,0,0 )
	//std::optional<glm::dvec3>  Kt( 1,1,1,1 ) // ?
	//std::optional<glm::dvec3>  Kr( 1,1,1,1 ) // ?
	//std::optional<glm::dvec3> Tf; // - `Tf r g b` ( 1,1,1 ) – Transmission filter (RGB values between `0.0` and `1.0`).
	std::optional<MtlTex> map_Kd; // - `map_Kd [file]` – Diffuse texture map RGB.
	std::optional<MtlTex> map_d; // - `map_d [file]` – Transparency texture map A.
	std::optional<MtlTex> map_Bump; 	// CombiMap: NormalMapRGB + HeightMapA;
						// - `map_Bump [file]` – Alternative bump map definition.
						// - `bump [file]` – Bump map texture.
	std::optional<MtlTex> map_Ks; // specularColor - `map_Ks [file]` – Specular texture map.
	std::optional<MtlTex> map_Ns; // shininessFactor - `map_Ns [file]` – Specular exponent texture map.
	std::optional<MtlTex> map_Ka; // ambientColor - `map_Ka [file]` – Ambient texture map.
	
	std::optional<MtlTex> map_disp; // displacementMap - disp [file] – displacement map texture.
	std::optional<MtlTex> map_decal; // decalMap - decal [file] – Decal texture map.
	std::optional<MtlTex> map_refl; // reflectionMap - refl -type sphere [file] – Reflection map texture.

    std::wstring wstr() const;
    std::string str() const { return StringUtil::to_str(wstr()); }
};

// ===========================================================================
struct MtlLib
// ===========================================================================
{
    std::wstring name; // mtllib uri
    std::vector<Mtl> materials;
    std::wstring wstr() const;
    std::string str() const;
    int getMaterialIndex( std::wstring name ) const;
    Mtl getMaterial( std::wstring name ) const;
    uint64_t computeByteConsumption() const;
};

// ===========================================================================
struct MtlLibParser
// ===========================================================================
{
    static MtlLib readFile( const std::wstring& uri );
};

// ===========================================================================
struct ObjFile_Position // v = {x y z [w]}   # Vertex position (w is optional)
// ===========================================================================
{
    typedef std::ostringstream O;
    typedef std::wostringstream W;
    double x;
    double y;
    double z;
    std::optional<double> w;
    std::string str() const;
    std::wstring wstr() const;
};
// ===========================================================================
struct ObjFile_Normal // vn = {x y z}      # Normal vector
// ===========================================================================
{
    typedef std::ostringstream O;
    typedef std::wostringstream W;
    double x;
    double y;
    double z;
    std::string str() const;
    std::wstring wstr() const;
};

// ===========================================================================
struct ObjFile_TexCoord // vt = {u v [w]}    # Texture coordinate (w is optional)
// ===========================================================================
{
    typedef std::ostringstream O;
    typedef std::wostringstream W;
    double u;
    double v;
    std::optional<double> w;
    std::string str() const;
    std::wstring wstr() const;
};

// ===========================================================================
struct ObjFile_Face // f 1/2/3  1/2/3  1/2/3  1/2/3 ... N-Polygon
// ===========================================================================
{
    typedef std::ostringstream O;
    typedef std::wostringstream W;
    struct Item
    {
        std::optional<uint32_t> v;  // v - position
        std::optional<uint32_t> vn; // vn - normal
        std::optional<uint32_t> vt; // vt - texcoords;
        std::string str() const;
        std::wstring wstr() const;
    };
    std::vector<Item> items;
    std::string str() const;
    std::wstring wstr() const;
    uint64_t computeByteConsumption() const;
    std::string computeByteConsumptionStr() const;
};
// ===========================================================================
struct ObjFile_Group // g
// ===========================================================================
{
    std::wstring name;
    std::wstring mtllib;
    std::wstring usemtl;
    std::vector<ObjFile_Face> f; // faces;
    std::string str() const;
    std::wstring wstr() const;
    uint64_t computeByteConsumption() const;
    std::string computeByteConsumptionStr() const;
};
// ===========================================================================
struct ObjFile_Mesh // o
// ===========================================================================
{
    std::wstring name;
    std::wstring mtllib;
    std::wstring usemtl;
    std::vector<ObjFile_Group> g; // groups;
    std::string str() const;
    std::wstring wstr() const;
    uint64_t computeByteConsumption() const;
    std::string computeByteConsumptionStr() const;
};

// ===========================================================================
struct ObjFile // Collection of o meshes, m materials and v global vertices
// ===========================================================================
{
    std::wstring uri;
    MtlLib mtllib;
    std::vector<ObjFile_Position> v; // vertex positions XYZ[W]
    std::vector<ObjFile_TexCoord> vt;// vertex texCoords UV[W]
    std::vector<ObjFile_Normal> vn;  // vertex normals XYZ
    std::vector<ObjFile_Mesh> o;     // mesh objects -o
    std::wstring wstr() const;
    std::string str() const;
    uint64_t computeByteConsumption() const;
    std::string computeByteConsumptionStr() const;
};

// ===========================================================================
struct ObjFile_Parser
// ===========================================================================
{
    static ObjFile_Position parseStrPosition( const std::wstring& lineStr );
    static ObjFile_TexCoord parseStrTexCoord( const std::wstring& lineStr );
    static ObjFile_Normal parseStrNormal( const std::wstring& lineStr );
    static ObjFile_Face parseStrFace( const std::wstring& lineStr );
    static ObjFile_Face::Item parseStrFaceItem( const std::wstring& lineStr );
    static std::optional<uint32_t> parseStrUInt( const std::wstring& span );
    static std::optional<double> parseStrDouble( const std::wstring& span );
    // static std::optional<ObjFile> parse( const std::wstring & textStr );
    // static std::optional<ObjFile> parse( const std::string & textStr );
    // static std::optional<ObjFile> load( const std::wstring & uri );
    static std::optional<ObjFile> load( const std::string & uri );
};

// ===========================================================================
struct ObjFile_Converter
// ===========================================================================
{
    static SMaterial convertToSMaterial( const Mtl& mtl, VideoDriver* driver );
    static PMaterial convertToPMaterial( const Mtl& mtl, VideoDriver* driver );

    static std::vector<SMesh> convertToSMesh( const ObjFile & file, VideoDriver* driver );
    static std::vector<PMesh> convertToPMesh( const ObjFile & file, VideoDriver* driver );
};

// ===========================================================================
struct PMeshReaderOBJ
// ===========================================================================
{
    static std::vector<de::gpu::PMesh>
    load( std::string uri, VideoDriver* driver )
    {
        DE_PERF_MARKER

        auto obj = ObjFile_Parser::load( uri );
        if (!obj)
        {
            DE_ERROR("No obj")
            return {};
        }

        DE_OK("ObjFile RAM(",
              dbStrBytes(obj->computeByteConsumption()),")",
              ", uri(", uri, ")")

        /*
            m_node = new de::gpu::SMeshSceneNode( m_driver->getSceneManager(), nullptr, 0, 50,50,50);

            auto smeshes = de::gpu::mtl::ObjFile_Converter::convertSMesh( *obj, m_driver );

            de::gpu::SMesh mesh;

            if ( smeshes.size() > 0 )
            {
                DE_WARN("Set Node's mesh 0 of ", smeshes.size() )
                m_node->setMesh(smeshes[0]);
            }
        */
        return ObjFile_Converter::convertToPMesh( *obj, driver );
    }
};

} // namespace mtl.
} // namespace gpu.
} // namespace de.

