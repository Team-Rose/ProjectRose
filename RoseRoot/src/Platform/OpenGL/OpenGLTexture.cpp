#include "rrpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include "stb_image.h"

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
	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecification& specification)
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
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		RR_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			RR_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;

			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			RR_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		RR_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
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

		RR_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		RR_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}