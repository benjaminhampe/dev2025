#include "core/gl_util/ComputeTexture.h"
//#include <iostream>

ComputeTexture::ComputeTexture(int width, int height, unsigned int binding_point)
: m_Width(width), m_Height(height), m_binding_point(binding_point), m_RendererID(0) {

	//IMPORTANT: the source texture has to be attached to a bound frame buffer for this to work
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
	GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTextureStorage2D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height));
	GLCall(glBindImageTexture(m_binding_point, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
    m_RendererID64 = m_RendererID;
}

ComputeTexture::~ComputeTexture()
{
    m_RendererID64 = 0;
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void ComputeTexture::changeBindingPoint(unsigned int binding_point)
{
	m_binding_point = binding_point;
	Bind();
}

void ComputeTexture::Bind() const
{
    // (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
	GLCall(glBindImageTexture(m_binding_point, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
}

void ComputeTexture::Unbind() const
{
    // (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
	GLCall(glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
}
