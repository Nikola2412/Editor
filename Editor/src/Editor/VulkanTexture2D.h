#pragma once

#ifdef EDITOR_HAS_VULKAN

#include "Texture.h"

#include <vulkan/vulkan.h>
#include <cstddef>

namespace Editor
{
	class VulkanTexture2D final : public Texture2D
	{
	public:
		explicit VulkanTexture2D(const TextureSpecification& specification);
		explicit VulkanTexture2D(const std::string& path);
		~VulkanTexture2D() override;

		const TextureSpecification& GetSpecification() const override { return m_Specification; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		float GetRotation() const override { return m_Specification.Rotation; }
		uint64_t GetRendererID() const override { return m_RendererID; }
		int Save(const std::string& path) const override;
		const std::string& GetPath() const override { return m_Path; }
		void SetData(void* data, uint32_t size) override;
		void Bind(uint32_t slot = 0) const override;
		bool IsLoaded() const override { return m_IsLoaded; }
		bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}

	private:
		bool CreateResources(const unsigned char* pixels, size_t byteCount);
		bool UploadPixels(const unsigned char* pixels, size_t byteCount, bool createImage);
		void ReleaseResources();

		TextureSpecification m_Specification;
		std::string m_Path;
		std::vector<unsigned char> m_Pixels;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Channels = 0;
		uint64_t m_RendererID = 0;
		VkImage m_Image = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
		bool m_IsLoaded = false;
	};
}

#endif
