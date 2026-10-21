#include "pch.h"

#ifdef EDITOR_HAS_VULKAN

#include "VulkanTexture2D.h"
#include "VulkanRenderer.h"
#include "Log.h"

#include <stb_image.h>
#include <stb_image_write.h>

namespace Editor
{
	namespace
	{
		VkFormat GetVulkanFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::R8:      return VK_FORMAT_R8_UNORM;
			case ImageFormat::RGB8:    return VK_FORMAT_R8G8B8_UNORM;
			case ImageFormat::RGBA8:   return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
			default:                  return VK_FORMAT_UNDEFINED;
			}
		}

		uint32_t GetBytesPerPixel(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::R8:      return 1;
			case ImageFormat::RGB8:    return 3;
			case ImageFormat::RGBA8:   return 4;
			case ImageFormat::RGBA32F: return 16;
			default:                  return 0;
			}
		}

		bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory)
		{
			VkDevice device = VulkanRenderer::GetDevice();
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
				return false;

			VkMemoryRequirements requirements{};
			vkGetBufferMemoryRequirements(device, buffer, &requirements);
			const uint32_t memoryType = VulkanRenderer::FindMemoryType(
				requirements.memoryTypeBits, properties);
			if (memoryType == UINT32_MAX)
				return false;

			VkMemoryAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.allocationSize = requirements.size;
			allocateInfo.memoryTypeIndex = memoryType;
			if (vkAllocateMemory(device, &allocateInfo, nullptr, &memory) != VK_SUCCESS)
				return false;
			return vkBindBufferMemory(device, buffer, memory, 0) == VK_SUCCESS;
		}

		void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image,
			VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;
			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
				newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
				newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}

			vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0,
				0, nullptr, 0, nullptr, 1, &barrier);
		}
	}

	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& specification)
		: m_Specification(specification), m_Width(specification.Width), m_Height(specification.Height),
		m_Channels(GetBytesPerPixel(specification.Format))
	{
		if (m_Width == 0 || m_Height == 0 || m_Channels == 0)
		{
			CORE_ERROR("Invalid Vulkan texture dimensions or image format.");
			return;
		}
		m_Pixels.resize(static_cast<size_t>(m_Width) * m_Height * m_Channels);
		CreateResources(m_Pixels.data(), m_Pixels.size());
	}

	VulkanTexture2D::VulkanTexture2D(const std::string& path)
		: m_Path(path)
	{
		int width = 0;
		int height = 0;
		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
		if (!pixels)
		{
			CORE_ERROR("Failed to load texture '{}': {}", path, stbi_failure_reason());
			return;
		}

		m_Width = static_cast<uint32_t>(width);
		m_Height = static_cast<uint32_t>(height);
		m_Channels = 4;
		m_Specification.Width = m_Width;
		m_Specification.Height = m_Height;
		m_Specification.Format = ImageFormat::RGBA8;
		m_Pixels.assign(pixels, pixels + static_cast<size_t>(m_Width) * m_Height * m_Channels);
		stbi_image_free(pixels);

		m_IsLoaded = CreateResources(m_Pixels.data(), m_Pixels.size());
		if (m_IsLoaded)
			CORE_INFO("Loaded texture from path: {}", path);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		ReleaseResources();
	}

	bool VulkanTexture2D::CreateResources(const unsigned char* pixels, size_t byteCount)
	{
		if (!UploadPixels(pixels, byteCount, true))
			return false;

		VkDevice device = VulkanRenderer::GetDevice();
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = GetVulkanFormat(m_Specification.Format);
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(device, &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to create Vulkan texture image view.");
			ReleaseResources();
			return false;
		}

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		if (vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to create Vulkan texture sampler.");
			ReleaseResources();
			return false;
		}

		m_DescriptorSet = VulkanRenderer::AddTexture(m_Sampler, m_ImageView);
		if (m_DescriptorSet == VK_NULL_HANDLE)
		{
			CORE_ERROR("Failed to allocate a Vulkan texture descriptor.");
			ReleaseResources();
			return false;
		}
		m_RendererID = VulkanRenderer::GetTextureID(m_DescriptorSet);
		return true;
	}

	bool VulkanTexture2D::UploadPixels(const unsigned char* pixels, size_t byteCount, bool createImage)
	{
		const VkDevice device = VulkanRenderer::GetDevice();
		const VkFormat format = GetVulkanFormat(m_Specification.Format);
		if (device == VK_NULL_HANDLE || format == VK_FORMAT_UNDEFINED || pixels == nullptr ||
			byteCount != static_cast<size_t>(m_Width) * m_Height * m_Channels)
		{
			CORE_ERROR("Invalid Vulkan texture upload data.");
			return false;
		}

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
		if (!CreateBuffer(byteCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingMemory))
		{
			CORE_ERROR("Failed to allocate Vulkan texture staging buffer.");
			if (stagingBuffer != VK_NULL_HANDLE)
				vkDestroyBuffer(device, stagingBuffer, nullptr);
			if (stagingMemory != VK_NULL_HANDLE)
				vkFreeMemory(device, stagingMemory, nullptr);
			return false;
		}

		void* mapped = nullptr;
		if (vkMapMemory(device, stagingMemory, 0, byteCount, 0, &mapped) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to map Vulkan texture staging memory.");
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingMemory, nullptr);
			return false;
		}
		memcpy(mapped, pixels, byteCount);
		vkUnmapMemory(device, stagingMemory);

		if (createImage)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent = { m_Width, m_Height, 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			if (vkCreateImage(device, &imageInfo, nullptr, &m_Image) != VK_SUCCESS)
			{
				CORE_ERROR("Failed to create Vulkan texture image.");
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingMemory, nullptr);
				return false;
			}

			VkMemoryRequirements requirements{};
			vkGetImageMemoryRequirements(device, m_Image, &requirements);
			const uint32_t memoryType = VulkanRenderer::FindMemoryType(
				requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (memoryType == UINT32_MAX)
			{
				CORE_ERROR("No compatible Vulkan memory type for texture image.");
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingMemory, nullptr);
				ReleaseResources();
				return false;
			}

			VkMemoryAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.allocationSize = requirements.size;
			allocateInfo.memoryTypeIndex = memoryType;
			if (vkAllocateMemory(device, &allocateInfo, nullptr, &m_ImageMemory) != VK_SUCCESS ||
				vkBindImageMemory(device, m_Image, m_ImageMemory, 0) != VK_SUCCESS)
			{
				CORE_ERROR("Failed to allocate Vulkan texture image memory.");
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingMemory, nullptr);
				ReleaseResources();
				return false;
			}
		}

		VkCommandBuffer commandBuffer = VulkanRenderer::BeginSingleTimeCommands();
		if (commandBuffer == VK_NULL_HANDLE)
		{
			CORE_ERROR("Failed to create Vulkan texture upload command buffer.");
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingMemory, nullptr);
			return false;
		}

		TransitionImageLayout(commandBuffer, m_Image,
			createImage ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VkBufferImageCopy copyRegion{};
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = { m_Width, m_Height, 1 };
		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, m_Image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
		TransitionImageLayout(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		const bool uploadSucceeded = VulkanRenderer::EndSingleTimeCommands(commandBuffer);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
		return uploadSucceeded;
	}

	void VulkanTexture2D::ReleaseResources()
	{
		VulkanRenderer::RetireTexture(m_DescriptorSet, m_Sampler, m_ImageView, m_Image, m_ImageMemory);
		m_DescriptorSet = VK_NULL_HANDLE;
		m_RendererID = 0;
		m_Sampler = VK_NULL_HANDLE;
		m_ImageView = VK_NULL_HANDLE;
		m_Image = VK_NULL_HANDLE;
		m_ImageMemory = VK_NULL_HANDLE;
	}

	int VulkanTexture2D::Save(const std::string& path) const
	{
		if (!m_IsLoaded)
			return -1;
		if (m_Channels > 4 || m_Channels == 0)
			return -1;
		return stbi_write_png(path.c_str(), static_cast<int>(m_Width),
			static_cast<int>(m_Height), static_cast<int>(m_Channels), m_Pixels.data(),
			static_cast<int>(m_Width * m_Channels)) ? 0 : -1;
	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
		if (!data || size != static_cast<size_t>(m_Width) * m_Height * m_Channels)
		{
			CORE_ERROR("Vulkan texture data size must match the full texture.");
			return;
		}

		std::vector<unsigned char> pixels(static_cast<unsigned char*>(data),
			static_cast<unsigned char*>(data) + size);
		if (UploadPixels(pixels.data(), pixels.size(), false))
			m_Pixels = std::move(pixels);
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
		(void)slot;
	}
}

#endif
