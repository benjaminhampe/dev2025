#include <de/gpu/mtl/MTL.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

#include <cwctype> // For std::iswspace()
#include <vector>

namespace de {
namespace gpu {
namespace mtl {

// ===========================================================================
struct tSpan
// ===========================================================================
{
    const std::wstring* m_ref = nullptr;  // Pointer to the main string
    size_t m_pos = 0;         // Start position in the string
    size_t m_count = 0;       // Length of the substring

    tSpan() : m_ref(nullptr), m_pos(0), m_count(0)
    {

    }

    tSpan( const std::wstring* _ref ) : tSpan()
    {
        if (_ref)
        {
            m_ref = _ref;
            m_count = _ref->size();
        }
    }

    tSpan( const std::wstring* _ref, size_t _pos, size_t _count) : tSpan()
    {
        set(_ref,_pos,_count);
    }

    void reset()
    {
        m_ref = nullptr;
        m_pos = 0;
        m_count = 0;
    }

    std::wstring toStrW() const
    {
        std::wostringstream o;
        if (m_ref)
        {
            for (size_t i = 0; i < m_count; ++i)
            {
                o << m_ref->at( m_pos + i);
            }
        }
        return o.str();
    }

    void set( const std::wstring* _ref, size_t _pos, size_t _count)
    {
        m_ref = _ref;
        m_pos = _pos;
        m_count = _count;

        if (m_ref && m_ref->size() && m_count)
        {
            if (m_pos > m_ref->size() - 1)
            {
                DE_WARN("m_pos > m_ref->size() - 1")
                reset();
            }
            if (m_count > m_ref->size())
            {
                DE_WARN("m_count > m_ref->size()")
                m_count = m_ref->size();
            }
            if (m_pos + m_count > m_ref->size() - 1)
            {
                //DE_WARN("m_pos + m_count > m_ref->size() - 1")
                m_count = m_ref->size() - 1 - m_pos;
            }
            if (m_count < 1)
            {
                DE_WARN("count < 1")
                reset();
            }
        }
    }

    wchar_t at( size_t i ) const
    {
        if (!m_ref) return 0;
        if (i >= m_count) return 0;
        if (m_pos + i >= m_ref->size() ) return 0;
        return (*m_ref).at(m_pos + i);
    }

    tSpan substr( size_t i, size_t n = 0 ) const
    {
        if (m_count < 1) return {};
        if (i >= m_count - 1) return {};

        if (n == 0)
        {
            return { m_ref, m_pos + i, m_count - i };
        }
        else
        {
            if (n >= m_count)
            {
                n = m_count;
            }
            if (i + n >= m_count)
            {
                n = m_count - i;
            }
            if (n > 0)
            {
                return { m_ref, m_pos + i, n };
            }
        }
        return {};
    }

    static tSpan trimSpanLeft(const tSpan& span)
    {
        size_t i = 0;
        while (i < span.size() && std::iswspace(span.at(i)))
        {
            i++;
        }
        return span.substr( i ); // get right part
    }

    static tSpan trimSpanRight(const tSpan& span)
    {
        size_t i = span.size();
        while (i > 0 && std::iswspace(span.at(i - 1)))
        {
            i--;
        }
        return span.substr( 0, span.size() - i ); // get left part
    }

    static tSpan trimSpan(const tSpan& span)
    {
        return trimSpanLeft(trimSpanRight(span));
    }

    bool empty() const { return !m_ref || m_count < 1; }

    size_t size() const { return m_count; }

    size_t pos() const { return m_pos; }

    tSpan trim() const { return trimSpan(*this); }

    // txt.find( searchChar, pos1 );

    size_t find( wchar_t searchChar, size_t pos_start = 0 ) const
    {
        size_t i = pos_start;
        while ( i < size() )
        {
            if ( at(i) == searchChar )
            {
                return i;
            }
            i++;
        }

        return std::string::npos;
    }

    static std::vector<tSpan>
    splitSpan(const tSpan& txt, wchar_t searchChar)
    {
        std::vector<tSpan> liste;

        size_t pos1 = 0;
        size_t pos2 = txt.find( searchChar );

        while ( pos2 != std::string::npos )
        {
            auto a = txt.substr( pos1, pos2-pos1 );
            if ( !a.empty() )
            {
                liste.emplace_back( std::move( a ) );
            }

            pos1 = pos2 + 1;
            pos2 = txt.find( searchChar, pos1 );
        }

        auto a = txt.substr( pos1 );
        if (!a.empty())
        {
            liste.emplace_back( std::move( a ) );
        }

        return liste;
    }

    std::vector<tSpan> split(wchar_t searchChar) const
    {
        return splitSpan(*this, searchChar);
    }

};


// ===========================================================================
std::string MtlTex::str() const
// ===========================================================================
{
    std::ostringstream k; k << StringUtil::to_str(name);
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    return k.str();
}

// ===========================================================================
std::wstring MtlTex::wstr() const
// ===========================================================================
{
    std::wostringstream k; k << FileSystem::fileName(name);
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    return k.str();
}


// ===========================================================================
struct MtlUtil // list of all known `.mtl` file parameters
// ===========================================================================
{
    static std::wstring toStrDouble( const double & number );
    static std::wstring toStrInt( const int & number );
    static std::wstring toStrColor( const glm::dvec3 & opt_color );
    //static std::wstring toStrTexture( const MtlTex & opt_tex );
    static double parseStrDouble( std::wstring lineStr );
    static int parseStrInt( std::wstring lineStr );
    static glm::dvec3 parseStrColor( std::wstring lineStr );
    static MtlTex parseStrTexture( std::wstring lineStr, std::string mtl_dir );
};

// static
std::wstring
MtlUtil::toStrDouble( const double & number )
{
    std::wostringstream o;
    o << number;
    return o.str();
}

// static
std::wstring
MtlUtil::toStrInt( const int & number )
{
    std::wostringstream o;
    o << number;
    return o.str();
}

// static
std::wstring
MtlUtil::toStrColor( const glm::dvec3 & color )
{
    std::wostringstream o;
    o << color.x << "," << color.y << "," << color.z;
    return o.str();
}

/*
//static
std::wstring
MtlUtil::toStrTexture( const MtlTex & opt_tex )
{
    std::wostringstream o;
    if ( opt_tex )
    {
        const auto & tex = *opt_tex;
        o << tex.wstr();
    }
    else
    {
        o << "nullopt";
    }
    return o.str();
}
*/

// static
double
MtlUtil::parseStrDouble( std::wstring lineStr )
{
    std::wistringstream line(lineStr);
    std::wstring name; double number = 0.0; // Load
    line >> name; line >> number; // Load
    //DE_DEBUG("Parsed Double[", StringUtil::to_str(name), "] = ", number, "; ") // Store
    return number;
}

// static
int
MtlUtil::parseStrInt( std::wstring lineStr )
{
    std::wistringstream line(lineStr);
    std::wstring name; int number = 0; // Load
    line >> name; line >> number; // Load
    //DE_DEBUG("Parsed Int[", StringUtil::to_str(name), "] = ", number, "; ") // Store
    return number;
}

// static
glm::dvec3
MtlUtil::parseStrColor( std::wstring lineStr )
{
    std::wistringstream line(lineStr);
    std::wstring name; double x = 1.0, y = 1.0, z = 1.0; // Load.
    line >> name; line >> x; line >> y; line >> z; // Load.
    auto color = glm::dvec3(x,y,z); // Store.
    //DE_DEBUG("Parsed Color[", StringUtil::to_str(name), "] = ", color.x, ", ", color.y, ", ", color.z, "; ")
    return color;
}

// static
MtlTex
MtlUtil::parseStrTexture( std::wstring lineStr, std::string mtl_dir )
{
    MtlTex tex;
    tex.debug = lineStr;

    std::wistringstream line(lineStr);
    std::wstring param0;
    line >> param0;

    bool ok = false;
    while (line >> param0)
    {
        if (param0 == L"-bm")
        {
            double v = 1.0;
            line >> v;
            tex.bm = v;
            //DE_DEBUG("Parsed -bm = ", tex.bm) // double 'map_Bump' BumpScaleFactor or 'map_Ks' Shininess_strength = 1.5;
        }
        else if (param0 == L"-mm")
        {
            double v = 1.0;
            line >> v;
            tex.mm = v;
            //DE_DEBUG("Parsed -mm = ", tex.mm)
        }
        else if (param0 == L"-o")
        {
            double x = 0.0; double y = 0.0; double z = 0.0;
            line >> x; line >> y; line >> z;
            auto o = glm::dvec3(x,y,z); // Offset (animated water)
            tex.o = o;
            //DE_DEBUG("Parsed -o = ", o.x, ", ", o.y, ", ", o.z, "; ")
        }
        else if (param0 == L"-s")
        {
            double x = 0.0; double y = 0.0; double z = 0.0;
            line >> x; line >> y; line >> z;
            auto s = glm::dvec3(x,y,z); // Scale (repeat)
            tex.s = s;
            //DE_DEBUG("Parsed -s = ", s.x, ", ", s.y, ", ", s.z, "; ")
        }
        else // uri
        {
            std::string tex_uri = de_mbstr(param0);

            if (StringUtil::startsWith(tex_uri,".\\\\"))
            {
                tex_uri = tex_uri.substr( 3 );
            }
            if (StringUtil::startsWith(tex_uri,".\\"))
            {
                tex_uri = tex_uri.substr( 2 );
            }
            if (StringUtil::startsWith(tex_uri,".//"))
            {
                tex_uri = tex_uri.substr( 3 );
            }
            if (StringUtil::startsWith(tex_uri,"./"))
            {
                tex_uri = tex_uri.substr( 2 );
            }

            if (dbExistFile(tex_uri))
            {
                //DE_DEBUG("Texture file exist ", tex_uri)
            }
            else
            {
                tex_uri = mtl_dir + "/" + tex_uri;
                if (dbExistFile(tex_uri))
                {
                    //DE_DEBUG("Texture file exist2 ", tex_uri)
                }
                else
                {
                    DE_ERROR("No texture file ", tex_uri)
                }
            }

            tex.name = de_wstr(tex_uri);

        }
    }
    //DE_DEBUG("Parsed tex ", tex.str())
    return tex;
}

std::wstring Mtl::wstr() const
{
    std::wostringstream o;
    // =============== newmtl ===========================================
    o << "name(" << name << ")";
    // =============== Numbers ===========================================
    if(illum) o << ", illum(" << MtlUtil::toStrInt(*illum) << ")";
    if(d) o << ", d(" << MtlUtil::toStrDouble(*d) << ")";
    if(Ni) o << ", Ni(" << MtlUtil::toStrDouble(*Ni) << ")";
    if(Ns) o << ", Ns(" << MtlUtil::toStrDouble(*Ns) << ")";
    // =============== Colors ============================================
    if(Kd) o << ", Kd(" << MtlUtil::toStrColor(*Kd) << ")";
    if(Ks) o << ", Ks(" << MtlUtil::toStrColor(*Ks) << ")";
    if(Ka) o << ", Ka(" << MtlUtil::toStrColor(*Ka) << ")";
    //o << ", Tf(" << toStrColor(Tf) << ")";
    // =============== Textures ==========================================
    if(map_Kd) o << ", map_Kd(" << map_Kd->wstr() << ")";
    if(map_Bump) o << ", map_Bump(" << map_Bump->wstr() << ")";
    if(map_Ka) o << ", map_Ka(" << map_Ka->wstr() << ")";
    if(map_Ks) o << ", map_Ks(" << map_Ks->wstr() << ")";
    if(map_Ns) o << ", map_Ns(" << map_Ns->wstr() << ")";
    //if(map_Tr) o << ", map_Tr(" << map_Tr->wstr() << ")";
    if(map_disp) o << ", map_disp(" << map_disp->wstr() << ")";
    if(map_decal) o << ", map_decal(" << map_decal->wstr() << ")";
    if(map_refl) o << ", map_refl(" << map_refl->wstr() << ")";
    return o.str();
}


// ===========================================================================
// struct MtlLib
// ===========================================================================

std::wstring MtlLib::wstr() const
{
    std::wostringstream o;
    o << "mtllib.Name " << name << "\n";
    o << "mtllib.Count = " << materials.size() << "\n";
    for (size_t i = 0; i < materials.size(); ++i)
    {
        const auto& mtl = materials[i];
        o << "mtllib[" << i << "] " << mtl.wstr() << "\n";
    }
    return o.str();
}

std::string MtlLib::str() const { return StringUtil::to_str(wstr()); }

int MtlLib::getMaterialIndex( std::wstring name ) const
{
    if (name.empty())
    {
        return -1;
    }

    auto it = std::find_if(materials.begin(),materials.end(),
                           [name](const Mtl & cached ) { return cached.name == name; });
    if (it == materials.end())
    {
        DE_ERROR("No such mtllib ",StringUtil::to_str(name))
        return -1;
    }

    return int(std::distance(materials.begin(), it));
}

Mtl MtlLib::getMaterial( std::wstring name ) const
{
    if (name.empty())
    {
        DE_ERROR("Empty Mtl material name")
        //throw std::runtime_error("Empty Mtl material name");
        return {};
    }

    auto it = std::find_if(materials.begin(),materials.end(),
                           [name](const Mtl & cached ) { return cached.name == name; });
    if (it == materials.end())
    {
        auto msg = dbStr("No such Mtl ",StringUtil::to_str(name));
        throw std::runtime_error(msg);
    }

    return *it;
}

uint64_t MtlLib::computeByteConsumption() const
{
    uint64_t n = sizeof( *this );
    n += materials.capacity() * sizeof( Mtl );
    return n;
}

// std::string name;	// - `newmtl [name]` – Defines a new material with the given name.
// int illum = 1;	// - `illum [model]` – Illumination model (`0` to `10`, defining shading behavior).
// double d = 1.0; // - `d [value]` – Opacity = Dissolve factor (`0.0` = fully transparent, `1.0` = fully opaque).
// double Ni = 1.0; // - `Ni [value]` – Optical density (`0.001` to `10.0`, affects refraction).
// double Ns = 30.0; // - `Ns [value]` – Specular exponent (`0` to `1000`, controls shininess).
// double sharpness = 0; //  - `sharpness [value]` – Sharpness of reflections (`0` to `1000`).
// double Shininess_strength; // Ni = 1.5;
// double Reflectivity; // Tr
// double Shininess; // Ns

// glm::dvec3 Kd = glm::dvec3( 1,1,1 ); // diffuseColor; Kd r g b – Diffuse reflectivity (RGB values between `0.0` and `1.0`).
// glm::dvec3 Ka = glm::dvec3( 0,0,0 );	// ambientColor; Ka r g b – Ambient reflectivity (RGB values between `0.0` and `1.0`).
// glm::dvec3 Ks = glm::dvec3( 1,1,1 );	// specularColor Ks r g b – Specular reflectivity (RGB values between `0.0` and `1.0`).
// glm::dvec3 Tf = glm::dvec3( 0,0,0 );	// transparentColor Tf r g b – Transmission filter (RGB values between `0.0` and `1.0`).
// glm::dvec3 Ke; // emissive_color; // Ke
// glm::dvec3 Kt; // TransparentColor; // Kt, Tf
// glm::dvec3 Kr; // ReflectiveColor = Emissive?

// MtlTex map_Kd; // - `map_Kd [file]` – Diffuse texture map RGB.
// MtlTex map_d; // - `map_d [file]` – Transparency texture map A.
// MtlTex map_Bump; // CombiMap: NormalMapRGB + HeightMapA;
// - `map_Bump [file]` – Alternative bump map definition.
// - `bump [file]` – Bump map texture.
// MtlTex map_Ks; // specularColor - `map_Ks [file]` – Specular texture map.
// MtlTex map_Ns; // shininessFactor - `map_Ns [file]` – Specular exponent texture map.
// MtlTex map_Ka; // ambientColor - `map_Ka [file]` – Ambient texture map.
// MtlTex map_disp; // displacementMap - disp [file] – displacement map texture.
// MtlTex map_decal; // decalMap - decal [file] – Decal texture map.
// MtlTex map_refl; // reflectionMap - refl -type sphere [file] – Reflection map texture.

// static
MtlLib
// ===========================================================================
MtlLibParser::readFile( const std::wstring& uri )
// ===========================================================================
{
    DE_PERF_MARKER

    if (!dbExistFile(uri))
    {
        DE_ERROR("No file. ", de_mbstr(uri))
        return {};
    }

    std::wstring fileContentW = de::FileSystem::loadStrW(uri);
    if (fileContentW.empty())
    {
        DE_ERROR("No file content. ", de_mbstr(uri))
        return {};
    }

    const std::string mtl_dir = FileSystem::fileDir( de_mbstr(uri) );

    MtlLib lib;

    lib.name = uri;

    std::wistringstream textStream(fileContentW);

    std::wstring lineStr;



    while (std::getline(textStream, lineStr))
    {
        std::wistringstream line(lineStr);

        std::wstring param0;
        line >> param0;

        if (param0.empty()) { continue; } // Skip empty line...

        dbStrLowerCase(param0);

        if (param0 == L"newmtl")
        {
            line >> param0;
            lib.materials.emplace_back();
            Mtl & mtl = lib.materials.back();
            mtl.name = param0;
        }
        else if (param0 == L"illum")
        {
            Mtl & mtl = lib.materials.back();
            mtl.illum = MtlUtil::parseStrInt(lineStr);
        }
        else if (param0 == L"d")
        {
            Mtl & mtl = lib.materials.back();
            mtl.d = MtlUtil::parseStrDouble(lineStr);
        }
        else if (param0 == L"ni")
        {
            Mtl & mtl = lib.materials.back();
            mtl.Ni = MtlUtil::parseStrDouble(lineStr);
        }
        else if (param0 == L"ns")
        {
            Mtl & mtl = lib.materials.back();
            mtl.Ns = MtlUtil::parseStrDouble(lineStr);
        }
        else if (param0 == L"kd")
        {
            Mtl & mtl = lib.materials.back();
            mtl.Kd = MtlUtil::parseStrColor( lineStr);
        }
        else if (param0 == L"ks")
        {
            Mtl & mtl = lib.materials.back();
            mtl.Ks = MtlUtil::parseStrColor( lineStr);
        }
        else if (param0 == L"ka")
        {
            Mtl & mtl = lib.materials.back();
            mtl.Ka = MtlUtil::parseStrColor( lineStr);
        }
        //else if (param0 == "Tf") 	{ mtl.Tf = MtlUtil::parseStrColor( lineStr); }
        else if (param0 == L"map_kd")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_Kd = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_d")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_d = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_bump" || param0 == L"bump" || param0 == L"bumpmap")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_Bump = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_ks")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_Ks = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_ns")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_Ns = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_ka")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_Ka = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_disp" || param0 == L"disp")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_disp = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_decal" || param0 == L"decal")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_decal = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
        else if (param0 == L"map_refl" || param0 == L"refl")
        {
            Mtl & mtl = lib.materials.back();
            mtl.map_refl = MtlUtil::parseStrTexture(lineStr, mtl_dir);
        }
    }
    return lib;
}



