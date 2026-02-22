#pragma once
#include <DarkImage.h>
#include <tiny_obj_loader.h>
#include <string>
#include <optional>
#include <vector>

#include <de/gpu/smesh/SMesh.h>

namespace de {

    // struct SMeshBufferReaderObj
    // {
    //     static bool load(const std::string& uri, gpu::SMeshBuffer & obj, const std::string& objName);
    // };

    struct SMeshBufferWriterObj
    {
        static bool save(const std::string& uri, const gpu::SMeshBuffer & obj, bool bVerbose = false );
    };

    struct ObjFace
    {
        // Available Data       Face Syntax Example
        // Face-element:        f v/vt/vn
        // Only vertex          f 1 2 3
        // Vertex + texcoord	f 1/1 2/2 3/3
        // Vertex + normal      f 1//1 2//2 3//3
        // All three            f 1/1/1 2/2/2 3/3/3

        std::vector<tinyobj::index_t> elements;

        std::string str() const;

        static std::string str( const tinyobj::index_t& e );
    };

    struct Obj
    {
        typedef glm::vec2 V2;
        typedef glm::vec3 V3;
        std::string name;
        std::vector<V3> v;   // vertices: x, y, z
        std::vector<V3> vn;  // normals:  x, y, z
        std::vector<V2> vt;  // texcoords:   u, v
        std::vector<ObjFace> faces; // Preserves face structure

        int32_t addPos( const V3& p );
        int32_t addNormal( const V3& n );
        int32_t addTexCoord( const V2& t );
    };

    struct ObjUtil
    {
        static bool test()
        {
            // enumerate("media/Kinescope/TV.obj");

            auto o = load("media/Kinescope/TV.obj", "ob.kinescope");
            if (o)
            {
                return save(*o, "kinescope.obj");
            }
            else
            {
                return false;
            }
        }

        static bool
        enumerate(const std::string& uri);

        static std::optional<tinyobj::shape_t>
        find(const std::string& uri, const std::string& objName);

        static std::optional<Obj>
        load(const std::string& uri, const std::string& objName);

        static bool
        save(const Obj& obj, const std::string& uri);

        static bool
        toSMeshBuffer( const Obj& obj, gpu::SMeshBuffer & out);
    };

} // end namespace de.
