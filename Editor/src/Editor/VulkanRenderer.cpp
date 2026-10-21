#include "pch.h"

#ifdef EDITOR_HAS_VULKAN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Log.h"
#include "VulkanRenderer.h"

#include <cstring>
#include <iterator>

namespace Editor
{
	namespace
	{
		VkInstance s_Instance = VK_NULL_HANDLE;
		VkPhysicalDevice s_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice s_Device = VK_NULL_HANDLE;
		uint32_t s_QueueFamily = UINT32_MAX;
		VkQueue s_Queue = VK_NULL_HANDLE;
		VkCommandPool s_CommandPool = VK_NULL_HANDLE;
		VkDescriptorPool s_DescriptorPool = VK_NULL_HANDLE;
		VkSurfaceKHR s_Surface = VK_NULL_HANDLE;
		struct RetiredTexture
		{
			VkDescriptorSet DescriptorSet;
			VkSampler Sampler;
			VkImageView ImageView;
			VkImage Image;
			VkDeviceMemory ImageMemory;
		};
		std::vector<RetiredTexture> s_RetiredTextures;
		ImGui_ImplVulkanH_Window s_MainWindow;
		GLFWwindow* s_Window = nullptr;
		bool s_Initialized = false;
		bool s_ImGuiInitialized = false;
		bool s_DeviceIdle = true;
		bool s_DeviceLost = false;
		bool s_VSync = true;
		constexpr uint32_t MinImageCount = 2;

		bool HasDeviceExtension(VkPhysicalDevice device, const char* extension)
		{
			uint32_t count = 0;
			if (vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr) != VK_SUCCESS)
				return false;

			std::vector<VkExtensionProperties> extensions(count);
			if (vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data()) != VK_SUCCESS)
				return false;