// ===========================================================================
// ======== *.obj File description ===========================================
// ===========================================================================

// v x y z [w]   # Vertex position (w is optional)
// vt u v [w]    # Texture coordinate (w is optional)
// vn x y z      # Normal vector

// usemtl Green
// s 1 (Smoothing group)
//  - The s command enables or disables smoothing groups, which affect shading and normal calculations.
//  - s 1 means smoothing is enabled, causing adjacent faces in the same group to share vertex normals for smooth shading.
//  - s off would disable smoothing, making each face flat-shaded.
// v 2.980785 -1.000000 0.804910
// vt 3.000000 -1.000000 1.000000
// vn 2.980785 -1.000000 1.195090
//  - vn 0.000000 -1.000000 0.000000
// f 32//1 10//2 33//3
//  - v/vt/vn → Vertex Index / Texture Coordinate Index / Normal Index
//  - v//vn
// f 1//4 10//2 2//5
// f 31//6 10//2 32//1
// f 30//7 10//2 31//6
// f 29//8 10//2 30//7
// f 28//9 10//2 29//8
// f 27//10 10//2 28//9
// f 26//11 10//2 27//10
// f 25//12 10//2 26//11
// Each f command represents a face, and the notation format used here includes:
// v/vt/vn → Vertex Index / Texture Coordinate Index / Normal Index
// v//vn → Vertex Index / Normal Index (no texture coordinates)
// Breaking down:
// - f 32//1 10//2 33//3
// - Vertex indices: 32, 10, 33 (referencing positions from global v list)
// - Normal indices: 1, 2, 3 (referencing normals from global vn list)
// - Texture coordinates are missing (hence // notation)
// - f 1//4 10//2 2//5
// - Vertex indices: 1, 10, 2
// - Normal indices: 4, 2, 5
// - Texture coordinates omitted
// This means these faces will use vertex positions and apply normal vectors for lighting, but lack texture coordinate references.
// Would you like a deeper dive into how smoothing groups affect shading calculations in rendering engines?

