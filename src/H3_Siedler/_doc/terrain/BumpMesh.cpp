#include <de/terrain/BumpMesh.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {
	
// ===========================================================================
BumpMesh::BumpMesh()
// ===========================================================================
{
    m_primitiveType = de::gpu::PrimitiveType::Triangles;
    m_vao = 0;
    m_vbo_vertices = 0;
    m_vbo_indices = 0;
    m_vbo_instanceMat = 0;
}

void
BumpMesh::upload()
{
    //DE_WARN("Upload ", m_vertices.size(), " vertices.")
    //DE_WARN("sizeof(Vertex) = ", sizeof(BumpVertex))
    //DE_WARN("Upload ", m_instanceMat.size(), " modelInstances.")
    //DE_WARN("Upload ", m_instanceColor.size(), " colorInstances.")

    if (!m_vbo_vertices)
    {
        glGenBuffers(1, &m_vbo_vertices);
        //DE_TRACE("m_vbo_vertices = ",m_vbo_vertices)
    }

    if (!m_vbo_indices && m_indices.size())
    {
        glGenBuffers(1, &m_vbo_indices);
        //DE_TRACE("m_vbo_indices = ",m_vbo_indices)
    }

    if (!m_vbo_instanceMat)
    {
        glGenBuffers(1, &m_vbo_instanceMat);
        //DE_TRACE("m_vbo_instanceMat = ",m_vbo_instanceMat)
    }

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao);
        //DE_TRACE("m_vao = ",m_vao)
    }

    glBindVertexArray(m_vao);

    int k = 0;
	
    // [vbo] vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(BumpVertex), 
					m_vertices.data(), GL_STATIC_DRAW);
    // a_pos
    glVertexAttribPointer( k, 3, GL_FLOAT, GL_FALSE, sizeof(BumpVertex), (void*)(0) );
    glEnableVertexAttribArray( k ); k++;
    // a_tex
    glVertexAttribPointer( k, 2, GL_FLOAT, GL_FALSE, sizeof(BumpVertex), (void*)(12) );
    glEnableVertexAttribArray( k ); k++;

    // [vbo] instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), 
					m_instanceMat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;

    if (m_vbo_indices)
    {
        const auto & indices = m_indices;
        const size_t indexCount = indices.size();
        const size_t indexSize = sizeof(uint32_t);
        const size_t indexBytes = indexCount * indexSize;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), 
		              m_indices.data(), GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
}

void
BumpMesh::render()
{
    if (m_instanceMat.empty())
    {
        DE_WARN("Nothing to draw")
        return;
    }

    glBindVertexArray(m_vao);

    const GLenum primType = PrimitiveType::toOpenGL( m_primitiveType );

    if (m_instanceMat.empty())
    {
        DE_ERROR("No instance")
    }
    else
    {
        if (m_indices.empty())
        {
            glDrawArraysInstanced(primType, 0, m_vertices.size(), m_instanceMat.size());
        }
        else
        {
            glDrawElementsInstanced(primType, GLsizei(m_indices.size()), GL_UNSIGNED_INT, nullptr, m_instanceMat.size());
        }
    }
    GL_VALIDATE;

    glBindVertexArray(0);
}

} // end namespace gpu.
} // end namespace de.
