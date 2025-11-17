#include "EnumerateObjFile.h"

namespace de {

bool
ObjUtil::toSMeshBuffer( const Obj& obj, gpu::SMeshBuffer & mesh)
{
    mesh.name = obj.name;
    mesh.vertices.clear();
    mesh.indices.clear();

    const auto& p = obj.v;
    const auto& n = obj.vn;
    const auto& t = obj.vt;

    if (p.size() != n.size())
    {
        DE_ERROR("Unsupported p.size(",p.size(),") != n.size(",n.size(),")")
        return false;
    }

    if (p.size() != t.size())
    {
        DE_ERROR("Unsupported p.size(",p.size(),") != t.size(",t.size(),")")
        return false;
    }

    for (size_t i = 0; i < p.size(); ++i)
    {
        gpu::S3DVertex v;
        v.pos = p[i];
        v.normal = n[i];
        v.color = 0xFFFFFFFF;
        v.tex = t[i];
        mesh.vertices.emplace_back( std::move(v) );
    }

    for (const auto& face : obj.faces)
    {
        for (const auto& elem : face.elements)
        {
            mesh.indices.emplace_back( elem.vertex_index );
        }
    }

#if 0
    for (const auto& face : obj.faces)
    {
        for (const auto& elem : face.elements)
        {
            gpu::S3DVertex v;
            v.color = 0xFFFFFFFF;

            if ( elem.vertex_index > -1 )
            {
                v.pos = obj.v[ elem.vertex_index ];
            }
            if ( elem.normal_index > -1 )
            {
                v.normal = obj.vn[ elem.normal_index ];
            }
            if ( elem.texcoord_index > -1 )
            {
                v.tex = obj.vt[ elem.texcoord_index ];
            }

            auto it = std::find_if(mesh.vertices.cbegin(), mesh.vertices.cend(),
                                   [&] (const gpu::S3DVertex& cached) { return cached == v; });
            if (it == mesh.vertices.cend())
            {
                mesh.vertices.emplace_back( std::move(v) );
            }
            mesh.indices.emplace_back( elem.vertex_index );
        }
    }
#endif
    gpu::SMeshBufferTool::computeBoundingBox(mesh);
    DE_DEBUG("BBox = ", mesh.getBoundingBox().str())
    gpu::SMeshBufferTool::centerVertices(mesh);
    DE_DEBUG("BBox = ", mesh.getBoundingBox().str())
    return true;
}

/*
// static
bool
SMeshBufferReaderObj::load(const std::string& uri, gpu::SMeshBuffer & mesh, const std::string& objName)
{
    auto opt_obj = ObjUtil::load(uri, objName);
    if (!opt_obj)
    {
        DE_ERROR("No objName ", objName, " in file ", uri)
        return false;
    }

    const Obj& obj = *opt_obj;

    mesh.name = objName;
    mesh.vertices.clear();
    mesh.indices.clear();

    for (const auto& face : obj.faces)
    {
        for (const auto& elem : face.elements)
        {
            gpu::S3DVertex v;
            v.color = 0xFFFFFFFF;

            if ( elem.vertex_index > -1 )
            {
                v.pos = obj.v[ elem.vertex_index ];
            }
            if ( elem.normal_index > -1 )
            {
                v.normal = obj.vn[ elem.normal_index ];
            }
            if ( elem.texcoord_index > -1 )
            {
                v.tex = obj.vt[ elem.texcoord_index ];
            }

            auto it = std::find_if(mesh.vertices.cbegin(), mesh.vertices.cend(),
                                   [&] (const gpu::S3DVertex& cached) { return cached == v; });
            if (it == mesh.vertices.cend())
            {
                mesh.vertices.emplace_back( std::move(v) );
            }
            mesh.indices.emplace_back( elem.vertex_index );
        }
    }

    gpu::SMeshBufferTool::computeBoundingBox(mesh);
    DE_DEBUG("BBox = ", mesh.getBoundingBox().str())
    gpu::SMeshBufferTool::centerVertices(mesh);
    DE_DEBUG("BBox = ", mesh.getBoundingBox().str())
    return true;
}
*/

// static
bool
SMeshBufferWriterObj::save(const std::string& uri, const gpu::SMeshBuffer & obj, bool bVerbose )
{
    std::ostringstream o;
    o << "# Exported OBJ: " << obj.name << "\n";
    o << "# Num vertices: " << obj.vertices.size() << "\n";
    o << "# Num indices: " << obj.indices.size() << "\n";
    o << "# BoundingBox: " << obj.boundingBox.str() << "\n";
    o << "o " << obj.name << "\n";

    size_t i;

    // Write positions (and colors)
    o << "# Num positions (and colors): " << obj.vertices.size() << "\n";
    i = 0;
    for (const auto& v : obj.vertices)
    {
        if (bVerbose)
        {
            o << "# v[" << (i+1) << "/" << obj.vertices.size() << "]:\n";
            i++;
        }
        o << "v " << v.pos.x << " " << v.pos.y << " " << v.pos.z;
        if (v.color != 0xFFFFFFFF)
        {
            const float r = float(dbRGBA_R(v.color)) / 255.0f;
            const float g = float(dbRGBA_G(v.color)) / 255.0f;
            const float b = float(dbRGBA_B(v.color)) / 255.0f;
            const float a = float(dbRGBA_A(v.color)) / 255.0f;
            o << " " << r << " " << g << " " << b << " " << a;
        }
        o << "\n";
    }

    // Write normals
    o << "# Num normals: " << obj.vertices.size() << "\n";
    i = 0;
    for (const auto& v : obj.vertices)
    {
        if (bVerbose)
        {
            o << "# vn[" << (i+1) << "/" << obj.vertices.size() << "]:\n";
            i++;
        }
        o << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
    }

    // Write texture coordinates
    o << "# Num texcoords: " << obj.vertices.size() << "\n";
    i = 0;
    for (const auto& v : obj.vertices)
    {
        if (bVerbose)
        {
            o << "# vt[" << (i+1) << "/" << obj.vertices.size() << "]:\n";
            i++;
        }
        o << "vt " << v.tex.x << " " << v.tex.y << "\n";
    }

    // Write faces:
    // ObjFace face;

    // Available Data       Face Syntax Example
    // Face-element:        f v/vt/vn
    // Only vertex          f 1 2 3
    // Vertex + texcoord	f 1/1 2/2 3/3
    // Vertex + normal      f 1//1 2//2 3//3
    // All three            f 1/1/1 2/2/2 3/3/3

    if (obj.primitiveType == gpu::PrimitiveType::Triangles)
    {
        size_t numTriangles = obj.indices.size() / 3;
        o << "# Num triangle faces: " << numTriangles << "\n";
        for (size_t k = 0; k < numTriangles; k++)
        {
            size_t i = size_t(3)*k;
            int32_t a = 1 + obj.indices[i + 0];
            int32_t b = 1 + obj.indices[i + 1];
            int32_t c = 1 + obj.indices[i + 2];
            if (bVerbose)
            {
                o << "# f triangle[" << (i+1) << "/" << obj.indices.size()/3 << "]:\n";
            }
            o << "f";
            o << " " << a << "/" << a << "/" << a;
            o << " " << b << "/" << b << "/" << b;
            o << " " << c << "/" << c << "/" << c;
            o << "\n";
        }
    }
    else
    {
        DE_ERROR("Unsupported PrimitiveType ", obj.primitiveType.str())
    }

    return FileSystem::saveStr(uri,o.str());
}

// static
bool
ObjUtil::enumerate(const std::string& uri)
{
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(uri))
    {
        DE_ERROR("No .obj file ",uri, " e:", reader.Error())
        return false;
    }