			return std::any_of(extensions.begin(), extensions.end(),
				[extension](const VkExtensionProperties& value)
				{
					return strcmp(value.extensionName, extension) == 0;
				});
		}

		bool FindQueueFamily(VkPhysicalDevice device, uint32_t& queueFamily)
		{
			uint32_t count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
			std::vector<VkQueueFamilyProperties> properties(count);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());

			for (uint32_t i = 0; i < count; ++i)
			{
				VkBool32 presentSupport = VK_FALSE;
				if ((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, s_Surface, &presentSupport) == VK_SUCCESS &&
					presentSupport)
				{
					queueFamily = i;
					return true;
				}
			}
			return false;
		}

		bool SupportsPresentation(VkPhysicalDevice device)
		{
			uint32_t formatCount = 0;
			uint32_t presentModeCount = 0;
			VkSurfaceCapabilitiesKHR capabilities{};
			return vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, s_Surface, &capabilities) == VK_SUCCESS &&
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, s_Surface, &formatCount, nullptr) == VK_SUCCESS &&
				formatCount > 0 &&
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, s_Surface, &presentModeCount, nullptr) == VK_SUCCESS &&
				presentModeCount > 0 &&
				(capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0;
		}

		bool SelectPhysicalDevice()
		{
			uint32_t count = 0;
			if (vkEnumeratePhysicalDevices(s_Instance, &count, nullptr) != VK_SUCCESS || count == 0)
				return false;

			std::vector<VkPhysicalDevice> devices(count);
			if (vkEnumeratePhysicalDevices(s_Instance, &count, devices.data()) != VK_SUCCESS)
				return false;

			for (VkPhysicalDevice device : devices)
			{
				uint32_t family = UINT32_MAX;
				if (HasDeviceExtension(device, VK_KHR_SWAPCHAIN_EXTENSION_NAME) &&
					FindQueueFamily(device, family) && SupportsPresentation(device))
				{
					s_PhysicalDevice = device;
					s_QueueFamily = family;
					return true;
				}
			}
			return false;
		}

		bool CreateDevice()
		{
			const float priority = 1.0f;
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = s_QueueFamily;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &priority;

			const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			VkDeviceCreateInfo deviceInfo{};
			deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceInfo.queueCreateInfoCount = 1;
			deviceInfo.pQueueCreateInfos = &queueInfo;
			deviceInfo.enabledExtensionCount = 1;
			deviceInfo.ppEnabledExtensionNames = extensions;

			if (vkCreateDevice(s_PhysicalDevice, &deviceInfo, nullptr, &s_Device) != VK_SUCCESS)
				return false;

			vkGetDeviceQueue(s_Device, s_QueueFamily, 0, &s_Queue);
			return true;
		}

		bool CreateDescriptorPool()
		{
			const VkDescriptorPoolSize poolSizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolInfo.maxSets = 11000;
			poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
			poolInfo.pPoolSizes = poolSizes;
			return vkCreateDescriptorPool(s_Device, &poolInfo, nullptr, &s_DescriptorPool) == VK_SUCCESS;
		}

		bool CreateCommandPool()
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = s_QueueFamily;
			return vkCreateCommandPool(s_Device, &poolInfo, nullptr, &s_CommandPool) == VK_SUCCESS;
		}

		void SetupSwapchain(uint32_t width, uint32_t height)
		{
			s_MainWindow.Surface = s_Surface;
			const VkFormat formats[] =
			{
				VK_FORMAT_B8G8R8A8_UNORM,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_FORMAT_B8G8R8_UNORM,
				VK_FORMAT_R8G8B8_UNORM
			};
			s_MainWindow.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
				s_PhysicalDevice, s_Surface, formats, static_cast<int>(std::size(formats)),
				VK_COLORSPACE_SRGB_NONLINEAR_KHR);

			const VkPresentModeKHR vsyncModes[] = { VK_PRESENT_MODE_FIFO_KHR };
			const VkPresentModeKHR noVsyncModes[] =
			{
				VK_PRESENT_MODE_MAILBOX_KHR,
				VK_PRESENT_MODE_IMMEDIATE_KHR,
				VK_PRESENT_MODE_FIFO_KHR
			};
			s_MainWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
				s_PhysicalDevice, s_Surface, s_VSync ? vsyncModes : noVsyncModes,
				s_VSync ? static_cast<int>(std::size(vsyncModes)) : static_cast<int>(std::size(noVsyncModes)));

			ImGui_ImplVulkanH_CreateOrResizeWindow(
				s_Instance, s_PhysicalDevice, s_Device, &s_MainWindow, s_QueueFamily,
				nullptr, static_cast<int>(width), static_cast<int>(height), MinImageCount, 0);
		}

		bool InitializeImGuiBackend()
		{
			ImGui_ImplVulkan_InitInfo initInfo{};
			initInfo.ApiVersion = VK_API_VERSION_1_0;
			initInfo.Instance = s_Instance;
			initInfo.PhysicalDevice = s_PhysicalDevice;
			initInfo.Device = s_Device;
			initInfo.QueueFamily = s_QueueFamily;
			initInfo.Queue = s_Queue;
			initInfo.DescriptorPool = s_DescriptorPool;
			initInfo.MinImageCount = MinImageCount;
			initInfo.ImageCount = s_MainWindow.ImageCount;
			initInfo.PipelineInfoMain.RenderPass = s_MainWindow.RenderPass;
			initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			initInfo.CheckVkResultFn = [](VkResult result)
			{
				if (result != VK_SUCCESS)
					CORE_ERROR("Vulkan ImGui backend error: {}", static_cast<int>(result));
			};
			if (!ImGui_ImplVulkan_Init(&initInfo))
				return false;
			s_ImGuiInitialized = true;
			return true;
		}

		bool CreateInstanceAndSurface(GLFWwindow* window)
		{
			uint32_t extensionCount = 0;
			const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
			if (!extensions || extensionCount == 0)
				return false;

			VkApplicationInfo applicationInfo{};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pApplicationName = "Editor";
			applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = "Editor";
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo instanceInfo{};
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pApplicationInfo = &applicationInfo;
			instanceInfo.enabledExtensionCount = extensionCount;
			instanceInfo.ppEnabledExtensionNames = extensions;
			if (vkCreateInstance(&instanceInfo, nullptr, &s_Instance) != VK_SUCCESS)
				return false;

			return glfwCreateWindowSurface(s_Instance, window, nullptr, &s_Surface) == VK_SUCCESS;
		}

		void CheckVulkanResult(VkResult result, const char* operation);
		VkResult WaitForDeviceIdle();

		void RecreateSwapchain(uint32_t width, uint32_t height)
		{
			if (width == 0 || height == 0)
				return;

			const VkResult waitResult = WaitForDeviceIdle();
			if (waitResult != VK_SUCCESS)
			{
				CheckVulkanResult(waitResult, "waiting before recreating the swapchain");
				return;
			}
			const bool reinitializeImGui = s_ImGuiInitialized;
			if (reinitializeImGui)
			{
				ImGui_ImplVulkan_Shutdown();
				s_ImGuiInitialized = false;
			}

			SetupSwapchain(width, height);
			if (reinitializeImGui && !InitializeImGuiBackend())
				CORE_ERROR("Failed to reinitialize the Vulkan ImGui renderer after resizing.");
		}

		void CheckVulkanResult(VkResult result, const char* operation)
		{
			if (result != VK_SUCCESS)
			{
				if (result == VK_ERROR_DEVICE_LOST)
					s_DeviceLost = true;
				CORE_ERROR("Vulkan operation '{}' failed with VkResult {}", operation, static_cast<int>(result));
			}
		}

		VkResult WaitForDeviceIdle()
		{
			if (s_Device == VK_NULL_HANDLE || s_DeviceIdle)
				return VK_SUCCESS;
			if (s_DeviceLost)
				return VK_ERROR_DEVICE_LOST;

			const VkResult result = vkDeviceWaitIdle(s_Device);
			if (result == VK_SUCCESS)
				s_DeviceIdle = true;
			else if (result == VK_ERROR_DEVICE_LOST)
				s_DeviceLost = true;
			return result;
		}

		void FlushRetiredTextures(bool deviceIdle = false)
		{
			if (s_RetiredTextures.empty() || s_Device == VK_NULL_HANDLE)
				return;
			if (!deviceIdle && !s_DeviceIdle)
			{
				const VkResult result = WaitForDeviceIdle();
				if (result != VK_SUCCESS && result != VK_ERROR_DEVICE_LOST)
				{
					CORE_ERROR("Waiting before releasing retired Vulkan textures failed with VkResult {}.",
						static_cast<int>(result));
					return;
				}
			}

			for (const RetiredTexture& texture : s_RetiredTextures)
			{
				if (texture.DescriptorSet != VK_NULL_HANDLE)
					VulkanRenderer::RemoveTexture(texture.DescriptorSet);
				if (texture.Sampler != VK_NULL_HANDLE)
					vkDestroySampler(s_Device, texture.Sampler, nullptr);
				if (texture.ImageView != VK_NULL_HANDLE)
					vkDestroyImageView(s_Device, texture.ImageView, nullptr);
				if (texture.Image != VK_NULL_HANDLE)
					vkDestroyImage(s_Device, texture.Image, nullptr);
				if (texture.ImageMemory != VK_NULL_HANDLE)
					vkFreeMemory(s_Device, texture.ImageMemory, nullptr);
			}
			s_RetiredTextures.clear();
		}
	}

	bool VulkanRenderer::Init(GLFWwindow* window, uint32_t width, uint32_t height, bool vsync)
	{
		s_Window = window;
		s_VSync = vsync;
		if (!glfwVulkanSupported() || !CreateInstanceAndSurface(window) ||
			!SelectPhysicalDevice() || !CreateDevice() || !CreateCommandPool() || !CreateDescriptorPool())
		{
			CORE_WARN("Vulkan initialization failed; using OpenGL instead.");
			Shutdown();
			return false;
		}

		SetupSwapchain(width, height);
		if (s_MainWindow.Swapchain == VK_NULL_HANDLE ||
			s_MainWindow.RenderPass == VK_NULL_HANDLE ||
			s_MainWindow.Frames.Size == 0)
		{
			CORE_WARN("Vulkan swapchain initialization failed; using OpenGL instead.");
			Shutdown();
			return false;
		}
		s_Initialized = true;
		CORE_INFO("Vulkan renderer initialized.");
		return true;
	}

	void VulkanRenderer::Shutdown()
	{
		const VkResult waitResult = WaitForDeviceIdle();
		if (waitResult != VK_SUCCESS)
			CheckVulkanResult(waitResult, "waiting for the device during shutdown");
		FlushRetiredTextures(s_DeviceIdle || s_DeviceLost);
		if (s_ImGuiInitialized)
		{
			ImGui_ImplVulkan_Shutdown();
			s_ImGuiInitialized = false;
		}
		if (s_MainWindow.Swapchain != VK_NULL_HANDLE)
			ImGui_ImplVulkanH_DestroyWindow(s_Instance, s_Device, &s_MainWindow, nullptr);
		if (s_Surface != VK_NULL_HANDLE && s_Instance != VK_NULL_HANDLE)
			vkDestroySurfaceKHR(s_Instance, s_Surface, nullptr);
		if (s_DescriptorPool != VK_NULL_HANDLE && s_Device != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(s_Device, s_DescriptorPool, nullptr);
		if (s_CommandPool != VK_NULL_HANDLE && s_Device != VK_NULL_HANDLE)
			vkDestroyCommandPool(s_Device, s_CommandPool, nullptr);
		if (s_Device != VK_NULL_HANDLE)
			vkDestroyDevice(s_Device, nullptr);
		if (s_Instance != VK_NULL_HANDLE)
			vkDestroyInstance(s_Instance, nullptr);

		s_MainWindow = ImGui_ImplVulkanH_Window{};
		s_Instance = VK_NULL_HANDLE;
		s_PhysicalDevice = VK_NULL_HANDLE;
		s_Device = VK_NULL_HANDLE;
		s_Queue = VK_NULL_HANDLE;
		s_QueueFamily = UINT32_MAX;
		s_DescriptorPool = VK_NULL_HANDLE;
		s_Surface = VK_NULL_HANDLE;
		s_Window = nullptr;
		s_Initialized = false;
		s_DeviceIdle = true;
		s_DeviceLost = false;
	}

	void VulkanRenderer::NewFrame()
	{
		if (!s_Initialized)
			return;
		ImGui_ImplVulkan_NewFrame();
	}

	bool VulkanRenderer::InitImGui()
	{
		return s_Initialized && InitializeImGuiBackend();
	}

	void VulkanRenderer::ShutdownImGui()
	{
		if (s_ImGuiInitialized)
		{
			const VkResult waitResult = WaitForDeviceIdle();
			if (waitResult != VK_SUCCESS)
				CheckVulkanResult(waitResult, "waiting before shutting down the Vulkan ImGui backend");
			ImGui_ImplVulkan_Shutdown();
			s_ImGuiInitialized = false;
		}
	}

	void VulkanRenderer::Render(ImDrawData* drawData)
	{
		if (!s_Initialized || !drawData)
			return;

		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(s_Window, &width, &height);
		if (width <= 0 || height <= 0)
			return;
		if (s_MainWindow.Width != width || s_MainWindow.Height != height)
			RecreateSwapchain(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		VkSemaphore imageAcquired = s_MainWindow.FrameSemaphores[s_MainWindow.SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore renderComplete = s_MainWindow.FrameSemaphores[s_MainWindow.SemaphoreIndex].RenderCompleteSemaphore;
		VkResult result = vkAcquireNextImageKHR(s_Device, s_MainWindow.Swapchain, UINT64_MAX,
			imageAcquired, VK_NULL_HANDLE, &s_MainWindow.FrameIndex);
		const bool acquiredSuboptimal = result == VK_SUBOPTIMAL_KHR;
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
			return;
		}
		if (result != VK_SUCCESS && !acquiredSuboptimal)
		{
			CheckVulkanResult(result, "acquiring a swapchain image");
			return;
		}

		ImGui_ImplVulkanH_Frame* frame = &s_MainWindow.Frames[s_MainWindow.FrameIndex];
		if (vkWaitForFences(s_Device, 1, &frame->Fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS ||
			vkResetFences(s_Device, 1, &frame->Fence) != VK_SUCCESS ||
			vkResetCommandPool(s_Device, frame->CommandPool, 0) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to prepare a Vulkan frame.");
			return;
		}

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(frame->CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to begin Vulkan command buffer.");
			return;
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = s_MainWindow.RenderPass;
		renderPassInfo.framebuffer = frame->Framebuffer;
		renderPassInfo.renderArea.extent = {
			static_cast<uint32_t>(s_MainWindow.Width),
			static_cast<uint32_t>(s_MainWindow.Height)
		};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &s_MainWindow.ClearValue;
		s_MainWindow.ClearValue.color.float32[0] = 0.1f;
		s_MainWindow.ClearValue.color.float32[1] = 0.1f;
		s_MainWindow.ClearValue.color.float32[2] = 0.1f;
		s_MainWindow.ClearValue.color.float32[3] = 1.0f;
		vkCmdBeginRenderPass(frame->CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		ImGui_ImplVulkan_RenderDrawData(drawData, frame->CommandBuffer);
		vkCmdEndRenderPass(frame->CommandBuffer);
		if (vkEndCommandBuffer(frame->CommandBuffer) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to end Vulkan command buffer.");
			return;
		}

		const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAcquired;
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frame->CommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderComplete;
		const VkResult submitResult = vkQueueSubmit(s_Queue, 1, &submitInfo, frame->Fence);
		if (submitResult != VK_SUCCESS)
		{
			CheckVulkanResult(submitResult, "submitting a frame");
			return;
		}
		s_DeviceIdle = false;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderComplete;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &s_MainWindow.Swapchain;
		presentInfo.pImageIndices = &s_MainWindow.FrameIndex;
		result = vkQueuePresentKHR(s_Queue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || acquiredSuboptimal)
			RecreateSwapchain(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		else
			CheckVulkanResult(result, "presenting a swapchain image");
		s_MainWindow.SemaphoreIndex = (s_MainWindow.SemaphoreIndex + 1) % s_MainWindow.SemaphoreCount;
		FlushRetiredTextures();
	}

	void VulkanRenderer::SetVSync(bool enabled)
	{
		if (s_Initialized && s_VSync != enabled)
		{
			s_VSync = enabled;
			int width = 0;
			int height = 0;
			glfwGetFramebufferSize(s_Window, &width, &height);
			RecreateSwapchain(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		}
	}

	bool VulkanRenderer::IsInitialized()
	{
		return s_Initialized;
	}

	VkDevice VulkanRenderer::GetDevice()
	{
		return s_Device;
	}

	VkPhysicalDevice VulkanRenderer::GetPhysicalDevice()
	{
		return s_PhysicalDevice;
	}

	VkQueue VulkanRenderer::GetQueue()
	{
		return s_Queue;
	}

	uint32_t VulkanRenderer::GetQueueFamily()
	{
		return s_QueueFamily;
	}

	VkCommandBuffer VulkanRenderer::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = s_CommandPool;
		allocateInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		if (vkAllocateCommandBuffers(s_Device, &allocateInfo, &commandBuffer) != VK_SUCCESS)
			return VK_NULL_HANDLE;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			vkFreeCommandBuffers(s_Device, s_CommandPool, 1, &commandBuffer);
			return VK_NULL_HANDLE;
		}
		return commandBuffer;
	}

	bool VulkanRenderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			CORE_ERROR("Failed to end Vulkan upload command buffer.");
			vkFreeCommandBuffers(s_Device, s_CommandPool, 1, &commandBuffer);
			return false;
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		const VkResult result = vkQueueSubmit(s_Queue, 1, &submitInfo, VK_NULL_HANDLE);
		if (result != VK_SUCCESS)
		{
			CheckVulkanResult(result, "submitting texture upload");
			vkFreeCommandBuffers(s_Device, s_CommandPool, 1, &commandBuffer);
			return false;
		}
		s_DeviceIdle = false;
		const VkResult waitResult = vkQueueWaitIdle(s_Queue);
		if (waitResult != VK_SUCCESS)
		{
			CheckVulkanResult(waitResult, "waiting for texture upload");
		}
		else
		{
			s_DeviceIdle = true;
		}
		vkFreeCommandBuffers(s_Device, s_CommandPool, 1, &commandBuffer);
		return waitResult == VK_SUCCESS;
	}

	uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memoryProperties{};
		vkGetPhysicalDeviceMemoryProperties(s_PhysicalDevice, &memoryProperties);
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1u << i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		return UINT32_MAX;
	}

	VkDescriptorSet VulkanRenderer::AddTexture(VkSampler sampler, VkImageView imageView)
	{
		if (!s_ImGuiInitialized)
			return VK_NULL_HANDLE;
		return ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void VulkanRenderer::RemoveTexture(VkDescriptorSet descriptorSet)
	{
		if (descriptorSet == VK_NULL_HANDLE || s_Device == VK_NULL_HANDLE)
			return;
		if (s_ImGuiInitialized)
			ImGui_ImplVulkan_RemoveTexture(descriptorSet);
		else if (s_DescriptorPool != VK_NULL_HANDLE)
			vkFreeDescriptorSets(s_Device, s_DescriptorPool, 1, &descriptorSet);
	}

	void VulkanRenderer::RetireTexture(VkDescriptorSet descriptorSet, VkSampler sampler,
		VkImageView imageView, VkImage image, VkDeviceMemory imageMemory)
	{
		if (descriptorSet != VK_NULL_HANDLE || sampler != VK_NULL_HANDLE ||
			imageView != VK_NULL_HANDLE || image != VK_NULL_HANDLE || imageMemory != VK_NULL_HANDLE)
		{
			s_RetiredTextures.push_back({ descriptorSet, sampler, imageView, image, imageMemory });
		}
	}

	uint64_t VulkanRenderer::GetTextureID(VkDescriptorSet descriptorSet)
	{
		return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(descriptorSet));
	}
}

#endif
