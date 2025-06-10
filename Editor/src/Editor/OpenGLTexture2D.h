#pragma once
#include "Texture.h"

#include <glad/glad.h>


namespace Editor {

    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const TextureSpecification& specification);
        OpenGLTexture2D(const std::string& path);
        virtual ~OpenGLTexture2D();

        virtual const TextureSpecification& GetSpecification() const override { return spec; }


        virtual uint32_t GetWidth() const override { return width; }
        virtual uint32_t GetHeight() const override { return height; }
        virtual uint32_t GetRendererID() const override { return rendererID; }

        virtual const std::string& GetPath() const override { return path; }

        virtual void SetData(void* data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool IsLoaded() const override { return isLoaded; }

        virtual bool operator==(const Texture& other) const override
        {
            return rendererID == other.GetRendererID();
        }
    private:
        TextureSpecification spec;

        std::string path;
        bool isLoaded = false;
        uint32_t width, height;
        uint32_t rendererID;
        GLenum internalFormat = 0, dataFormat = 0;
    };

}