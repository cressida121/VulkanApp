#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include <CVulkanCore.h>

/*
The application design aims to reflect actual Vulkan
resources dependencies
*/

namespace VulkanApp {
	class CVulkanPass;
	class CVulkanPipeline;
	class Application {
	public:
		Application(const uint32_t windowWidth, const uint32_t windowHeight);
		~Application();
		void Run();
		bool RenderFrame();

	private:
		bool RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t rendererIndex = 0u);

		CVulkanCore m_core;
		CVulkanPass *m_pPass = nullptr;
		CVulkanPipeline *m_pPipeline = nullptr;
		CVulkanSwapchain *m_pSwapchain = nullptr;

		// Window Manager - Win32 window system adapter class
		void *m_mainWindowHandle = nullptr;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_vkSurfaceFormat;
		VkExtent2D m_vkSurfaceExtent;

		// Command manager
		VkCommandPool m_vkCommandPool;
		VkCommandBuffer m_vkCommandBuffer; // Destroyed along with VkCommandPool

		// Pipeline
		VkPipelineShaderStageCreateInfo m_shaderStageCI[2];
		VkSemaphore m_vkImgRdySem = VK_NULL_HANDLE;
		VkSemaphore m_vkRenderDoneSem[2] = { VK_NULL_HANDLE, VK_NULL_HANDLE };
		VkFence m_vkFence = VK_NULL_HANDLE;
	};
}