    if (!reader.Warning().empty())
    {
        DE_WARN(reader.Warning())
    }

    const auto& shapes = reader.GetShapes();
    const std::string target_object_name = "desired_object_name"; // Replace with your 'o' name

    for (size_t i = 0; i < shapes.size(); ++i)
    {
        const auto& shape = shapes[i];

        DE_OK("o[",i,"].name = ", shape.name)
    }
    return true;
}

// static
std::optional<tinyobj::shape_t>
ObjUtil::find(const std::string& uri, const std::string& objName)
{
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(uri))
    {
        DE_ERROR("No .obj file ",uri, " e:", reader.Error())
        return std::nullopt;
    }

    if (!reader.Warning().empty())
    {
        DE_WARN(reader.Warning())
    }

    const auto& shapes = reader.GetShapes();
    for (const auto& shape : shapes)
    {
        if (shape.name == objName)
        {
            return shape; // Found the matching object
        }
    }

    DE_ERROR("No objName '", objName, "' found in file ",uri,".")
    return std::nullopt;
}

// Available Data       Face Syntax Example
// Face-element:        f v/vt/vn
// Only vertex          f 1 2 3
// Vertex + texcoord	f 1/1 2/2 3/3
// Vertex + normal      f 1//1 2//2 3//3
// All three            f 1/1/1 2/2/2 3/3/3

