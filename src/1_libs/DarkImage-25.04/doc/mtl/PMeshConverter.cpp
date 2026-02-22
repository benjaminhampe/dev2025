#include <de/gpu/mtl/PMeshConverter.h>
// #include <de/gpu/VideoDriver.h>
// #include <de_opengl.h>

namespace de {
namespace gpu {

//static
// ===========================================================================
bool PMeshConverter::toSMesh(SMesh & smesh, const PMesh& pmesh)
// ===========================================================================
{
    smesh.boundingBox = pmesh.m_bbox;
    smesh.name = pmesh.m_name;
    //smesh.m_trs = pmesh.m_trs;

    for (const auto & pmeshbuffer : pmesh.m_limbs)
    {
        smesh.buffers.emplace_back();
        SMeshBuffer & smeshbuffer = smesh.buffers.emplace_back();
        smeshbuffer.primitiveType = pmeshbuffer.m_primType;
        smeshbuffer.boundingBox = pmeshbuffer.m_bbox;

        SMaterial & smaterial = smeshbuffer.material;
        const PMaterial & pmaterial = pmeshbuffer.m_material;

        smaterial.DiffuseMap = TexRef(pmaterial.map_Kd.tex);
        smaterial.DiffuseMap.coords.x = pmaterial.map_Kd.o.x;
        smaterial.DiffuseMap.coords.y = pmaterial.map_Kd.o.y;
        smaterial.DiffuseMap.coords.z = pmaterial.map_Kd.s.x;
        smaterial.DiffuseMap.coords.w = pmaterial.map_Kd.s.y;
        smaterial.DiffuseMap.repeat.x = pmaterial.map_Kd.r.x;
        smaterial.DiffuseMap.repeat.y = pmaterial.map_Kd.r.y;
        smaterial.DiffuseMap.gamma = pmaterial.map_Kd.gamma;

        smaterial.BumpMap = TexRef(pmaterial.map_Bump.tex);
        smaterial.BumpMap.coords.x = pmaterial.map_Bump.o.x;
        smaterial.BumpMap.coords.y = pmaterial.map_Bump.o.y;
        smaterial.BumpMap.coords.z = pmaterial.map_Bump.s.x;
        smaterial.BumpMap.coords.w = pmaterial.map_Bump.s.y;
        smaterial.BumpMap.repeat.x = pmaterial.map_Bump.r.x;
        smaterial.BumpMap.repeat.y = pmaterial.map_Bump.r.y;
        smaterial.BumpMap.gamma = pmaterial.map_Bump.gamma;

        smaterial.BumpScale = pmaterial.map_Bump.bumpScale;

        smaterial.Lighting = 1; // pmaterial.illum;
        smaterial.Kd = glm::vec4(pmaterial.Kd,1);
        smaterial.Ka = glm::vec4(pmaterial.Ka,1);
        smaterial.Ks = glm::vec4(pmaterial.Ks,1);
        smaterial.Ke = glm::vec4(pmaterial.Ke,1);
        smaterial.Kr = glm::vec4(pmaterial.Kr,1);
        smaterial.Kt = glm::vec4(pmaterial.Kt,1);

        smaterial.Opacity = pmaterial.d; // d in MTL?
        smaterial.Reflectivity = 0.0f; // pmaterial.; // Tr
        smaterial.Shininess = pmaterial.Ns; // Ns
        smaterial.Shininess_strength = pmaterial.shininess_strength; // Ni
        //smaterial.Shininess_strength = pmaterial.Ni;
        smaterial.state = pmaterial.state;

        smeshbuffer.vertices.reserve( pmeshbuffer.m_vertices.size() );
        for (const auto & pvertex : pmeshbuffer.m_vertices)
        {
            S3DVertex svertex;
            svertex.pos = pvertex.pos;
            svertex.pos.y = -svertex.pos.y;
            svertex.normal = pvertex.normal;
            //svertex.normal.y = -svertex.normal.y;
            svertex.color = pvertex.color;
            svertex.tex = pvertex.tex;
            smeshbuffer.vertices.emplace_back( std::move( svertex ) );
        }

        smeshbuffer.indices.reserve( pmeshbuffer.m_indices.size() );
        for (const auto & pindex : pmeshbuffer.m_indices)
        {
            smeshbuffer.indices.push_back( pindex );
        }

        smeshbuffer.material.state.culling = Culling::disabled();
    }

    return true;
}

} // namespace gpu.
} // namespace de.


