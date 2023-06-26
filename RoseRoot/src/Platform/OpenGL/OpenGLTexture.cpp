#include "rrpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rose
 {
	namespace Utils {
		static GLenum RoseFormatToGLFormat(ImageFormat format) 
		{
			switch (format)
			{
			case Rose::ImageFormat::RGB8: return GL_RGB;
			case Rose::ImageFormat::RGBA8:return GL_RGBA;
			}
			RR_CORE_ERROR("RoseFormatToGLFormat could not convert {} to GLenum!", (int)format);
			RR_CORE_ASSERT(false);
			return 0;
		}
		static GLenum RoseFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case Rose::ImageFormat::RGB8: return GL_RGB8;
			case Rose::ImageFormat::RGBA8:return GL_RGBA8;
			}
			RR_CORE_ERROR("RoseFormatToGLInternalFormat could not convert {} to GLenum!", (int)format);
			RR_CORE_ASSERT(false);
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecification& specification, Buffer data)
		: m_Specification(specification), m_Width(specification.Width), m_Height(specification.Height)
	{
		RR_PROFILE_FUNCTION();

		m_InternalFormat = Utils::RoseFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = Utils::RoseFormatToGLFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
			SetData(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		RR_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(Buffer data)
	{
		RR_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;

		switch (m_DataFormat)
		{
		case GL_RGBA:
			bpp = 4; break;
		case GL_RGB:
			bpp = 3; break;
		case GL_RG:
			bpp = 2; break;
		case GL_R:
			bpp = 1; break;
		}
		RR_CORE_ASSERT(!bpp == 0, "Invalid data format");

		RR_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		RR_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}