// static
std::string
ObjFace::str( const tinyobj::index_t& e )
{
    std::ostringstream o;

    o << " " << e.vertex_index + 1;

    if ((e.texcoord_index > -1) && (e.normal_index > -1))
    {
        o << "/" << e.texcoord_index + 1;
        o << "/" << e.normal_index + 1;
    }
    else
    {
        if (e.texcoord_index > -1)
        {
            o << "/" << e.texcoord_index + 1;
        }
        else if (e.normal_index > -1)
        {
            o << "//" << e.normal_index + 1;
        }
    }
    return o.str();
}

std::string
ObjFace::str() const
{
    // Available Data       Face Syntax Example
    // Face-element:        f v/vt/vn
    // Only vertex          f 1 2 3
    // Vertex + texcoord	f 1/1 2/2 3/3
    // Vertex + normal      f 1//1 2//2 3//3
    // All three            f 1/1/1 2/2/2 3/3/3

    std::ostringstream o;

    o << "f";
    for (const auto& e : elements)
    {
        o << str(e);
    }
    return o.str();
}

int32_t
Obj::addPos( const V3& p )
{
    auto it = std::find_if( v.cbegin(), v.cend(),
                           [&](const V3& cached ) { return cached == p; });
    if (it == v.cend())
    {
        v.push_back(p);
        return int32_t(v.size()) - 1;
    }
    else
    {
        return int32_t(std::distance(v.cbegin(),it));
    }
}
int32_t
Obj::addNormal( const V3& n )
{
    auto it = std::find_if( vn.cbegin(), vn.cend(),
                           [&](const V3& cached ) { return cached == n; });
    if (it == vn.cend())
    {
        vn.push_back(n);
        return int32_t(vn.size()) - 1;
    }
    else
    {
        return int32_t(std::distance(vn.cbegin(),it));
    }
}
int32_t
Obj::addTexCoord( const V2& t )
{
    auto it = std::find_if( vt.cbegin(), vt.cend(),
                           [&](const V2& cached ) { return cached == t; });
    if (it == vt.cend())
    {
        vt.push_back(t);
        return int32_t(vt.size()) - 1;
    }
    else
    {
        return int32_t(std::distance(vt.cbegin(),it));
    }
}

