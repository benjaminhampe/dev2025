#include <de/gpu/mtl/PMesh.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {


// ===========================================================================

PMaterialTex::PMaterialTex() : tex(nullptr), gamma(1), bumpScale(1)
    , o(0,0,0), r(1,1), s(1,1,1)
{}

PMaterialTex::PMaterialTex( Texture* tex_ ) : tex(tex_), gamma(1), bumpScale(1)
    , o(0,0,0), r(1,1), s(1,1,1)
{}

PMaterialTex::V4
PMaterialTex::coords() const { return V4(o.x,o.y,s.x,s.y); }
PMaterialTex::V2
PMaterialTex::repeat() const { return V2(r.x,r.y); }
void
PMaterialTex::setTex( Texture* tex_ ) { tex = tex_; } //o = (0,0,0), r(1,1), scale(1,1,1)

std::string
PMaterialTex::str() const
{
    std::ostringstream k;
    /*
        k << StringUtil::to_str(name);
        if ( bm ) { k << ", bm(" << bm.value() << ")"; }
        if ( mm ) { k << ", mm(" << mm.value() << ")"; }
        if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
        if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
        */
    return k.str();
}

std::wstring
PMaterialTex::wstr() const
{
    std::wostringstream k;
    /*
        k << name;
        if ( bm ) { k << ", bm(" << bm.value() << ")"; }
        if ( mm ) { k << ", mm(" << mm.value() << ")"; }
        if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
        if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
        */
    return k.str();
}

// ===========================================================================
// struct PMaterialFog
// ===========================================================================

std::string PMaterialFog::str() const
{
    std::ostringstream k;
    /*
    k << StringUtil::to_str(name);
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    */
    return k.str();
}

// ===========================================================================

std::wstring PMaterialFog::wstr() const
{
    std::wostringstream k;
    /*
    k << name;
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    */
    return k.str();
}

// ===========================================================================
// ** Definition of (.mtl) material files for (.obj) files. **
// These members are used in MtlObjShader.
// ===========================================================================
// struct PMaterial // list of all known `.mtl` file parameters
// ===========================================================================

std::string PMaterial::str() const
{
    std::ostringstream k;
    /*
    k << StringUtil::to_str(name);
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    */
    return k.str();
}

// ===========================================================================

std::wstring PMaterial::wstr() const
{
    std::wostringstream k;
    /*
    k << name;
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    */
    return k.str();
}

// ===========================================================================
// struct PVertex
// ===========================================================================
PVertex::PVertex()
    : pos(0,0,0)
    , normal(0,1,0)
    , tangent(1,0,0)
    , color( 0xFFFFFFFF )
    , tex()
    , tex2()
{}
PVertex::PVertex(  T x, T y, T z,    // pos
        T nx, T ny, T nz, // normal
        T tx, T ty, T tz, // tangent
        C4 colorRGBA, 	  // color
        T u1, T v1, T w1, // texcoords
        T u2, T v2, T w2 )// texcoords2
    : pos( x,y,z )
    , normal( nx,ny,nz )
    , tangent( tx,ty,tz )
    , color( colorRGBA )
    , tex( u1,v1,w1 )
    , tex2( u2,v2,w2 )
{}

PVertex::PVertex(  T x, T y, T z,    // pos
        T nx, T ny, T nz, // normal
        T tx, T ty, T tz, // tangent
        C4 c,  // color
        T u1, T v1, // texcoords
        T u2, T v2 )// texcoords2
    : pos( x,y,z )
    , normal( nx,ny,nz )
    , tangent( tx,ty,tz )
    , color( c )
    , tex( u1,v1,0 )
    , tex2( u2,v2,0 )
{}

PVertex::PVertex(	const V3 & p,	// pos
        const V3 & n,	// normal
        const V3 & t,	// tangent
        const C4 c, 	// color
        const V3 & uvw1,// texcoords
        const V3 & uvw2)// texcoords2
    : pos( p )
    , normal( n )
    , tangent( t )
    , color( c )
    , tex( uvw1 )
    , tex2( uvw2 )
{}

PVertex::PVertex(	const V3 & p,	// pos
        const V3 & n,	// normal
        const V3 & t,	// tangent
        const C4 c, 	// color
        const V2 & uv1, // texcoords
        const V2 & uv2) // texcoords2
    : pos( p )
    , normal( n )
    , tangent( t )
    , color( c )
    , tex( uv1.x, uv1.y, 0 )
    , tex2( uv2.x, uv2.y, 0 )
{}

