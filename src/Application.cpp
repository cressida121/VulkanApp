#include <Application.h>
#include <CVulkanSwapchain.h>
#include <CVulkanPass.h>
#include <CVulkanPipeline.h>
#include <Utilities.h>
#include <Local.h>

#include <Windows.h>

#define RENDER_RES_W 640
#define RENDER_RES_H 480

static LRESULT MyWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg)
	{
	case WM_CREATE:
		// Initialize the window. 
		return 0;

	case WM_PAINT:
		// Paint the window's client area. 
		return 0;

	case WM_SIZE:
		// Set the size and position of the window. 
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// 
		// Process other messages. 
		// 

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;

}

HWND CreateSystemWindow(const std::wstring &name, const uint32_t windowWidth, const uint32_t windowHeight) {
	
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEXW wndClassExW = { 0u };
	const wchar_t windowClassName[] = L"DefaultWindowClass";
	
	wndClassExW.lpszClassName = windowClassName;
	wndClassExW.lpfnWndProc = MyWindowProc;
	wndClassExW.hInstance = hInstance;
	wndClassExW.hCursor = NULL;
	wndClassExW.hIcon = NULL;
	wndClassExW.hbrBackground = (HBRUSH)(COLOR_BTNTEXT);
	wndClassExW.lpszMenuName = NULL;
	wndClassExW.hIconSm = NULL;
	wndClassExW.style = NULL;
	wndClassExW.cbClsExtra = NULL;
	wndClassExW.cbWndExtra = NULL;
	wndClassExW.cbSize = sizeof(wndClassExW);

	RegisterClassExW(&wndClassExW);

	RECT clientArea = { 0 };
	clientArea.top = 0u;
	clientArea.left = 0u;
	clientArea.bottom = windowHeight;
	clientArea.right = windowWidth;

	DWORD windowStyle = WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

	AdjustWindowRectEx(&clientArea, windowStyle, TRUE, WS_EX_OVERLAPPEDWINDOW);

	HWND systemWindowHandle = CreateWindowExW(
		WS_EX_OVERLAPPEDWINDOW,
		windowClassName,
		name.c_str(),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		clientArea.right - clientArea.left,
		clientArea.bottom - clientArea.top + 5u,
		NULL,
		NULL,
		hInstance,
		NULL);

	return systemWindowHandle;
}


VulkanApp::Application::Application(const uint32_t windowWidth, const uint32_t windowHeight) 
	:	m_mainWindowHandle(CreateSystemWindow(L"VulkanApp", windowWidth, windowHeight)),
		m_core("VulkanApp") {

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	{
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext = nullptr;
		surfaceInfo.hinstance = GetModuleHandle(NULL);
		surfaceInfo.hwnd = static_cast<HWND>(m_mainWindowHandle);
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
	
	m_pPipeline = new CVulkanPipeline(&m_core, m_pPass, RENDER_RES_W, RENDER_RES_H, m_shaderStageCI);
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

void VulkanApp::Application::Run() {

	ShowWindow(static_cast<HWND>(m_mainWindowHandle), SW_SHOW);

	MSG msg = { 0 };
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				return;
			}
		}
		RenderFrame();
	}

}

void VulkanApp::Application::RenderFrame() {
	
	vkWaitForFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_core.GetVkLogicalDevice(), 1u, &m_vkFence);

	uint32_t imgIndex = 0u;
	VkResult result = vkAcquireNextImageKHR(
		m_core.GetVkLogicalDevice(),
		m_pSwapchain->GetHandle(),
		UINT64_MAX,
		m_vkImgRdySem,
		NULL,
		&imgIndex);

	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot acquire a swapchain image", result));
	}

	m_pPass->SubmitWorkload(
		m_core.m_vkQueue,
		m_pPipeline->GetHandle(),
		m_vkImgRdySem,
		m_vkRenderDoneSem[imgIndex],
		m_vkFence,
		m_pSwapchain->GetFramebuffer(imgIndex),
		{ {0,0}, m_vkSurfaceExtent });

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_vkRenderDoneSem[imgIndex];

	VkSwapchainKHR swapChains[] = { m_pSwapchain->GetHandle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imgIndex;

	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(m_core.m_vkQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		vkDeviceWaitIdle(m_core.GetVkLogicalDevice());
		
		VkSurfaceCapabilitiesKHR newSurfaceCaps;
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_core.GetVkPhysicalDevice(), m_vkSurface, &newSurfaceCaps);

		if (result == VK_SUCCESS) {
			m_vkSurfaceExtent = newSurfaceCaps.currentExtent;
			m_pSwapchain->SetImageSize(m_vkSurfaceExtent.width, m_vkSurfaceExtent.height);
			m_pSwapchain->Update();
		}
		else {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Swapchain recreation failure", result));
		}

	} else if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Frame presentation failed", result));
	}
}