// ===========================================================================
// struct ObjFile_Position // v = {x y z [w]}   # Vertex position (w is optional)
// ===========================================================================

std::string ObjFile_Position::str() const   { O o; o<<"v "<<x<<' '<<y<<' '<<z; if (w) { o<<' '<<(*w); } return o.str();}
std::wstring ObjFile_Position::wstr() const { W o; o<<"v "<<x<<' '<<y<<' '<<z; if (w) { o<<' '<<(*w); } return o.str(); }

// ===========================================================================
// struct ObjFile_Normal // vn = {x y z}      # Normal vector
// ===========================================================================

std::string ObjFile_Normal::str() const {   O o; o << "vn " << x << ' ' << y << ' ' << z; return o.str(); }
std::wstring ObjFile_Normal::wstr() const { W o; o << "vn " << x << ' ' << y << ' ' << z; return o.str(); }

// ===========================================================================
// struct ObjFile_TexCoord // vt = {u v [w]}    # Texture coordinate (w is optional)
// ===========================================================================

std::string ObjFile_TexCoord::str() const
{
    O o; o << "vt " << u << ' ' << v;
    if (w) { o << ' ' << (*w); } return o.str();
}
std::wstring ObjFile_TexCoord::wstr() const
{
    W o; o << "vt " << u << ' ' << v;
    if (w) { o << ' ' << (*w); } return o.str();
}


// ===========================================================================
// struct ObjFile_Face // f 1/2/3  1/2/3  1/2/3  1/2/3 ... N-Polygon
// ===========================================================================

std::string ObjFile_Face::Item::str() const
{
    O o; o << v.value_or(0);
    if (vt || vn)
    {
        o << "/";
        if (vt) { o << (*vt); }
        o << "/";
        if (vn) { o << (*vn); }
    }
    return o.str();
}

std::wstring ObjFile_Face::Item::wstr() const
{
    W o; o << v.value_or(0);
    if (vt || vn)
    {
        o << "/";
        if (vt) { o << (*vt); }
        o << "/";
        if (vn) { o << (*vn); }
    }
    return o.str();
}

std::string ObjFile_Face::str() const
{
    O o; o << "f"; for (const Item& i : items) { o << " " << i.str(); } return o.str();
}
std::wstring ObjFile_Face::wstr() const
{
    W o; o << "f"; for (const Item& i : items) { o << " " << i.wstr(); } return o.str();
}

uint64_t ObjFile_Face::computeByteConsumption() const
{
    uint64_t n = sizeof( *this );
    n += items.capacity() * sizeof( Item );
    return n;
}

std::string ObjFile_Face::computeByteConsumptionStr() const
{
    return dbStrBytes(computeByteConsumption() );
}

// ===========================================================================
// struct ObjFile_Group // g
// ===========================================================================

std::string ObjFile_Group::str() const
{
    std::ostringstream o;
    o << "g(" << de_mbstr(name) << ")";
    o << ", faces(" << f.size() << ")";
    if (mtllib.size() > 0) o << ", mtllib(" << de_mbstr(mtllib) << ")";
    if (usemtl.size() > 0) o << ", usemtl(" << de_mbstr(usemtl) << ")";
    //for (const auto& face : f) { o << face.str() << "\n"; }
    return o.str();
}
std::wstring ObjFile_Group::wstr() const
{
    std::wostringstream o;
    o << "g(" << name << ")";
    o << ", faces(" << f.size() << ")";
    if (mtllib.size() > 0) o << ", mtllib(" << mtllib << ")";
    if (usemtl.size() > 0) o << ", usemtl(" << usemtl << ")";
    //for (const auto& face : f) { o << face.wstr() << "\n"; }
    return o.str();
}

uint64_t ObjFile_Group::computeByteConsumption() const
{
    uint64_t n = sizeof( *this );
    for (const auto & F : f) { n += F.computeByteConsumption(); }
    return n;
}

std::string ObjFile_Group::computeByteConsumptionStr() const
{
    return dbStrBytes(computeByteConsumption() );
}

// ===========================================================================
// struct ObjFile_Mesh // o
// ===========================================================================

std::string ObjFile_Mesh::str() const
{
    std::ostringstream o;
    o << "m " << StringUtil::to_str(name) << "\n";
    if (mtllib.size() > 0) o << "mtllib " << StringUtil::to_str(mtllib) << "\n";
    if (usemtl.size() > 0) o << "usemtl " << StringUtil::to_str(usemtl) << "\n";
    for (const auto& group : g) { o << group.str(); }
    return o.str();
}

std::wstring ObjFile_Mesh::wstr() const
{
    std::wostringstream o;
    o << "m " << name << ", ";
    if (mtllib.size() > 0) o << ", mtllib " << mtllib;
    if (usemtl.size() > 0) o << ", usemtl " << usemtl;
    o << ", g(" << g.size() << ")";
    for (const auto& group : g)
    {
        o << group.wstr(); }
    return o.str();
}

uint64_t ObjFile_Mesh::computeByteConsumption() const
{
    uint64_t n = sizeof( *this );
    for (const auto & G : g) { n += G.computeByteConsumption(); }
    return n;
}

std::string ObjFile_Mesh::computeByteConsumptionStr() const
{
    return dbStrBytes(computeByteConsumption() );
}

// ===========================================================================
// struct ObjFile // Collection of o meshes, m materials and v global vertices
// ===========================================================================

std::wstring ObjFile::wstr() const
{
    std::wostringstream k; k << "\n"
         "ObjFile.uri = " << uri << "\n"
                "ObjFile.v.count = " << v.size() << "\n"
                     "ObjFile.vt.count = " << vt.size() << "\n"
                      "ObjFile.vn.count = " << vn.size() << "\n"
                      "ObjFile.mtllib.Name = " << mtllib.name << "\n"
                        "ObjFile.mtllib.Count = " << mtllib.materials.size() << "\n";
    for (size_t i = 0; i < mtllib.materials.size(); ++i)
    {
        const auto& mtl = mtllib.materials[i];
        k << "ObjFile.mtllib[" << i << "] " << mtl.wstr() << "\n";
    }

    k << "ObjFile.o.count = " << o.size() << "\n";
    for (size_t i = 0; i < o.size(); ++i)
    {
        const auto& O = o[i];
        k << "ObjFile."
             "o[" << i << "|" << O.name << "].g(" << O.g.size() << ") :: "
                                                           "RAM(" << de_wstr(O.computeByteConsumptionStr()) << ")";
        //"BBox(" << de_wstr(O.) << ")";
        if (O.mtllib.size() > 0) k << ", mtllib " << O.mtllib;
        if (O.usemtl.size() > 0) k << ", usemtl " << O.usemtl;
        k << "\n";

        for (size_t j = 0; j < O.g.size(); ++j)
        {
            const auto& G = O.g[j];
            k << "ObjFile."
                 "o[" << i << "|" << O.name << "]."
                                       "g[" << j << "|" << G.name << "] "
                                       "RAM(" << de_wstr(G.computeByteConsumptionStr()) << ")"
                                                           //"BBox(" << de_wstr(O.) << ")";
                                                           ", f(" << G.f.size() << ")\n";
        }
    }
    return k.str();
}

std::string ObjFile::str() const { return StringUtil::to_str(wstr()); }

uint64_t ObjFile::computeByteConsumption() const
{
    uint64_t n = sizeof( *this );
    n += mtllib.computeByteConsumption();
    n += v.capacity() * sizeof( ObjFile_Position );
    n += vt.capacity() * sizeof( ObjFile_TexCoord );
    n += vn.capacity() * sizeof( ObjFile_Normal );
    for (const auto & O : o) { n += O.computeByteConsumption(); }
    return n;
}

std::string ObjFile::computeByteConsumptionStr() const
{
    return dbStrBytes(computeByteConsumption() );
}


// ===========================================================================
// ===========================================================================
// ===========================================================================

// static
ObjFile_Position
ObjFile_Parser::parseStrPosition( const std::wstring& lineStr )
{
    ObjFile_Position v;
    std::wistringstream iss(lineStr);
    std::wstring s; double x = 0.0, y = 0.0, z = 0.0, w = 0.0;
    iss >> s;
    if (iss >> x) { v.x = x; }
    if (iss >> y) { v.y = y; }
    if (iss >> z) { v.z = z; }
    if (iss >> w) { v.w = w; }
    // DE_DEBUG(v.str())
    return v;
}

// static
ObjFile_TexCoord
ObjFile_Parser::parseStrTexCoord( const std::wstring& lineStr )
{
    ObjFile_TexCoord vt;
    std::wistringstream line(lineStr);
    std::wstring s; double u = 0.0, v = 0.0, w = 0.0;
    line >> s;
    if (line >> u) { vt.u = u; }
    if (line >> v) { vt.v = v; }
    if (line >> w) { vt.w = w; }
    // DE_DEBUG(vt.str())
    return vt;
}

// static
ObjFile_Normal
ObjFile_Parser::parseStrNormal( const std::wstring& lineStr )
{
    ObjFile_Normal vn;
    std::wistringstream line(lineStr);
    std::wstring s; double x = 0.0, y = 0.0, z = 0.0;
    if (line >> s) {  }
    else { DE_ERROR("Not parsed string 'vn'") }
    if (line >> x) { vn.x = x; }
    else { DE_ERROR("No vn.x value parsed!") }
    if (line >> y) { vn.y = y; }
    else { DE_ERROR("No vn.y value parsed!") }
    if (line >> z) { vn.z = z; }
    else { DE_ERROR("No vn.z value parsed!") }
    // DE_DEBUG(vn.str())
    return vn;
}