std::string PVertex::str() const
{
    std::ostringstream k;
    /*
    k << StringUtil::to_str(name);
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    */
    return k.str();
}

// ===========================================================================

std::wstring PVertex::wstr() const
{
    std::wostringstream k;
    /*
    k << name;
    if ( bm ) { k << ", bm(" << bm.value() << ")"; }
    if ( mm ) { k << ", mm(" << mm.value() << ")"; }
    if ( s ) {  k << ", s(" << (*s).x << " " << (*s).y << " " << (*s).z << ")"; }
    if ( o ) {  k << ", o(" << (*o).x << " " << (*o).y << " " << (*o).z << ")"; }
    */
    return k.str();
}




PMeshBuffer::PMeshBuffer()
    : m_primType(PrimitiveType::Triangles)
    , m_bNeedUpload(true)
    , m_vao(0)
    , m_vbo_vertices(0)
    , m_vbo_indices(0)
    , m_vbo_instanceMat(0)
    //, m_vbo_instanceColor(0)
{
    resetInstances();
}

// ===========================================================================
void PMeshBuffer::upload()
// ===========================================================================
{
    if (m_vertices.empty())
	{
        DE_ERROR("Nothing to upload.")
		return;
	}
	
    if (!m_vbo_vertices)
    {
        glGenBuffers(1, &m_vbo_vertices);
        //DE_TRACE("m_vbo_vertices = ",m_vbo_vertices)
        m_bNeedUpload = true;
    }

    if (!m_vbo_indices && m_indices.size())
    {
        glGenBuffers(1, &m_vbo_indices);
        //DE_TRACE("m_vbo_indices = ",m_vbo_indices)
        m_bNeedUpload = true;
    }

    if (!m_vbo_instanceMat)
    {
        glGenBuffers(1, &m_vbo_instanceMat);
        //DE_TRACE("m_vbo_instanceMat = ",m_vbo_instanceMat)
        m_bNeedUpload = true;
    }

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao);
        //DE_TRACE("m_vao = ",m_vao)
        m_bNeedUpload = true;
    }

    if (!m_bNeedUpload)
    {
        return;
    }

    //DE_WARN("Upload ", m_vertices.size(), " vertices.")
    //DE_WARN("sizeof(Vertex) = ", sizeof(H3_ObjVertex))
    //DE_WARN("Upload ", m_instanceMat.size(), " modelInstances.")
    //DE_WARN("Upload ", m_instanceColor.size(), " colorInstances.")

    glBindVertexArray(m_vao);

    auto sizeV = sizeof(PVertex);
	
	uint64_t k = 0;
	uint64_t ks = 0;
    
	// m_vbo_vertices:
	// m_vbo_vertices = a_pos + a_normal + a_tangent + a_color + a_tex + a_tex2;
    const size_t nBytes = m_vertices.size() * sizeV;
    const uint8_t* pBytes = reinterpret_cast< const uint8_t* >( m_vertices.data() );
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(nBytes), pBytes, GL_STATIC_DRAW);
    // a_pos
    glVertexAttribPointer( k, 3, GL_FLOAT, GL_FALSE, sizeV, (void*)(0) );
    glEnableVertexAttribArray( k ); k++; ks += 12;
    // a_normal
    glVertexAttribPointer( k, 3, GL_FLOAT, GL_FALSE, sizeV, (void*)(ks) );
    glEnableVertexAttribArray( k ); k++; ks += 12;
    // a_tangent
    glVertexAttribPointer( k, 3, GL_FLOAT, GL_FALSE, sizeV, (void*)(ks) );
    glEnableVertexAttribArray( k ); k++; ks += 12;
    // a_color
    glVertexAttribPointer( k, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeV, (void*)(ks) );
    glEnableVertexAttribArray( k ); k++; ks += 4;
    // a_tex
    glVertexAttribPointer( k, 3, GL_FLOAT, GL_FALSE, sizeV, (void*)(ks) );
    glEnableVertexAttribArray( k ); k++; ks += 12;
    // a_tex2
    glVertexAttribPointer( k, 3, GL_FLOAT, GL_FALSE, sizeV, (void*)(ks) );
    glEnableVertexAttribArray( k ); k++; // ks += 12;

    // m_vbo_instanceMat:
    if ( m_instanceMat.size() > 0 )
	{
		ks = 0; // Reset offset for new mat4 buffer.
		// Upload 4x vec4 to form a modelMat mat4:
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
		glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(ks));
		glVertexAttribDivisor(k, 1); 
		glEnableVertexAttribArray(k); k++; ks += 16;
		glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(ks));
		glVertexAttribDivisor(k, 1);
		glEnableVertexAttribArray(k); k++; ks += 16;
		glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(ks));
		glVertexAttribDivisor(k, 1); 
		glEnableVertexAttribArray(k); k++; ks += 16;
		glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(ks));
		glVertexAttribDivisor(k, 1);
        glEnableVertexAttribArray(k); k++; // ks += 16;
	}
    else
    {
        DE_WARN("Upload 0 instances")
    }
	
    if (m_indices.size() > 0)
    {
        const auto nI = GLsizeiptr(m_indices.size() * sizeof(uint32_t));
        const auto pI = m_indices.data();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nI, pI, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
	
    m_bNeedUpload = false;
    m_bNeedInstanceUpload = false;
    // uploadInstances();
}

