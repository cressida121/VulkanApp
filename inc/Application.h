#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include <CVulkanCore.h>
#include <CWindow.h>

/*
The application design aims to reflect actual Vulkan
resources dependencies
*/

namespace VulkanApp {
	class CVulkanPass;
	class CVulkanPipeline;
	class CVulkanSwapchain;
	class Application : public CWindow::IEventListener {
	public:
		Application(const HWND windowHandle);
		~Application();
		bool RenderFrame();

	private:
		void OnSizeChanged(const uint32_t width, const uint32_t height) override;
		void OnClose() override;

		bool windowMinimized = false;
		bool windowClosed = false;

		CVulkanCore m_core;
		CVulkanPass *m_pPass = nullptr;
		CVulkanPipeline *m_pPipeline = nullptr;
		CVulkanSwapchain *m_pSwapchain = nullptr;

		// Window Manager - Win32 window system adapter class
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_vkSurfaceFormat;
		VkExtent2D m_vkSurfaceExtent;

		// Pipeline
		VkPipelineShaderStageCreateInfo m_shaderStageCI[2];
		VkSemaphore m_vkImgRdySem = VK_NULL_HANDLE;
		VkSemaphore m_vkRenderDoneSem[2] = { VK_NULL_HANDLE, VK_NULL_HANDLE };
		VkFence m_vkFence = VK_NULL_HANDLE;
	};
}