// static
ObjFile_Face::Item
ObjFile_Parser::parseStrFaceItem( const std::wstring& txt )
{
    ObjFile_Face::Item item;

    tSpan span(&txt);
    span.trim();
    if (span.empty())
    {
        DE_WARN("No span left to analyze, txt = ", StringUtil::to_str(txt))
        return {};
    }

    size_t pos1 = span.find( L'/' );
    size_t pos2 = 0;

    if (pos1 == std::string::npos)
    {
        item.v = parseStrUInt( span.toStrW() );
    }
    else
    {
        tSpan span_v = span.substr( 0, pos1 );
        item.v = parseStrUInt( span_v.toStrW() );

        // We expect two slashes in total now...
        size_t pos2 = span.find( L'/', pos1+1 );

        if (pos2 == std::string::npos)
        {
            DE_ERROR("Expected two slashes")
        }
        else
        {
            tSpan span_vt = span.substr( pos1+1, pos2-pos1 );
            item.vt = parseStrUInt( span_vt.toStrW() );

            tSpan span_vn = span.substr( pos2+1 );
            item.vn = parseStrUInt( span_vn.toStrW() );
        }
    }

    return item;
}

// static
ObjFile_Face
ObjFile_Parser::parseStrFace( const std::wstring& lineStr )
{
    //DE_PERF_MARKER
    ObjFile_Face objFace;
    std::wstring f;
    std::wstring n;

    std::wistringstream s(lineStr);

    s >> f;

    while (s >> n)
    {
        objFace.items.emplace_back( parseStrFaceItem(n) );
    }

    //DE_DEBUG(objFace.toStr())
    return objFace;
}

// static
std::optional<uint32_t>
ObjFile_Parser::parseStrUInt( const std::wstring& txt )
{
    if (txt.empty()) { return {}; }
    std::wistringstream s(txt);
    uint32_t u = 0;
    if (s >> u) { return u; }
    return {};
}

// static
std::optional<double>
ObjFile_Parser::parseStrDouble( const std::wstring& txt )
{
    if (txt.empty()) { return {}; }
    std::wistringstream s(txt);
    double d = 0.0;
    if (s >> d) { return d; }
    return {};
}

