#include "pch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "OpenGLTexture2D.h"
#ifdef EDITOR_HAS_VULKAN
#include "VulkanTexture2D.h"
#endif



namespace Editor {
    Ref<Texture2D> Texture2D::Load(const TextureSpecification& specification)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(specification);
#ifdef EDITOR_HAS_VULKAN
        case RendererAPI::API::Vulkan:  return CreateRef<VulkanTexture2D>(specification);
#endif
        }

        ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Load(const std::string& path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
#ifdef EDITOR_HAS_VULKAN
        case RendererAPI::API::Vulkan:  return CreateRef<VulkanTexture2D>(path);
#endif
        }

        ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}