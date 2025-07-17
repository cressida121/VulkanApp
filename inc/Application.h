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
	class Renderer;
	class Application {
		friend class Renderer;
	public:
		Application(const uint32_t windowWidth, const uint32_t windowHeight);
		~Application();
		void Run();
		void RenderFrame();

	private:
		bool RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t rendererIndex = 0u);

		CVulkanCore m_vkCore;
		CVulkanSwapchain *m_vkSwapchain = nullptr;
		Renderer* m_renderer = nullptr;

		// Window Manager - Win32 window system adapter class
		void* m_mainWindowHandle = nullptr;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_vkSurfaceFormat;
		VkExtent2D m_surfaceExtent;

		// Command manager
		VkCommandPool m_gfxCommandPool;
		VkCommandBuffer m_gfxCommandBuffer; // Destroyed along with VkCommandPool

		// Pipeline
		VkSemaphore m_imageAvailableSem = VK_NULL_HANDLE;
		VkSemaphore m_renderFinishedSem = VK_NULL_HANDLE;
		VkFence m_inFlightFence = VK_NULL_HANDLE;
	};
}