// static
std::optional<ObjFile>
ObjFile_Parser::load( const std::string & uri_obj )
{
    DE_PERF_MARKER
    //DE_WARN("Load obj ", uri_obj)

    ObjFile objFile;
    objFile.uri = de_wstr(uri_obj);

    if (!dbExistFile(uri_obj))
    {
        DE_ERROR("No obj file ", uri_obj)
        return {};
    }

    std::wstring fileContentW;
    {
        DE_PERF_MARKER
        fileContentW = FileSystem::loadStrW( uri_obj );
        DE_OK("fileContentW = ", dbStrBytes( fileContentW.size() * sizeof(wchar_t) ))
    }


    // objFile.o.emplace_back();
    // ObjFile_Mesh & currMesh = objFile.o.back();

    // currMesh.g.emplace_back();
    // ObjFile_Group & currGroup = currMesh.g.back();

    // currGroup.f.emplace_back();
    // ObjFile_Face & currFace = currGroup.f.back();

    std::wistringstream wideStream(fileContentW);
    std::wstring lineStr;

    // o Car
    // g Body
    // mtllib filename.mtl   # Loads material file
    // usemtl MaterialName   # Assigns material to faces
    // mtllib car_materials.mtl
    // usemtl Paint
    // s 1   # Enable smoothing
    // s off # Disable smoothing
    // f 1 2 3
    // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
    // v x y z [w]   # Vertex position (w is optional)
    // vt u v [w]    # Texture coordinate (w is optional)
    // vn x y z      # Normal vector
    // l v1 v2 v3 ...  # Line element
    // p v1 v2 v3 ...  # Point element

    while (std::getline(wideStream, lineStr))
    {
        std::wistringstream line(lineStr);

        // DE_DEBUG("Read line ", de_mbstr(lineStr))

        // Read 1st word
        std::wstring param0;

        if (line >> param0)
        {
            if (param0.empty())
            {
                continue; // Skip empty line.
            }
            else if (param0.front() == L'#')
            {
                continue; // Skip comment line.
            }
        }
        else
        {
            continue; // Skip empty line.
        }

        dbStrLowerCase( param0 );

        //DE_DEBUG("Read 1st word ", de_mbstr(param0))

        std::wstring param1;

        if (param0 == L"mtllib")
        {
            if (line >> param1)
            {
                //DE_DEBUG("Read 2nd word ", de_mbstr(param1))

                std::string dir_obj = FileSystem::fileDir(uri_obj);

                //DE_DEBUG("Build dir_obj ", dir_obj)

                std::string uri_mtl = dir_obj + "/" + de_mbstr(param1);

                //DE_DEBUG("Build uri_mtl ", uri_mtl)

                objFile.mtllib = MtlLibParser::readFile( de_wstr(uri_mtl) );

                //DE_DEBUG("Parsed mtllib ", uri_mtl)
            }
            else
            {
                DE_ERROR("Fail 2nd word for mtllib ", StringUtil::to_str(param1))
            }
        }
        else if (param0 == L"o")
        {
            line >> param0;
            objFile.o.emplace_back();
            ObjFile_Mesh & mesh = objFile.o.back();
            mesh.name = param0;
            mesh.mtllib = objFile.mtllib.name;
        }
        else if (param0 == L"g")
        {
            line >> param0;
            if (param0 == L"off")
            {
                // Found: g off, which is an unsupported bloat from blender. Skip it
                continue;
            }

            if (objFile.o.empty()) { objFile.o.emplace_back(); }
            ObjFile_Mesh & mesh = objFile.o.back();
            if (mesh.g.empty()) { mesh.g.emplace_back(); }
            ObjFile_Group & group = mesh.g.back();
            group.name = param0;
            //group.usemtl = mesh.usemtl;
        }
        else if (param0 == L"usemtl")
        {
            line >> param0;
            if (objFile.o.empty()) { objFile.o.emplace_back(); }
            ObjFile_Mesh & mesh = objFile.o.back();
            if (mesh.g.empty()) { mesh.g.emplace_back(); }
            ObjFile_Group & group = mesh.g.back();
            group.usemtl = param0;
        }
        else if (param0 == L"f")
        {
            ObjFile_Face objFace = parseStrFace(lineStr);
            if (objFile.o.empty()) { objFile.o.emplace_back(); }
            ObjFile_Mesh & mesh = objFile.o.back();
            if (mesh.g.empty()) { mesh.g.emplace_back(); }
            ObjFile_Group & group = mesh.g.back();
            group.f.emplace_back( std::move(objFace) );
        }
        else if (param0 == L"v")
        {
            ObjFile_Position objPos = parseStrPosition(lineStr);
            objFile.v.emplace_back( std::move(objPos) );
        }
        else if (param0 == L"vn")
        {
            ObjFile_Normal objNormal = parseStrNormal(lineStr);
            objFile.vn.emplace_back( std::move(objNormal) );
        }
        else if (param0 == L"vt")
        {
            ObjFile_TexCoord objTexCoord = parseStrTexCoord(lineStr);
            objFile.vt.emplace_back( std::move(objTexCoord) );
        }
    }

    if (objFile.o.size() > 0)
    {
        if (objFile.o[0].name.empty())
        {
            objFile.o[0].name = FileSystem::fileName( de_wstr(uri_obj) );
        }
    }
    return objFile;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

// static
SMaterial
ObjFile_Converter::convertToSMaterial( const Mtl& mtl, VideoDriver* driver )
{
    SMaterial m;
    m.Lighting = mtl.illum.value_or(0);
    m.Wireframe = false;
    m.FogEnable = false;
    //m.MaterialType = EMT_SOLID;

    if (mtl.Ka)
    {
        // m.AmbientColor = *mtl.Ka;
    }

    if (mtl.map_Kd)
    {
        auto texName = StringUtil::to_str(mtl.map_Kd->name);
        m.DiffuseMap.tex = driver->getTexture( texName );
        if (!m.DiffuseMap.tex)
        {
            m.DiffuseMap.tex = driver->loadTexture2D( texName );
            if (!m.DiffuseMap.tex)
            {
                DE_ERROR("No diffuseMap ",texName)
            }
        }
    }

    if (mtl.map_Bump)
    {
        auto texName = StringUtil::to_str(mtl.map_Bump->name);
        m.BumpMap.tex = driver->getTexture( texName );
        if (!m.BumpMap.tex)
        {
            m.BumpMap.tex = driver->loadTexture2D( texName );
            if (!m.BumpMap.tex)
            {
                DE_ERROR("No bumpMap ",texName)
            }
        }
    }

    return m;
}

// static
PMaterial
ObjFile_Converter::convertToPMaterial( const Mtl& mtl, VideoDriver* driver )
{
    PMaterial m;
    //m.MaterialType = EMT_SOLID;
    m.illum = mtl.illum.value_or(0);
    m.d = mtl.d.value_or(1);
    m.Ni = mtl.Ni.value_or(1);
    m.Ns = mtl.Ns.value_or(1);
    m.Ka = mtl.Ka.value_or(PMaterial::V3(0.1,0.1,0.1));
    m.Ks = mtl.Ks.value_or(PMaterial::V3(1,1,1));
    m.Kd = mtl.Kd.value_or(PMaterial::V3(1,1,1));

    if (mtl.map_Kd)
    {
        auto texName = StringUtil::to_str(mtl.map_Kd->name);
        m.map_Kd.tex = driver->getTexture( texName );
        if (!m.map_Kd.tex)
        {
            m.map_Kd.tex = driver->loadTexture2D( texName );
            if (!m.map_Kd.tex)
            {
                DE_ERROR("No map_Kd ",texName)
            }
        }
    }

    if (mtl.map_Bump)
    {
        auto texName = StringUtil::to_str(mtl.map_Bump->name);
        m.map_Bump.tex = driver->getTexture( texName );
        if (!m.map_Bump.tex)
        {
            m.map_Bump.tex = driver->loadTexture2D( texName );
            if (!m.map_Bump.tex)
            {
                DE_ERROR("No map_Bump ",texName)
            }
        }
    }

    return m;
}


// static
std::vector<PMesh>
ObjFile_Converter::convertToPMesh( const ObjFile & objFile, VideoDriver* driver )
{
    DE_PERF_MARKER
    std::vector<PMesh> pmeshes;

    size_t nMesh = 0;
    size_t nLimb = 0;
    size_t nFace = 0;
    size_t nTris = 0;
    for (const ObjFile_Mesh & objMesh : objFile.o)
    {
        pmeshes.emplace_back();
        PMesh & pmesh = pmeshes.back();
        pmesh.m_name = StringUtil::to_str(objMesh.name);
        //pmesh.m_name = objMesh.mtllib;
        //pmesh.m_name = objMesh.usemtl;

        for (const ObjFile_Group & objGroup : objMesh.g)
        {
            pmesh.m_limbs.emplace_back();
            PMeshBuffer & plimb = pmesh.m_limbs.back();
            plimb.m_name = StringUtil::to_str(objGroup.name);

            // Convert material
            auto objMaterial = objFile.mtllib.getMaterial(objGroup.usemtl);
            plimb.m_material = convertToPMaterial( objMaterial, driver );

            for (const ObjFile_Face & objFace : objGroup.f)
            {
                if (objFace.items.size() < 3)
                {
                    DE_WARN("Mesh[",nMesh,"|",pmesh.m_name,"]."
                            "Limb[",nLimb,"|",plimb.m_name,"]."
                            "Face[",nFace,"] :: Invalid i = ", objFace.items.size())
                }
                else
                {
                    auto toVertex = [&](const ObjFile_Face::Item& item)
                    {
                        PVertex vtx;

                        const auto index_v = std::max( item.v.value_or(1), 1u) - 1;
                        if (index_v < objFile.v.size())
                        {
                            vtx.pos.x = objFile.v[index_v].x;
                            vtx.pos.y = objFile.v[index_v].y;
                            vtx.pos.z = objFile.v[index_v].z;
                        }
                        const auto index_n = std::max( item.vn.value_or(1), 1u) - 1;
                        if (index_n < objFile.vn.size())
                        {
                            vtx.normal.x = objFile.vn[index_n].x;
                            vtx.normal.y = objFile.vn[index_n].y;
                            vtx.normal.z = objFile.vn[index_n].z;
                        }
                        const auto index_t = std::max( item.vt.value_or(1), 1u) - 1;
                        if (index_t < objFile.vt.size())
                        {
                            vtx.tex.x = objFile.vt[index_t].u;
                            vtx.tex.y = objFile.vt[index_t].v;
                            vtx.tex.z = objFile.vt[index_t].w.value_or(0.0);
                        }
                        return vtx;
                    };

                    // Triangle
                    if (objFace.items.size() == 3)
                    {
                        PVertex A = toVertex( objFace.items[0] );
                        PVertex B = toVertex( objFace.items[1] );
                        PVertex C = toVertex( objFace.items[2] );
                        plimb.addVertex( A );
                        plimb.addVertex( B );
                        plimb.addVertex( C );
                    }
                    // Quad
                    else if (objFace.items.size() == 4)
                    {
                        PVertex A = toVertex( objFace.items[0] );
                        PVertex B = toVertex( objFace.items[1] );
                        PVertex C = toVertex( objFace.items[2] );
                        PVertex D = toVertex( objFace.items[3] );
                        plimb.addVertex( A );
                        plimb.addVertex( B );
                        plimb.addVertex( C );
                        plimb.addVertex( A );
                        plimb.addVertex( C );
                        plimb.addVertex( D );
                    }
                    // Polygon?
                    else // if (objFace.items.size() > 4)
                    {
                        DE_ERROR("Unsupported face with ", objFace.items.size(), " items.")
                    }
                }
            }
        }
    }

    for (auto & pmesh : pmeshes)
    {
        for (auto & plimb : pmesh.m_limbs)
        {
            plimb.recalculateBoundingBox();
        }
        pmesh.recalculateBoundingBox();
    }

    /*
    DE_DEBUG(objFile.str())
    DE_WARN("Converted ObjFile to ", pmeshes.size(), " PMeshes.")

    for (size_t i = 0; i < pmeshes.size(); ++i)
    {
        const auto & p = pmeshes[i];
        DE_DEBUG("PMesh[",i,"|", p.m_name, "] bbox = ", p.m_bbox.getSize())

        for (size_t j = 0; j < p.m_limbs.size(); ++j)
        {
            const auto & pm = p.m_limbs[j];
            DE_TRACE("PMesh[",i,"].Limb[",j,"] ", pm.m_name, ", bbox = ", pm.m_bbox.getSize())
        }
    }

    TexRef& ref = mtl.map_Kd->texRef;
    if (!ref.tex)
    {
        const auto& uri = mtl.map_Kd->name;
        auto tex = m_driver->getTexture( uri );
        if (!tex)
        {
            tex = m_driver->loadTexture2D( uri );
        }
        if (!tex)
        {
            DE_ERROR("Can't load diffuseMap ", uri )
        }
        else
        {
            DE_DEBUG("Loaded diffuseMap ", uri )
        }
        ref = TexRef(tex);
    }

    if (ref.tex)
    {
    */

    return pmeshes;
}

// static
std::vector<SMesh>
ObjFile_Converter::convertToSMesh( const ObjFile & file, VideoDriver* driver )
{
    DE_PERF_MARKER

    std::vector<SMesh> smeshes;

    size_t nMesh = 0;
    size_t nLimb = 0;
    size_t nFace = 0;
    size_t nTris = 0;
    for (const ObjFile_Mesh & mesh : file.o)
    {
        smeshes.emplace_back();
        SMesh & smesh = smeshes.back();

        for (const ObjFile_Group & limb : mesh.g)
        {
            smesh.buffers.emplace_back();
            SMeshBuffer & smeshbuffer = smesh.buffers.back();
            for (const ObjFile_Face & face : limb.f)
            {
                if (face.items.size() < 3)
                {
                    DE_WARN("Mesh[",nMesh,"|",StringUtil::to_str(mesh.name),"]"
                            ".Limb[",nLimb,"|",StringUtil::to_str(limb.name),"]"
                            ".Face[",nFace,"] :: Invalid i = ", face.items.size())
                }
                else
                {
                    auto toS3DVertex = [&](const ObjFile_Face::Item& item)
                    {
                        uint32_t iv = item.v.value_or(0);
                        uint32_t in = item.vn.value_or(0);
                        uint32_t it = item.vt.value_or(0);
                        S3DVertex o;
                        o.color = 0xFFFFFFFF;

                        if (iv < file.v.size())
                        {
                            o.pos.x = file.v[iv].x;
                            o.pos.y = file.v[iv].y;
                            o.pos.z = file.v[iv].z;
                        }
                        if (in < file.vn.size())
                        {
                            o.normal.x = file.vn[in].x;
                            o.normal.y = file.vn[in].y;
                            o.normal.z = file.vn[in].z;
                        }
                        if (it < file.vt.size())
                        {
                            o.tex.x = file.vt[it].u;
                            o.tex.y = file.vt[it].v;
                            //o.tex.z = file.vn[in].w.value_or(0.0);
                        }

                        return o;
                    };

                    if (face.items.size() == 3)
                    {
                        smeshbuffer.addVertex( toS3DVertex( face.items[0] ) ); // A
                        smeshbuffer.addVertex( toS3DVertex( face.items[1] ) ); // B
                        smeshbuffer.addVertex( toS3DVertex( face.items[2] ) ); // C
                        nTris++;

                    }
                    else if (face.items.size() == 4)
                    {
                        smeshbuffer.addVertex( toS3DVertex( face.items[0] ) ); // A
                        smeshbuffer.addVertex( toS3DVertex( face.items[1] ) ); // B
                        smeshbuffer.addVertex( toS3DVertex( face.items[2] ) ); // C
                        nTris++;

                        smeshbuffer.addVertex( toS3DVertex( face.items[0] ) ); // A
                        smeshbuffer.addVertex( toS3DVertex( face.items[2] ) ); // B
                        smeshbuffer.addVertex( toS3DVertex( face.items[3] ) ); // C
                        nTris++;
                    }
                }
                nFace++;
            }
            nLimb++;
        }
        nMesh++;
        smesh.recalculateBoundingBox(true);
    }

    DE_WARN("Converted to Obj3D: ", StringUtil::to_str(file.uri))
    DE_DEBUG("size_t nMesh = ",nMesh)
    DE_DEBUG("size_t nLimb = ",nLimb)
    DE_DEBUG("size_t nFace = ",nFace)
    DE_DEBUG("size_t nTris = ",nTris)

    DE_DEBUG("SMeshes = ",smeshes.size())
    int k = 0;
    for ( SMesh & smesh : smeshes )
    {
        DE_DEBUG("SMesh[",k,"] ",smesh.toString())
    }

    /*
    TexRef& ref = mtl.map_Kd->texRef;
    if (!ref.tex)
    {
        const auto& uri = mtl.map_Kd->name;
        auto tex = m_driver->getTexture( uri );
        if (!tex)
        {
            tex = m_driver->loadTexture2D( uri );
        }
        if (!tex)
        {
            DE_ERROR("Can't load diffuseMap ", uri )
        }
        else
        {
            DE_DEBUG("Loaded diffuseMap ", uri )
        }
        ref = TexRef(tex);
    }

    if (ref.tex)
    {
    */

    return smeshes;
}

/*
// **Material Definition**
// list of all known `.mtl` file parameters
- `newmtl [name]` – Defines a new material with the given name.

### **Material Color and Illumination**
- `Ka r g b` – Ambient reflectivity (RGB values between `0.0` and `1.0`).
- `Kd r g b` – Diffuse reflectivity (RGB values between `0.0` and `1.0`).
- `Ks r g b` – Specular reflectivity (RGB values between `0.0` and `1.0`).
- `Tf r g b` – Transmission filter (RGB values between `0.0` and `1.0`).
- `illum [model]` – Illumination model (`0` to `10`, defining shading behavior).
- `d [value]` – Dissolve factor (`0.0` = fully transparent, `1.0` = fully opaque).
- `Ns [value]` – Specular exponent (`0` to `1000`, controls shininess).
- `Ni [value]` – Optical density (`0.001` to `10.0`, affects refraction).
- `sharpness [value]` – Sharpness of reflections (`0` to `1000`).

### **Texture Mapping**
- `map_Ka [file]` – Ambient texture map.
- `map_Kd [file]` – Diffuse texture map.
- `map_Ks [file]` – Specular texture map.
- `map_Ns [file]` – Specular exponent texture map.
- `map_d [file]` – Transparency texture map.
- `bump [file]` – Bump map texture.
- `map_Bump [file]` – Alternative bump map definition.
- `disp [file]` – Displacement map texture.
- `decal [file]` – Decal texture map.
- `refl -type sphere [file]` – Reflection map texture.

### **Optional Texture Arguments**
- `-s x y z` – Texture scaling.
- `-o x y z` – Texture offset.
- `-mm base gain` – Modify texture brightness.
- `-bm value` – Bump map multiplier.

This should cover **all** `.mtl` parameters. Let me know if you need further details!


Of course!
These parameters help control how textures are applied to a material in .mtl files.

// ===========================================================================
Scaling (-s x y z)
// ===========================================================================
This parameter adjusts the size of the texture across the X, Y, and Z axes:

- Values: Each axis (x y z) takes a floating-point value.
- Typical Range: 0.001 (extreme shrink) to 10.0+ (extreme stretch).
- Example Usage:

map_Kd -s 2.0 2.0 1.0 texture.png

- This doubles the texture scale in both X and Y but leaves Z unchanged.

// ===========================================================================
Offset (-o x y z)
// ===========================================================================
This shifts the texture in a specific direction:- Values:
Each axis (x y z) takes a floating-point value, typically in texture coordinate space.
- Typical Range: -1.0 (move left/down/back) to +1.0 (move right/up/forward).
- Example Usage:

map_Kd -o 0.2 0.5 0.0 texture.png

- This moves the texture right (X) by 0.2 and up (Y) by 0.5, leaving Z unchanged.

// ===========================================================================
Modify Map (-mm base gain)
// ===========================================================================
This adjusts the brightness and contrast of the texture:
- base: Controls the minimum brightness (0.0 = darkest, 1.0 = normal).
- gain: Controls the contrast amplification (1.0 = normal, 10.0+ = high contrast).
- Example Usage:

map_Kd -mm 0.5 2.0 texture.png

- This sets the darkest pixel to 0.5 brightness and doubles the contrast.

// ===========================================================================
// ===========================================================================
// ===========================================================================

Would you like to see how these interact together? I can describe combined effects in more depth if needed!

// ===========================================================================
// ===========================================================================
// ===========================================================================


These parameters affect how a texture is mapped onto a surface.
Depending on their values, the texture can be stretched, moved, and brightened/darkened simultaneously.

// ===========================================================================
Example Scenario

Imagine you have a material with this .mtl definition:
// ===========================================================================

newmtl CustomSurface
map_Kd -s 2.0 1.0 1.0 -o 0.5 0.0 0.0 -mm 0.2 1.5 texture.png

// ===========================================================================

This applies the following transformations:

// ===========================================================================

map_Kd
    -s 2.0 1.0 1.0
    -o 0.5 0.0 0.0
    -mm 0.2 1.5
    texture.png

// ===========================================================================

This applies the following transformations:
- Scaling (-s 2.0 1.0 1.0):
    The texture is stretched horizontally (X is doubled),
    keeps original height (Y remains 1.0),
    and Z scale stays unchanged.
- Offset (-o 0.5 0.0 0.0):
    The texture moves halfway to the right (X shifts by 0.5),
    does not shift vertically (Y remains 0.0).
- Brightness modification (-mm 0.2 1.5):
    - The minimum brightness is raised to 0.2 (preventing full darkness).
    - The contrast gain is increased by 1.5x, making bright areas even brighter.

Visual Interpretation

These combined effects would result in:

    - The texture appearing wider than normal due to the X scaling.
    - A horizontal shift, revealing part of the adjacent texture if the material tiles.
    - A brighter texture, with dark areas never going fully black and highlights appearing more pronounced.
Potential Pitfalls & Adjustments
When combining these parameters, you need to be aware of:

    - Scaling too aggressively can cause texture distortion, making fine details stretch unnaturally.
    - Offsets can move the texture out of visible bounds, requiring careful adjustments.
    - Brightness modification (-mm) works best when balanced—too high gain can cause an overexposed look.

Would you like to see an approach for adaptive values based on different material types (e.g., metals vs. cloth)?

// ===========================================================================
// ======== *.mtl File description ===========================================
// ===========================================================================

# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware
# File Created: 10.01.2012 12:09:34
newmtl 12993_diffuse        // Materialnamen:
    Ns 30.0000              // Shininess (Glanzgrad):
    Ni 1.5000               // Optical Density (Brechungsindex):
    Tr 0.0000 | d 1.0000    // Dissolve (Transparenz):
    illum 2
    Ka 1.0000 1.0000 1.0000 // Ambient-Farbe (Umgebungslicht):
    Kd 1.0000 1.0000 1.0000 // Diffuse-Farbe (Streulicht):
    Ks 0.3600 0.3600 0.3600 // Specular-Farbe (Spiegellicht):
    Ke 0.0000 0.0000 0.0000 // Emissive-Farbe (Selbstleuchtung):
    Tf 1.0000 1.0000 1.0000 //   ?
    map_Ka fish.jpg         //	map_Ka moon_diffuse.jpg
    map_Kd fish.jpg         //	map_Kd moon_diffuse.jpg
    map_bump moon_normal.jpg
    bump moon_normal.jpg

usemtl Green
s 1 (Smoothing group)
 - The s command enables or disables smoothing groups, which affect shading and normal calculations.
 - s 1 means smoothing is enabled, causing adjacent faces in the same group to share vertex normals for smooth shading.
 - s off would disable smoothing, making each face flat-shaded.
v 2.980785 -1.000000 0.804910
vt 3.000000 -1.000000 1.000000
vn 2.980785 -1.000000 1.195090
 - vn 0.000000 -1.000000 0.000000
f 32//1 10//2 33//3
 - v/vt/vn → Vertex Index / Texture Coordinate Index / Normal Index
 - v//vn
f 1//4 10//2 2//5
f 31//6 10//2 32//1
f 30//7 10//2 31//6
f 29//8 10//2 30//7
f 28//9 10//2 29//8
f 27//10 10//2 28//9
f 26//11 10//2 27//10
f 25//12 10//2 26//11
Each f command represents a face, and the notation format used here includes:
v/vt/vn → Vertex Index / Texture Coordinate Index / Normal Index
v//vn → Vertex Index / Normal Index (no texture coordinates)
Breaking down:
- f 32//1 10//2 33//3
- Vertex indices: 32, 10, 33 (referencing positions from global v list)
- Normal indices: 1, 2, 3 (referencing normals from global vn list)
- Texture coordinates are missing (hence // notation)
- f 1//4 10//2 2//5
- Vertex indices: 1, 10, 2
- Normal indices: 4, 2, 5
- Texture coordinates omitted
This means these faces will use vertex positions and apply normal vectors for lighting, but lack texture coordinate references.
Would you like a deeper dive into how smoothing groups affect shading calculations in rendering engines?

*/


// ===========================================================================
// ** Definition of (.mtl) material files for (.obj) files. **
// These members are used in MtlObjShader.
/*
// ===========================================================================
struct MtlUtil // list of all known `.mtl` file parameters
// ===========================================================================
{
    static std::string toStrDouble( const std::optional<double> & number );
    static std::string toStrInt( const std::optional<int> & number );
    static std::string toStrColor( const std::optional<glm::dvec3> & opt_color );
    static std::string toStrTexture( const std::optional<MtlTex> & opt_tex );
    static double parseStrDouble( const std::string& lineStr );
    static int parseStrInt( const std::string& lineStr );
    static glm::dvec3 parseStrColor( const std::string& lineStr );
    static MtlTex parseStrTexture( const std::string& lineStr );
};

// static
std::string
MtlUtil::toStrDouble( const std::optional<double> & number )
{
    std::ostringstream o;
    if ( number ) { o << number.value(); } else { o << "nullopt"; }
    return o.str();
}

// static
std::string
MtlUtil::toStrInt( const std::optional<int> & number )
{
    std::ostringstream o;
    if ( number ) { o << number.value(); } else { o << "nullopt"; }
    return o.str();
}

// static
std::string
MtlUtil::toStrColor( const std::optional<glm::dvec3> & opt_color )
{
    std::ostringstream o;
    if ( opt_color )
    {
        const auto & color = *opt_color;
        o << color.x << ", " << color.y << ", " << color.z;
    }
    else
    {
        o << "nullopt";
    }
    return o.str();
}

//static
std::string
MtlUtil::toStrTexture( const std::optional<MtlTex> & opt_tex )
{
    std::ostringstream o;
    if ( opt_tex )
    {
        const auto & tex = *opt_tex;
        o << tex.toString();
    }
    else
    {
        o << "nullopt";
    }
    return o.str();
}

// static
double
MtlUtil::parseStrDouble( const std::string& lineStr )
{
    std::istringstream line(lineStr);
    std::string name; double number = 0.0; // Load
    line >> name; line >> number; // Load
    DE_DEBUG("Parsed Double[", name, "] = ", number, "; ") // Store
    return number;
}

// static
int
MtlUtil::parseStrInt( const std::string& lineStr )
{
    std::istringstream line(lineStr);
    std::string name; int number = 0; // Load
    line >> name; line >> number; // Load
    DE_DEBUG("Parsed Int[", name, "] = ", number, "; ") // Store
    return number;
}

// static
glm::dvec3
MtlUtil::parseStrColor( const std::string& lineStr )
{
    std::istringstream line(lineStr);
    std::string name; double x = 1.0, y = 1.0, z = 1.0; // Load.
    line >> name; line >> x; line >> y; line >> z; // Load.
    auto color = glm::dvec3(x,y,z); // Store.
    DE_DEBUG("Parsed Color[", name, "] = ", color.x, ", ", color.y, ", ", color.z, "; ")
    return color;
}

// static
MtlTex
MtlUtil::parseStrTexture( const std::string& lineStr )
{
    MtlTex tex;
    tex.debugText = lineStr;

    std::istringstream line(lineStr);
    std::string param0;
    line >> param0;

    bool ok = false;
    while (line >> param0)
    {
        if (param0 == "-bm")
        {
            double v = 1.0;
            line >> v;
            tex.bm = v;
            //DE_DEBUG("Parsed -bm = ", tex.bm) // double 'map_Bump' BumpScaleFactor or 'map_Ks' Shininess_strength = 1.5;
        }
        else if (param0 == "-mm")
        {
            double v = 1.0;
            line >> v;
            tex.mm = v;
            //DE_DEBUG("Parsed -mm = ", tex.mm)
        }
        else if (param0 == "-o")
        {
            double x = 0.0; double y = 0.0; double z = 0.0;
            line >> x; line >> y; line >> z;
            auto o = glm::dvec3(x,y,z); // Offset (animated water)
            tex.o = o;
            //DE_DEBUG("Parsed -o = ", o.x, ", ", o.y, ", ", o.z, "; ")
        }
        else if (param0 == "-s")
        {
            double x = 0.0; double y = 0.0; double z = 0.0;
            line >> x; line >> y; line >> z;
            auto s = glm::dvec3(x,y,z); // Scale (repeat)
            tex.s = s;
            //DE_DEBUG("Parsed -s = ", s.x, ", ", s.y, ", ", s.z, "; ")
        }
        else // uri
        {
            tex.name = param0;
        }
    }
    DE_DEBUG("Parsed tex ", tex.toString())
    return tex;
}

bool
writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* matNode ) const
{
tinyxml2::XMLElement* cullNode = doc.NewElement( "culling" );
cullNode->SetAttribute("on", state.culling.isEnabled() );
cullNode->SetAttribute("front", state.culling.isFront() );
cullNode->SetAttribute("back", state.culling.isFront() );
cullNode->SetAttribute("cw", state.culling.isCW() );
matNode->InsertEndChild( cullNode );

tinyxml2::XMLElement* blendNode = doc.NewElement( "blend" );
blendNode->SetAttribute("on", state.blend.enabled );
blendNode->SetAttribute("eq", Blend::getEquationString(state.blend.equation).c_str() );
blendNode->SetAttribute("src_rgb", Blend::getFunctionString(state.blend.src_rgb).c_str() );
blendNode->SetAttribute("src_a", Blend::getFunctionString(state.blend.src_a).c_str() );
blendNode->SetAttribute("dst_rgb", Blend::getFunctionString(state.blend.dst_rgb).c_str() );
blendNode->SetAttribute("dst_a", Blend::getFunctionString(state.blend.dst_a).c_str() );
matNode->InsertEndChild( blendNode );

tinyxml2::XMLElement* depthNode = doc.NewElement( "depth" );
depthNode->SetAttribute("on", state.depth.isEnabled() );
depthNode->SetAttribute("zwrite", state.depth.isZWriteEnabled() );
depthNode->SetAttribute("fn", Depth::getString( state.depth.getFunc() ).c_str() );
matNode->InsertEndChild( depthNode );

tinyxml2::XMLElement* stencilNode = doc.NewElement( "stencil" );
stencilNode->SetAttribute("on", state.stencil.enabled );
stencilNode->SetAttribute("sfail", Stencil::getActionString( state.stencil.sfail ).c_str() );
stencilNode->SetAttribute("zfail", Stencil::getActionString( state.stencil.zfail ).c_str() );
stencilNode->SetAttribute("zpass", Stencil::getActionString( state.stencil.zpass ).c_str() );
stencilNode->SetAttribute("func", Stencil::getFuncString( state.stencil.testFunc ).c_str() );
stencilNode->SetAttribute("refValue", state.stencil.testRefValue );
stencilNode->SetAttribute("testMask", state.stencil.testMask );
stencilNode->SetAttribute("stencilMask", state.stencil.stencilMask );
matNode->InsertEndChild( stencilNode );

tinyxml2::XMLElement* wireNode = doc.NewElement( "wireframe" );
wireNode->SetAttribute("on", Wireframe );
matNode->InsertEndChild( wireNode );

tinyxml2::XMLElement* cloudtNode = doc.NewElement( "cloud" );
cloudtNode->SetAttribute("on", CloudTransparent );
matNode->InsertEndChild( cloudtNode );

tinyxml2::XMLElement* illumNode = doc.NewElement( "illumination" );
illumNode->SetAttribute("v", Lighting );
matNode->InsertEndChild( illumNode );

tinyxml2::XMLElement* fogNode = doc.NewElement( "fog" );
fogNode->SetAttribute("v", FogEnable );
matNode->InsertEndChild( fogNode );

//	Ka 0.5882 0.5882 0.5882
//	Kd 1.0000 1.0000 1.0000
//	Ks 0.3600 0.3600 0.3600
//	Ke 0.0000 0.0000 0.0000

tinyxml2::XMLElement* ambientNode = doc.NewElement( "Ka" );
ambientNode->SetAttribute("r", Ka.r );
ambientNode->SetAttribute("g", Ka.g );
ambientNode->SetAttribute("b", Ka.b );
ambientNode->SetAttribute("a", Ka.a );
matNode->InsertEndChild( ambientNode );

tinyxml2::XMLElement* diffuseNode = doc.NewElement( "Kd" );
diffuseNode->SetAttribute("r", Kd.r );
diffuseNode->SetAttribute("g", Kd.g );
diffuseNode->SetAttribute("b", Kd.b );
diffuseNode->SetAttribute("a", Kd.a );
matNode->InsertEndChild( diffuseNode );

tinyxml2::XMLElement* specularNode = doc.NewElement( "Ks" );
specularNode->SetAttribute("r", Ks.r );
specularNode->SetAttribute("g", Ks.g );
specularNode->SetAttribute("b", Ks.b );
specularNode->SetAttribute("a", Ks.a );
matNode->InsertEndChild( specularNode );

tinyxml2::XMLElement* emissiveNode = doc.NewElement( "Ke" );
emissiveNode->SetAttribute("r", Ke.r );
emissiveNode->SetAttribute("g", Ke.g );
emissiveNode->SetAttribute("b", Ke.b );
emissiveNode->SetAttribute("a", Ke.a );
matNode->InsertEndChild( emissiveNode );

tinyxml2::XMLElement* reflectNode = doc.NewElement( "Kr" );
reflectNode->SetAttribute("r", Kr.r );
reflectNode->SetAttribute("g", Kr.g );
reflectNode->SetAttribute("b", Kr.b );
reflectNode->SetAttribute("a", Kr.a );
matNode->InsertEndChild( reflectNode );

tinyxml2::XMLElement* transparentNode = doc.NewElement( "Kt" );
transparentNode->SetAttribute("r", Kt.r );
transparentNode->SetAttribute("g", Kt.g );
transparentNode->SetAttribute("b", Kt.b );
transparentNode->SetAttribute("a", Kt.a );
matNode->InsertEndChild( transparentNode );

tinyxml2::XMLElement* shinyNode = doc.NewElement( "Ns" );
shinyNode->SetAttribute("v", Shininess );
matNode->InsertEndChild( shinyNode );

tinyxml2::XMLElement* shinysNode = doc.NewElement( "Ni" );
shinysNode->SetAttribute("v", Shininess_strength );
matNode->InsertEndChild( shinysNode );

tinyxml2::XMLElement* reflectivityNode = doc.NewElement( "Tr" );
reflectivityNode->SetAttribute("v", Reflectivity );
matNode->InsertEndChild( reflectivityNode );

//	d 1.0000

//   if ( hasDiffuseMap() )
//   {
//      TexRef const & ref = getDiffuseMap();
//      tinyxml2::XMLElement* texNode = doc.NewElement( "Td" );
//      texNode->SetAttribute("name", ref.tex->getName().c_str() );
//      texNode->SetAttribute("src", ref.tex->getName().c_str() );
//      matNode->InsertEndChild( texNode );
//   }

//   if ( hasBumpMap() ){ s << ", Tb:" << getBumpMap().tex->getName(); }
//   if ( hasNormalMap() ){ s << ", Tn:" << getNormalMap().tex->getName(); }
//   if ( hasSpecularMap() ){ s << ", Ts:" << getSpecularMap().tex->getName(); }
//   if ( hasDetailMap() ){ s << ", Tdet:" << getDetailMap().tex->getName(); }
return true;
}

bool saveXML( std::string uri ) const
{
tinyxml2::XMLDocument doc;
tinyxml2::XMLElement* matNode = doc.NewElement( "SMaterial" );
matNode->SetAttribute( "name", Name.c_str() );
matNode->SetAttribute( "tex-count", int( getTextureCount() ) );
writeXML( doc, matNode );

doc.InsertEndChild( matNode );
tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
if ( err != tinyxml2::XML_SUCCESS )
{
DE_ERROR("Cant save xml ", uri)
return false;
}
return true;
}

bool
loadXML( std::string uri )
{
tinyxml2::XMLDocument doc;
tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
if ( err != tinyxml2::XML_SUCCESS )
{
DE_ERROR("Cant load xml ", uri)
return false;
}

std::string xmlDir = dbGetFileDir( uri );

DE_DEBUG("=========================================" )
DE_DEBUG("loadXml(",uri,")" )
DE_DEBUG("=========================================" )
//std::string baseName = dbGetFileBaseName( uri );
//std::string dirName = dbGetFileDir( uri );
//DE_DEBUG("URI baseName = ", baseName )
//DE_DEBUG("URI dirName = ", dirName )

tinyxml2::XMLElement* atlasNode = doc.FirstChildElement( "image-atlas" );
if ( !atlasNode )
{
DE_ERROR("No image atlas node in xml ", uri)
return false;
}

if ( atlasNode->Attribute( "name" ) )
{
Name = atlasNode->Attribute( "name" );
}
else
{
DE_ERROR("No atlas name found ", uri)
}

int pageCount = atlasNode->IntAttribute( "pages" );

tinyxml2::XMLElement* pageNode = atlasNode->FirstChildElement( "page" );
if ( !pageNode )
{
DE_ERROR("No image atlas pages found in xml ", uri)
return false;
}

// Load Pages:
while ( pageNode )
{
if ( !pageNode->Attribute("src") )
{
DE_ERROR("No page src" )
continue;
}

int pageW = pageNode->IntAttribute("w");
int pageH = pageNode->IntAttribute("h");
pages.emplace_back();
ImageAtlasPage & page = pages.back();
page.pageId = int( pages.size() );
page.name = pageNode->Attribute("src");

std::string pageUri = xmlDir + "/" + page.name;
if ( !dbLoadImage( page.img, pageUri ) )
{
DE_ERROR("No page ", page.pageId," src image uri ", pageUri)
page.img.clear();
}

if ( pageW != page.img.getWidth() )
{
DE_ERROR("Differing src image width ", page.name)
}
if ( pageH != page.img.getHeight() )
{
DE_ERROR("Differing src image height ", page.name)
}

// Load Refs:
int refCount = pageNode->IntAttribute("refs");

// Load Refs:
tinyxml2::XMLElement* refNode = pageNode->FirstChildElement( "img" );
while ( refNode )
{
page.refs.emplace_back();
ImageAtlasRef & ref = page.refs.back();

int refId = refNode->IntAttribute("id");
int refX = refNode->IntAttribute("x");
int refY = refNode->IntAttribute("y");
int refW = refNode->IntAttribute("w");
int refH = refNode->IntAttribute("h");
bool wantMipmaps = refNode->IntAttribute("mipmaps") != 0 ? true:false;
std::string name = refNode->Attribute("name" );
ref.id = refId;
ref.name = name;
ref.img = &page.img;
ref.page = page.pageId;
ref.mipmaps = wantMipmaps;
ref.rect = Recti( refX, refY, refW, refH );

//DE_DEBUG("Add ref ", ref.toString() )

refNode = refNode->NextSiblingElement( "img" );
}

pageNode = pageNode->NextSiblingElement( "page" );
}

DE_DEBUG("Loaded atlas xml ", uri)

//   DE_DEBUG("PageCount = ", pages.size())
//   for ( int i = 0; i < pages.size(); ++i )
//   {
//      ImageAtlasPage & page = pages[ i ];
//      DE_DEBUG("Page[",i,"].RefCount = ", page.refs.size())
//      for ( int r = 0; r < page.refs.size(); ++r )
//      {
//         ImageAtlasRef & ref = page.refs[ r ];
//         DE_DEBUG("Page[",i,"].Ref[",r,"].Rect = ", ref.rect.toString() )
//      }
//   }
return true;
}

uint32_t getTextureCount() const;
bool hasTexture() const;
TexRef const & getTexture( int stage ) const;
TexRef & getTexture( int stage );
void setTexture( int stage, TexRef const & ref );

std::vector< TexRef > getTextures() const
{
std::vector< TexRef > textures;
if ( !DiffuseMap.empty() ) textures.emplace_back( DiffuseMap );
if ( !BumpMap.empty() ) textures.emplace_back( BumpMap );
if ( !NormalMap.empty() ) textures.emplace_back( NormalMap );
if ( !SpecularMap.empty() ) textures.emplace_back( SpecularMap );
if ( !DetailMap.empty() ) textures.emplace_back( DetailMap );
return textures;
}

bool hasDiffuseMap() const { return !DiffuseMap.empty(); }
bool hasBumpMap() const { return !BumpMap.empty(); }
bool hasNormalMap() const { return !NormalMap.empty(); }
bool hasSpecularMap() const { return !SpecularMap.empty(); }
bool hasDetailMap() const { return !DetailMap.empty(); }
TexRef const & getDiffuseMap() const { return DiffuseMap; }
TexRef const & getBumpMap() const { return BumpMap; }
TexRef const & getNormalMap() const { return NormalMap; }
TexRef const & getSpecularMap() const { return SpecularMap; }
TexRef const & getDetailMap() const { return DetailMap; }
TexRef & getDiffuseMap() { return DiffuseMap; }
TexRef & getBumpMap() { return BumpMap; }
TexRef & getNormalMap() { return NormalMap; }
TexRef & getSpecularMap() { return SpecularMap; }
TexRef & getDetailMap() { return DetailMap; }

bool
operator==( Mtl const & o ) const
{
// if ( state != o.state ) return false;
// if ( fog != o.fog ) return false;
// if ( wireframe != o.wireframe ) return false;
// if ( cloud_transparent != o.cloud_transparent ) return false;
if ( illum != o.illum ) return false;
if ( m_DiffuseMap.tex != o.m_DiffuseMap.tex ) return false;
if ( m_BumpMap.tex != o.m_BumpMap.tex ) return false;
if ( m_NormalMap.tex != o.m_NormalMap.tex ) return false;
if ( m_SpecularMap.tex != o.m_SpecularMap.tex ) return false;
if ( m_DetailMap.tex != o.m_DetailMap.tex ) return false;
return true;
}

bool
operator!= ( Mtl const & o ) const { return !( o == *this ); }

uint32_t
getTextureCount() const
{
uint32_t n = 0;
n += DiffuseMap.empty() ? 0 : 1;
n += BumpMap.empty() ? 0 : 1;
//n += NormalMap.empty() ? 0 : 1;
n += SpecularMap.empty() ? 0 : 1;
n += DetailMap.empty() ? 0 : 1;
return n;
}

bool
hasTexture() const { return getTextureCount() > 0; }

TexRef const &
getTexture( int stage ) const
{
if ( stage == 0 ) { return DiffuseMap; }
else if ( stage == 1 ) { return BumpMap; }
else if ( stage == 2 ) { return NormalMap; }
else if ( stage == 3 ) { return SpecularMap; }
else if ( stage == 4 ) { return DetailMap; }
else
{
//DE_RUNTIME_ERROR( std::to_string( stage ), "Invalid tex stage" )
DE_ERROR( "SMaterial(",Name,") :: Bad stage index " + std::to_string( stage ) )
return DiffuseMap;
}
}

TexRef &
getTexture( int stage )
{
if ( stage == 0 ) { return DiffuseMap; }
else if ( stage == 1 ) { return BumpMap; }
else if ( stage == 2 ) { return NormalMap; }
else if ( stage == 3 ) { return SpecularMap; }
else if ( stage == 4 ) { return DetailMap; }
else
{
//DE_RUNTIME_ERROR( std::to_string( stage ), "Invalid tex stage" )
DE_ERROR( "SMaterial(",Name,") :: Bad stage index " + std::to_string( stage ) )
return DiffuseMap;
}
}

void
setTexture( int stage, TexRef const & ref )
{
if ( stage == 0 ) { DiffuseMap = ref; }
else if ( stage == 1 ) { BumpMap = ref; }
else if ( stage == 2 ) { NormalMap = ref; }
else if ( stage == 3 ) { SpecularMap = ref; }
else if ( stage == 4 ) { DetailMap = ref; }
else
{
//throw std::runtime_error( "Invalid tex stage " + std::to_string( stage ) );
DE_ERROR( "SMaterial(",Name,") :: Bad stage index " + std::to_string( stage ) )
}
}
*/
#if 0

// static
std::optional<Obj3D>
ObjFile_Converter::convert( const ObjFile & file, VideoDriver* driver )
{
    DE_PERF_MARKER

    Obj3D obj;

    size_t nMesh = 0;
    size_t nLimb = 0;
    size_t nFace = 0;
    size_t nTris = 0;
    for (const ObjFile_Mesh & mesh : file.o)
    {
        obj.m_meshes.emplace_back();
        Obj3D_Mesh & dstMesh = obj.m_meshes.back();

        for (const ObjFile_Group & limb : mesh.g)
        {
            dstMesh.m_limbs.emplace_back();
            Obj3D_Limb & dstLimb = dstMesh.m_limbs.back();
            for (const ObjFile_Face & face : limb.f)
            {
                if (face.items.size() < 3)
                {
                    DE_WARN("Mesh[",nMesh,"|",StringUtil::to_str(mesh.name),"]"
                        ".Limb[",nLimb,"|",StringUtil::to_str(limb.name),"]"
                        ".Face[",nFace,"] :: Invalid i = ", face.items.size())
                }
                else
                {
                    auto toVertex = [&](const ObjFile_Face::Item& item)
                    {
                        uint32_t iv = item.v.value_or(0);
                        uint32_t in = item.vn.value_or(0);
                        uint32_t it = item.vt.value_or(0);
                        Obj3D_Vertex o;

                        if (iv < file.v.size())
                        {
                            o.pos.x = file.v[iv].x;
                            o.pos.y = file.v[iv].y;
                            o.pos.z = file.v[iv].z;
                        }
                        if (in < file.vn.size())
                        {
                            o.normal.x = file.vn[in].x;
                            o.normal.y = file.vn[in].y;
                            o.normal.z = file.vn[in].z;
                        }
                        if (it < file.vt.size())
                        {
                            o.tex.x = file.vt[in].u;
                            o.tex.y = file.vt[in].v;
                            //o.tex.z = file.vn[in].w.value_or(0.0);
                        }

                        return o;
                    };

                    // dstLimb.addTriangle(A,B,C);
                    Obj3D_Vertex A = toVertex( face.items[0] );
                    Obj3D_Vertex B = toVertex( face.items[1] );
                    Obj3D_Vertex C = toVertex( face.items[2] );
                    dstLimb.m_bbox.addInternalPoint(A.pos);
                    dstLimb.m_bbox.addInternalPoint(B.pos);
                    dstLimb.m_bbox.addInternalPoint(C.pos);
                    dstLimb.m_vertices.emplace_back( std::move(A) );
                    dstLimb.m_vertices.emplace_back( std::move(B) );
                    dstLimb.m_vertices.emplace_back( std::move(C) );
                    nTris++;
                }
                dstMesh.m_bbox.addInternalBox( dstLimb.m_bbox );
                nFace++;
            }
            obj.m_bbox.addInternalBox( dstMesh.m_bbox );
            nLimb++;
        }
        nMesh++;
    }

    DE_WARN("Converted to Obj3D: ", StringUtil::to_str(file.name))
    DE_DEBUG("size_t nMesh = ",nMesh)
    DE_DEBUG("size_t nLimb = ",nLimb)
    DE_DEBUG("size_t nFace = ",nFace)
    DE_DEBUG("size_t nTris = ",nTris)
    DE_DEBUG("Box3d m_bbox = ",obj.m_bbox.toString())

    /*
    TexRef& ref = mtl.map_Kd->texRef;
    if (!ref.tex)
    {
        const auto& uri = mtl.map_Kd->name;
        auto tex = m_driver->getTexture( uri );
        if (!tex)
        {
            tex = m_driver->loadTexture2D( uri );
        }
        if (!tex)
        {
            DE_ERROR("Can't load diffuseMap ", uri )
        }
        else
        {
            DE_DEBUG("Loaded diffuseMap ", uri )
        }
        ref = TexRef(tex);
    }

    if (ref.tex)
    {
    */

    return obj;
}

#endif
// ===========================================================================
// ===========================================================================
// ===========================================================================


} // namespace mtl.
} // namespace gpu.
} // namespace de.