// static
std::optional<Obj>
ObjUtil::load(const std::string& uri, const std::string& objName)
{
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(uri))
    {
        DE_ERROR("No .obj file ",uri, " e:", reader.Error())
        return std::nullopt;
    }

    if (!reader.Warning().empty())
    {
        DE_WARN(reader.Warning())
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    for (const auto& shape : shapes)
    {
        if (shape.name != objName)
        {
            continue;
        }

        Obj obj;
        obj.name = shape.name;

        size_t k = 0;
        for (uint32_t n : shape.mesh.num_face_vertices) // nFaceElements
        {
            // New face, aka a vector of face-elements
            obj.faces.emplace_back();
            ObjFace & oFace = obj.faces.back();
            oFace.elements.reserve( n );

            // Add/Loop face-elements, keep original indices for now
            // don't rebase yet, we don't have the vertices yet.
            for (uint32_t i = 0; i < n; ++i)
            {
                // Get face-element
                const tinyobj::index_t& e = shape.mesh.indices.at( k++ );

                //if (k<100) DE_OK("InputIndex[",k,"] = ", Face::str(e))
                tinyobj::index_t f;
                f.vertex_index = -1;
                f.normal_index = -1;
                f.texcoord_index = -1;

                if (e.vertex_index > -1)
                {
                    float x = attrib.vertices.at((3 * e.vertex_index) + 0);
                    float y = attrib.vertices.at((3 * e.vertex_index) + 1);
                    float z = attrib.vertices.at((3 * e.vertex_index) + 2);
                    f.vertex_index = obj.addPos( glm::vec3(x,y,z) );
                }
                if (e.normal_index > -1)
                {
                    float nx = attrib.normals.at((3 * e.normal_index) + 0);
                    float ny = attrib.normals.at((3 * e.normal_index) + 1);
                    float nz = attrib.normals.at((3 * e.normal_index) + 2);
                    f.normal_index = obj.addNormal( glm::vec3(nx,ny,nz) );
                }
                if (e.texcoord_index > -1)
                {
                    float u = attrib.texcoords.at((2 * e.texcoord_index) + 0);
                    float v = attrib.texcoords.at((2 * e.texcoord_index) + 1);
                    f.texcoord_index = obj.addTexCoord( glm::vec2(u,v) );
                }

                oFace.elements.push_back(f);
                //if (k<100) DE_OK("OutputIndex[",k,"] = ", Face::str(oFace.elements.back()))
            }
        }
        return obj;
    }

    return std::nullopt;
}

// static
bool
ObjUtil::save(const Obj& obj, const std::string& uri)
{
    // DE_DEBUG("Copied faces = ", obj.faces.size())
    // for (size_t d = 0; d < obj.faces.size(); ++d)
    // {
    //     DE_BENNI("Copied face[",d,"] = ", obj.faces[d].str())
    // }
    DE_BENNI("Copied faces = ", obj.faces.size())

    std::ostringstream o;
    o << "# Exported OBJ: " << obj.name << "\n";
    o << "o " << obj.name << "\n";

    // Write vertices
    o << "# Num positions: " << obj.v.size() << "\n";
    for (const auto& v : obj.v)
    {
        o << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    // Write normals
    o << "# Num normals: " << obj.vn.size() << "\n";
    for (const auto& vn : obj.vn)
    {
        o << "vn " << vn.x << " " << vn.y << " " << vn.z << "\n";
    }

    // Write texture coordinates
    o << "# Num texcoords: " << obj.vt.size() << "\n";
    for (const auto& vt : obj.vt)
    {
        o << "vt " << vt.x << " " << vt.y << "\n";
    }

    // Write faces
    o << "# Num faces: " << obj.faces.size() << "\n";
    for (const auto& face : obj.faces)
    {
        // Available Data       Face Syntax Example
        // Face-element:        f v/vt/vn
        // Only vertex          f 1 2 3
        // Vertex + texcoord	f 1/1 2/2 3/3
        // Vertex + normal      f 1//1 2//2 3//3
        // All three            f 1/1/1 2/2/2 3/3/3

        o << "f";
        for (const auto& elem : face.elements)
        {
            o << " " << elem.vertex_index + 1;

            if ((elem.texcoord_index > -1) && (elem.normal_index > -1))
            {
                o << "/" << elem.texcoord_index + 1;
                o << "/" << elem.normal_index + 1;
            }
            else
            {
                if (elem.texcoord_index > -1)
                {
                    o << "/" << elem.texcoord_index + 1;
                }
                else if (elem.normal_index > -1)
                {
                    o << "//" << elem.normal_index + 1;
                }
            }
        }
        o << "\n";
    }

    return FileSystem::saveStr(uri,o.str());
}

} // end namespace de.

