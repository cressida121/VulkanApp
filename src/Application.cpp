#include <Application.h>
#include <CVulkanSwapchain.h>
#include <CVulkanPass.h>
#include <CVulkanPipeline.h>
#include <Utilities.h>
#include <Local.h>

#include <Windows.h>

VulkanApp::Application::Application(const HWND windowHandle) :
		m_core("VulkanApp") {

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	{
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext = nullptr;
		surfaceInfo.hinstance = GetModuleHandle(NULL);
		surfaceInfo.hwnd = windowHandle;
	}

	if (vkCreateWin32SurfaceKHR(m_core.GetVkInstance(), &surfaceInfo, nullptr, &m_vkSurface) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot create Win32SurfaceKHR");
	}

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_core.GetVkPhysicalDevice(), m_vkSurface, &capabilities);

	m_vkSurfaceExtent = capabilities.currentExtent;
	m_vkSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
	m_vkSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	
	m_pPass = new CVulkanPass(&m_core, m_vkSurfaceFormat.format);
	
	// Initialize shaders
	m_shaderStageCI[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_shaderStageCI[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	m_shaderStageCI[0].module = CVulkanPipeline::LoadCompiledShader(m_core.GetVkLogicalDevice(), VERTEX_SHADER_PATH);
	m_shaderStageCI[0].pName = "main";

	m_shaderStageCI[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_shaderStageCI[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	m_shaderStageCI[1].module = CVulkanPipeline::LoadCompiledShader(m_core.GetVkLogicalDevice(), FRAGMENT_SHADER_PATH);
	m_shaderStageCI[1].pName = "main";
	
	m_pPipeline = new CVulkanPipeline(&m_core, m_pPass, capabilities.currentExtent.width, capabilities.currentExtent.height, m_shaderStageCI);
	m_pSwapchain = new CVulkanSwapchain(&m_core, capabilities.currentExtent.width, capabilities.currentExtent.height, m_vkSurface, m_vkSurfaceFormat, m_pPass->GetHandle());

	// Create synchronization objects

	VkSemaphoreCreateInfo semaphoreCI = {};
	semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCI = {};
	fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(m_core.GetVkLogicalDevice(), &semaphoreCI, nullptr, &m_vkImgRdySem) != VK_SUCCESS ||
		vkCreateSemaphore(m_core.GetVkLogicalDevice(), &semaphoreCI, nullptr, &m_vkRenderDoneSem[0]) != VK_SUCCESS ||
		vkCreateSemaphore(m_core.GetVkLogicalDevice(), &semaphoreCI, nullptr, &m_vkRenderDoneSem[1]) != VK_SUCCESS ||
		vkCreateFence(m_core.GetVkLogicalDevice(), &fenceCI, nullptr, &m_vkFence) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] failed to create semaphores");
	}
}

VulkanApp::Application::~Application() {
	// Cleanup created Vulkan resources
	vkDeviceWaitIdle(m_core.GetVkLogicalDevice());
	vkDestroySemaphore(m_core.GetVkLogicalDevice(), m_vkImgRdySem, nullptr);
	vkDestroySemaphore(m_core.GetVkLogicalDevice(), m_vkRenderDoneSem[0], nullptr);
	vkDestroySemaphore(m_core.GetVkLogicalDevice(), m_vkRenderDoneSem[1], nullptr);
	vkDestroyFence(m_core.GetVkLogicalDevice(), m_vkFence, nullptr);

	if (m_pSwapchain) {
		delete m_pSwapchain;
	}

	if (m_pPipeline) {
		delete m_pPipeline;
	}

	if (m_pPass) {
		delete m_pPass;
	}

	vkDestroyShaderModule(m_core.GetVkLogicalDevice(), m_shaderStageCI[0].module, nullptr);
	vkDestroyShaderModule(m_core.GetVkLogicalDevice(), m_shaderStageCI[1].module, nullptr);
	vkDestroySurfaceKHR(m_core.GetVkInstance(), m_vkSurface, nullptr);
}

bool VulkanApp::Application::RenderFrame() {
	if (windowClosed) {
		return false;
	}
	if (windowMinimized) {
		return true;
	}
	vkWaitForFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence);
	try
	{
		uint32_t imgIndex = m_pSwapchain->GetNextImageIndex(m_vkImgRdySem);
		m_pPass->SubmitWorkload(
			m_core.m_vkQueue,
			m_pPipeline->GetHandle(),
			m_vkImgRdySem,
			m_vkRenderDoneSem[imgIndex],
			m_vkFence,
			m_pSwapchain->GetFramebuffer(imgIndex),
			{ {0,0}, m_vkSurfaceExtent });
		m_pSwapchain->PresentFrame(imgIndex, m_vkRenderDoneSem[imgIndex]);
		return true;
	}
	catch (const std::exception &)
	{
		return false;
	}
}

void VulkanApp::Application::OnSizeChanged(const uint32_t width, const uint32_t height) {
	if (width == 0) {
		windowMinimized = true;
	}
	else {
		vkWaitForFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence, VK_TRUE, UINT64_MAX);
		m_vkSurfaceExtent.width = width;
		m_vkSurfaceExtent.height = height;
		m_pSwapchain->SetImageSize(width, height);
		m_pSwapchain->Update();
		windowMinimized = false;
	}
}

void VulkanApp::Application::OnClose() {
	windowClosed = true;
}