#pragma once

#ifdef EDITOR_HAS_VULKAN

#include <cstdint>
#include <vulkan/vulkan.h>

struct GLFWwindow;
struct ImDrawData;

namespace Editor
{
	class VulkanRenderer
	{
	public:
		static bool Init(GLFWwindow* window, uint32_t width, uint32_t height, bool vsync);
		static void Shutdown();
		static void ShutdownImGui();
		static bool InitImGui();
		static void NewFrame();
		static void Render(ImDrawData* drawData);
		static void SetVSync(bool enabled);
		static bool IsInitialized();
		static VkDevice GetDevice();
		static VkPhysicalDevice GetPhysicalDevice();
		static VkQueue GetQueue();
		static uint32_t GetQueueFamily();
		static VkCommandBuffer BeginSingleTimeCommands();
		static bool EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		static VkDescriptorSet AddTexture(VkSampler sampler, VkImageView imageView);
		static void RemoveTexture(VkDescriptorSet descriptorSet);
		static void RetireTexture(VkDescriptorSet descriptorSet, VkSampler sampler,
			VkImageView imageView, VkImage image, VkDeviceMemory imageMemory);
		static uint64_t GetTextureID(VkDescriptorSet descriptorSet);
	};
}

#endif
