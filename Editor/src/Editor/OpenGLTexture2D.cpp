#include "pch.h"
#include "OpenGLTexture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "Log.h"

namespace Editor {
	namespace Utils {

		static GLenum ImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
			}

			ASSERT(false);
			return 0;
		}

		static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			ASSERT(false);
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec) : spec(spec), width(spec.Width), height(spec.Height)
	{

		internalFormat = Utils::ImageFormatToGLInternalFormat(spec.Format);
		dataFormat = Utils::ImageFormatToGLDataFormat(spec.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glTextureStorage2D(rendererID, 1, internalFormat, width, width);

		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : path(path)
	{

		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (data)
		{
			isLoaded = true;
			Log::GetCoreLogger()->Info("Loaded texture from path: " + path);
			
			this->width = width;
			this->height = height;

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

			this->internalFormat = internalFormat;
			this->dataFormat = dataFormat;

			ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
			glTextureStorage2D(rendererID, 1, internalFormat, width, height);

			glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else
		{
			Log::GetCoreLogger()->Error("Failed to load texture from path: " +  path);
			//ASSERT(false, "Failed to load texture!");
			isLoaded = false;
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &rendererID);
	}
	

	int OpenGLTexture2D::Save(const std::string& path) const
	{

		if (!isLoaded) return -1;

		if (!rendererID)
			return -2;

		glBindTexture(GL_TEXTURE_2D, rendererID);

		int channels = 4;

		switch (dataFormat)
		{
		case GL_RED:
			channels = 1;
			break;

		case GL_RGB:
			channels = 3;
			break;

		case GL_RGBA:
			channels = 4;
			break;

		default:
			return -1;
		}

		std::vector<unsigned char> pixels(width * height * channels);

		glGetTexImage(
			GL_TEXTURE_2D,
			0,
			dataFormat,
			GL_UNSIGNED_BYTE,
			pixels.data()
		);

		const int stride = width * channels;
		std::vector<unsigned char> flipped(pixels.size());

		for (uint32_t y = 0; y < height; y++)
		{
			memcpy(
				&flipped[y * stride],
				&pixels[(height - 1 - y) * stride],
				stride
			);
		}
		int success = stbi_write_png(
			path.c_str(),
			width,
			height,
			channels,
			flipped.data(),
			stride
		);
		return 0;
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{

		uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;
		ASSERT(size == width * height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}
}