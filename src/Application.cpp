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

	m_windowWidth = capabilities.currentExtent.width;
	m_windowHeight = capabilities.currentExtent.height;
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
	
	CBufferLayout vbLayout = {
		{BufferAttribute::ShaderDataType::float3, "position"},
		{BufferAttribute::ShaderDataType::float3, "color"} 
	};

	m_pPipeline = new CVulkanPipeline(&m_core, m_pPass, m_windowWidth, m_windowHeight, m_shaderStageCI, vbLayout);
	m_pSwapchain = new CVulkanSwapchain(&m_core, m_windowWidth, m_windowHeight, m_vkSurface, m_vkSurfaceFormat, m_pPass->GetHandle());

	// Create synchronization objects

	VkSemaphoreCreateInfo semaphoreCI = {};
	semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCI = {};
	fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	m_vkRenderDoneSemVec.resize(m_pSwapchain->GetFramebufferCount());

	for (auto &fb : m_vkRenderDoneSemVec) {
		VkSemaphore semaphore = VK_NULL_HANDLE;
		if (vkCreateSemaphore(m_core.GetVkLogicalDevice(), &semaphoreCI, nullptr, &semaphore) != VK_SUCCESS) {
			throw std::runtime_error("[Runtime error] failed to create semaphores");
		}
		fb = semaphore;
	}

	if (vkCreateSemaphore(m_core.GetVkLogicalDevice(), &semaphoreCI, nullptr, &m_vkImgRdySem) != VK_SUCCESS ||
		vkCreateFence(m_core.GetVkLogicalDevice(), &fenceCI, nullptr, &m_vkFence) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] failed to create semaphores");
	}

	// Create vertex buffer
	const float vertDataRaw[] = { 
		 0.0,-1.0, 0.0,      1.0, 0.5, 0.5,
		 1.0, 1.0, 0.0,      0.1, 1.0, 0.4,
		-1.0, 1.0, 0.0,      0.0, 0.0, 1.0 };

	m_pVertexBuffer = new CVulkanBuffer(&m_core, vertDataRaw, 3 * vbLayout.GetByteSize(), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

VulkanApp::Application::~Application() {
	// Cleanup created Vulkan resources
	vkDeviceWaitIdle(m_core.GetVkLogicalDevice());
	vkDestroySemaphore(m_core.GetVkLogicalDevice(), m_vkImgRdySem, nullptr);
	for (auto &fb : m_vkRenderDoneSemVec) {
		vkDestroySemaphore(m_core.GetVkLogicalDevice(), fb, nullptr);
	}
	vkDestroyFence(m_core.GetVkLogicalDevice(), m_vkFence, nullptr);
	
	if (m_pVertexBuffer) {
		delete m_pVertexBuffer;
	}

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
	if (m_windowClosed) {
		return false;
	}
	if (m_windowMinimized) {
		return true;
	}
	vkWaitForFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence);
	try
	{
		uint32_t imgIndex = m_pSwapchain->GetNextImageIndex(m_vkImgRdySem);
		m_pPass->SubmitWorkload(
			m_core.m_vkQueue,
			m_pVertexBuffer->GetHandle(),
			m_pPipeline->GetHandle(),
			m_vkImgRdySem,
			m_vkRenderDoneSemVec[imgIndex],
			m_vkFence,
			m_pSwapchain->GetFramebuffer(imgIndex),
			{ {0,0}, {m_windowWidth, m_windowHeight} });
		m_pSwapchain->PresentFrame(imgIndex, m_vkRenderDoneSemVec[imgIndex]);
		return true;
	}
	catch (const std::exception &)
	{
		return false;
	}
}

void VulkanApp::Application::OnSizeChanged(const uint32_t width, const uint32_t height) {
	if (width == 0) {
		m_windowMinimized = true;
	}
	else {
		vkWaitForFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence, VK_TRUE, UINT64_MAX);
		m_windowWidth = width;
		m_windowHeight = height;
		m_pSwapchain->SetImageSize(width, height);
		m_pSwapchain->Update();
		m_pPipeline->m_viewport.width = static_cast<float>(width);
		m_pPipeline->m_viewport.height = static_cast<float>(height);
		m_pPipeline->m_scissorRect.extent.width = width;
		m_pPipeline->m_scissorRect.extent.height = height;
		m_pPipeline->Update();
		m_windowMinimized = false;
	}
}

void VulkanApp::Application::OnClose() {
	m_windowClosed = true;
}