void
PMeshBuffer::uploadInstances()
{
    if (!m_vbo_instanceMat)
    {
        DE_ERROR("m_vbo_instanceMat must exist, call upload() before.")
        return;
    }

    if (!m_bNeedInstanceUpload) { return; } // Nothing todo.

    if (m_instanceMat.size() < 1)
    {
        DE_ERROR("No instances to upload")
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_bNeedInstanceUpload = false;
}

void
PMeshBuffer::draw()
{
    if (m_vertices.empty()) { DE_ERROR("No vertices") return; }
    if (m_instanceMat.empty()) { DE_ERROR("No instances") }

    upload();
    uploadInstances();

    if (!m_vao) { DE_ERROR("No vao.") return; }

    const GLenum pType = PrimitiveType::toOpenGL( m_primType );
    const GLenum iType = GL_UNSIGNED_INT;

	glBindVertexArray(m_vao);
	
    if (m_instanceMat.empty())
    {
		if (m_indices.empty())
		{
            glDrawArrays( pType,
                0, GLint(m_vertices.size()) );
		}
		else
		{
            glDrawElements( pType,
                GLint(m_indices.size()), iType, nullptr );
		}
    }
	else
	{
        const auto nInstances = m_instanceMat.size();
		if (m_indices.empty())
		{
            glDrawArraysInstanced( pType,
                0, m_vertices.size(), nInstances);
		}
		else
		{
            glDrawElementsInstanced( pType,
                GLsizei(m_indices.size()), iType, nullptr, nInstances);
		}
	}
	
    GL_VALIDATE;

    glBindVertexArray(0);
}

void PMeshBuffer::recalculateBoundingBox()
{
    m_bbox = Box3f();

    if (m_vertices.size() > 0)
    {
        auto & v = m_vertices[0];
        m_bbox.reset(v.pos);

        for (size_t i = 1; i < m_vertices.size(); ++i)
        {
            auto & v = m_vertices[i];
            m_bbox.addInternalPoint(v.pos);
        }
    }
}

uint64_t PMeshBuffer::computeByteConsumption() const
{
    uint64_t n = sizeof(this);
    n += m_vertices.capacity() * sizeof(PVertex);
    n += m_indices.capacity() * sizeof(uint32_t);
    n += m_instanceMat.capacity() * sizeof(M4);
    return n;
}

void PMeshBuffer::resetInstances( const M4& modelMat )
{
    m_instanceMat.clear();
    m_instanceMat.push_back(modelMat);
    m_bNeedInstanceUpload = true;
}

// BKS instance:
//       ( Right )  BKS = UCS
//       ( Up,   )      = user coord system reference frame
//   + M4( Front )      = x,y,z unit axis vectors + pos vector.
//       ( Pos   )
void PMeshBuffer::addInstance( const M4& modelMat )
{
    m_instanceMat.push_back(modelMat);
    m_bNeedInstanceUpload = true;
}

void PMeshBuffer::addVertex( const PVertex& v )
{
    m_vertices.emplace_back( v );
    m_bNeedUpload = true;
}

void PMeshBuffer::addIndex( const uint32_t i )
{
    m_indices.emplace_back( i );
    m_bNeedUpload = true;
}
//
//     B
//    / \
//   /   \
//  / -Y  \
// A-------C
//
void PMeshBuffer::addTriangle(V3 a, V3 b, V3 c,
                 V2 uvA, V2 uvB, V2 uvC )
{
    constexpr C4 white = 0xFFFFFFFF;

    auto n = glm::normalize( glm::cross( b-a, c-a ) );
    auto t = V3(1,0,0); // tangent
    // auto bt = V3(0,1,0); // bitangent

    m_vertices.emplace_back( a, n, t, white, uvA, uvA );
    m_vertices.emplace_back( b, n, t, white, uvB, uvB );
    m_vertices.emplace_back( c, n, t, white, uvC, uvC );
    m_bNeedUpload = true;
}


//
// B-------C
// |       |
// |  -Y   |
// |       |
// A-------D
//
void PMeshBuffer::addQuad(V3 a, V3 b, V3 c, V3 d, T u, T v )
{
    constexpr C4 white = 0xFFFFFFFF;

    auto n = glm::normalize( glm::cross( b-a, c-a ) );
    auto t = V3(1,0,0);
    //auto bt = V3(0,1,0);

    m_vertices.emplace_back( a, n, t, white, V2(0,v) );
    m_vertices.emplace_back( b, n, t, white, V2(0,0) );
    m_vertices.emplace_back( c, n, t, white, V2(u,0) );
    m_vertices.emplace_back( a, n, t, white, V2(0,v) );
    m_vertices.emplace_back( c, n, t, white, V2(u,0) );
    m_vertices.emplace_back( d, n, t, white, V2(u,v) );
    m_bNeedUpload = true;
}

std::string PMeshBuffer::str() const
{
    std::ostringstream o; o <<
        "name(" << m_name << "), "
                             "bbox(" << m_bbox.str() << "), "
                              "prim(" << PrimitiveType::getString( m_primType ) << "), "
                                                                        "v(" << m_vertices.size() << "), "
                              "i(" << m_indices.size() << "), "
                             "m(" << m_instanceMat.size() << ")";
    return o.str();
}

std::wstring PMeshBuffer::wstr() const { return StringUtil::to_wstr(str()); }

void PMeshBuffer::translate( const V3& o )
{
    for (auto & v : m_vertices) { v.pos += o; }
    recalculateBoundingBox();
}
void PMeshBuffer::scale( const V3& s )
{
    for (auto & v : m_vertices) { v.pos *= s; }
    recalculateBoundingBox();
}
void PMeshBuffer::transform( const M4& m )
{
    for (auto & v : m_vertices)
    {
        const V4 p = m * V4(v.pos, T(1));
        v.pos = V3(p) / p.w; // bool divideByW is kinda expensive
    }
    recalculateBoundingBox();
}



// ===========================================================================
// struct PMesh // -o
// ===========================================================================

void PMesh::recalculateBoundingBox()
{
    m_bbox = Box3f();

    if (m_limbs.size() > 0)
    {
        m_bbox.reset(m_limbs[0].m_bbox);

        for (size_t i = 1; i < m_limbs.size(); ++i)
        {
            m_bbox.addInternalBox(m_limbs[i].m_bbox);
        }
    }
}

uint64_t PMesh::computeByteConsumption() const
{
    uint64_t n = sizeof(this);
    for (const auto & m : m_limbs) { n += m.computeByteConsumption(); }
    return n;
}

std::string PMesh::str() const
{
    std::ostringstream o; o <<
        "PMesh(" << m_name << "), "
                              "bbox(" << m_bbox.str() << "), "
                              "limbs(" << m_limbs.size() << ")\n";
    for (size_t i = 0; i < m_limbs.size(); ++i)
    {
        o << "PMeshBuffer[" << i << "] " << m_limbs[i].str() << "\n";
    }
    return o.str();
}

std::wstring PMesh::wstr() const { return StringUtil::to_wstr(str()); }

void PMesh::translate( const V3& o )
{
    for (auto & limb : m_limbs) { limb.translate(o); }
    recalculateBoundingBox();
}
void PMesh::scale( const V3& s )
{
    for (auto & limb : m_limbs) { limb.scale(s); }
    recalculateBoundingBox();
}
void PMesh::transform( const M4& m )
{
    for (auto & limb : m_limbs) { limb.transform(m); }
    recalculateBoundingBox();
}

void PMesh::center( const V3& o )
{
    translate(o - m_bbox.getCenter());
    recalculateBoundingBox();
}
} // namespace gpu.
} // namespace de.


