#include <Application.h>
#include <CVulkanSwapchain.h>
#include <CVulkanPass.h>
#include <CVulkanPipeline.h>
#include <Utilities.h>
#include <Local.h>

#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <optional>
#include <algorithm>

#define RENDER_RES_W 640
#define RENDER_RES_H 480

// Vulkan information retrieval functions (Information available in Vulkan Caps Viewer)

std::vector<std::string> getSupportedExtenstions() {

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> propertiesList(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, propertiesList.data());

	std::vector<std::string> result;
	for (auto& properties : propertiesList)
		result.push_back(std::string(properties.extensionName));

	return result;
}

std::vector<std::string> getAvailableInstanceLayers() {

	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	std::vector<std::string> result;
	for (auto& properties : layerProperties)
		result.push_back(std::string(properties.layerName));

	return result;
}

std::vector<std::string> getAvailableDevices(const VkInstance &instance) {
	
	uint32_t devicesCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
	assert(result == VK_SUCCESS);

	std::vector<std::string> deviceList;

	if (devicesCount == 0)
		return deviceList;

	std::vector<VkPhysicalDevice> physicalDevices(devicesCount);
	result = vkEnumeratePhysicalDevices(instance, &devicesCount, physicalDevices.data());
	assert(result == VK_SUCCESS);

	for (auto& device : physicalDevices) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		std::stringstream sstream;
		std::string deviceTypeStr;

		switch (properties.deviceType) {
		case 0:
			deviceTypeStr = "VK_PHYSICAL_DEVICE_TYPE_OTHER";
			break;
		case 1:
			deviceTypeStr = "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
			break;
		case 2:
			deviceTypeStr = "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
			break;
		case 3:
			deviceTypeStr = "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
			break;
		case 4:
			deviceTypeStr = "VK_PHYSICAL_DEVICE_TYPE_CPU";
			break;
		default:
			deviceTypeStr = "VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM";
			break;
		}

		sstream << "deviceType: " << deviceTypeStr << "\n";
		sstream << "deviceName: " << properties.deviceName << "\n";

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);

		{
			sstream << "robustBufferAccess: " << std::boolalpha << bool(features.robustBufferAccess) << "\n";
			sstream << "fullDrawIndexUint32: " << std::boolalpha << bool(features.fullDrawIndexUint32) << "\n";
			sstream << "imageCubeArray: " << std::boolalpha << bool(features.imageCubeArray) << "\n";
			sstream << "independentBlend: " << std::boolalpha << bool(features.independentBlend) << "\n";
			sstream << "geometryShader: " << std::boolalpha << bool(features.geometryShader) << "\n";
			sstream << "tessellationShader: " << std::boolalpha << bool(features.tessellationShader) << "\n";
			sstream << "sampleRateShading: " << std::boolalpha << bool(features.sampleRateShading) << "\n";
			sstream << "dualSrcBlend: " << std::boolalpha << bool(features.dualSrcBlend) << "\n";
			sstream << "logicOp: " << std::boolalpha << bool(features.logicOp) << "\n";
			sstream << "multiDrawIndirect: " << std::boolalpha << bool(features.multiDrawIndirect) << "\n";
			sstream << "drawIndirectFirstInstance: " << std::boolalpha << bool(features.drawIndirectFirstInstance) << "\n";
			sstream << "depthClamp: " << std::boolalpha << bool(features.depthClamp) << "\n";
			sstream << "depthBiasClamp: " << std::boolalpha << bool(features.depthBiasClamp) << "\n";
			sstream << "fillModeNonSolid: " << std::boolalpha << bool(features.fillModeNonSolid) << "\n";
			sstream << "depthBounds: " << std::boolalpha << bool(features.depthBounds) << "\n";
			sstream << "wideLines: " << std::boolalpha << bool(features.wideLines) << "\n";
			sstream << "largePoints: " << std::boolalpha << bool(features.largePoints) << "\n";
			sstream << "alphaToOne: " << std::boolalpha << bool(features.alphaToOne) << "\n";
			sstream << "multiViewport: " << std::boolalpha << bool(features.multiViewport) << "\n";
			sstream << "samplerAnisotropy: " << std::boolalpha << bool(features.samplerAnisotropy) << "\n";
			sstream << "textureCompressionETC2: " << std::boolalpha << bool(features.textureCompressionETC2) << "\n";
			sstream << "textureCompressionASTC_LDR: " << std::boolalpha << bool(features.textureCompressionASTC_LDR) << "\n";
			sstream << "textureCompressionBC: " << std::boolalpha << bool(features.textureCompressionBC) << "\n";
			sstream << "occlusionQueryPrecise: " << std::boolalpha << bool(features.occlusionQueryPrecise) << "\n";
			sstream << "pipelineStatisticsQuery: " << std::boolalpha << bool(features.pipelineStatisticsQuery) << "\n";
			sstream << "vertexPipelineStoresAndAtomics: " << std::boolalpha << bool(features.vertexPipelineStoresAndAtomics) << "\n";
			sstream << "fragmentStoresAndAtomics: " << std::boolalpha << bool(features.fragmentStoresAndAtomics) << "\n";
			sstream << "shaderTessellationAndGeometryPointSize: " << std::boolalpha << bool(features.shaderTessellationAndGeometryPointSize) << "\n";
			sstream << "shaderImageGatherExtended: " << std::boolalpha << bool(features.shaderImageGatherExtended) << "\n";
			sstream << "shaderStorageImageExtendedFormats: " << std::boolalpha << bool(features.shaderStorageImageExtendedFormats) << "\n";
			sstream << "shaderStorageImageMultisample: " << std::boolalpha << bool(features.shaderStorageImageMultisample) << "\n";
			sstream << "shaderStorageImageReadWithoutFormat: " << std::boolalpha << bool(features.shaderStorageImageReadWithoutFormat) << "\n";
			sstream << "shaderStorageImageWriteWithoutFormat: " << std::boolalpha << bool(features.shaderStorageImageWriteWithoutFormat) << "\n";
			sstream << "shaderUniformBufferArrayDynamicIndexing: " << std::boolalpha << bool(features.shaderUniformBufferArrayDynamicIndexing) << "\n";
			sstream << "shaderSampledImageArrayDynamicIndexing: " << std::boolalpha << bool(features.shaderSampledImageArrayDynamicIndexing) << "\n";
			sstream << "shaderStorageBufferArrayDynamicIndexing: " << std::boolalpha << bool(features.shaderStorageBufferArrayDynamicIndexing) << "\n";
			sstream << "shaderStorageImageArrayDynamicIndexing: " << std::boolalpha << bool(features.shaderStorageImageArrayDynamicIndexing) << "\n";
			sstream << "shaderClipDistance: " << std::boolalpha << bool(features.shaderClipDistance) << "\n";
			sstream << "shaderCullDistance: " << std::boolalpha << bool(features.shaderCullDistance) << "\n";
			sstream << "shaderFloat64: " << std::boolalpha << bool(features.shaderFloat64) << "\n";
			sstream << "shaderInt64: " << std::boolalpha << bool(features.shaderInt64) << "\n";
			sstream << "shaderInt16: " << std::boolalpha << bool(features.shaderInt16) << "\n";
			sstream << "shaderResourceResidency: " << std::boolalpha << bool(features.shaderResourceResidency) << "\n";
			sstream << "shaderResourceMinLod: " << std::boolalpha << bool(features.shaderResourceMinLod) << "\n";
			sstream << "sparseBinding: " << std::boolalpha << bool(features.sparseBinding) << "\n";
			sstream << "sparseResidencyBuffer: " << std::boolalpha << bool(features.sparseResidencyBuffer) << "\n";
			sstream << "sparseResidencyImage2D: " << std::boolalpha << bool(features.sparseResidencyImage2D) << "\n";
			sstream << "sparseResidencyImage3D: " << std::boolalpha << bool(features.sparseResidencyImage3D) << "\n";
			sstream << "sparseResidency2Samples: " << std::boolalpha << bool(features.sparseResidency2Samples) << "\n";
			sstream << "sparseResidency4Samples: " << std::boolalpha << bool(features.sparseResidency4Samples) << "\n";
			sstream << "sparseResidency8Samples: " << std::boolalpha << bool(features.sparseResidency8Samples) << "\n";
			sstream << "sparseResidency16Samples: " << std::boolalpha << bool(features.sparseResidency16Samples) << "\n";
			sstream << "sparseResidencyAliased: " << std::boolalpha << bool(features.sparseResidencyAliased) << "\n";
			sstream << "variableMultisampleRate: " << std::boolalpha << bool(features.variableMultisampleRate) << "\n";
			sstream << "inheritedQueries: " << std::boolalpha << bool(features.inheritedQueries) << "\n";
		}

		deviceList.push_back(sstream.str());
	}

	return deviceList;

}

std::optional<uint32_t> getQueueFamilies(const VkPhysicalDevice &device, const VkQueueFlags queueFlags) {
	
	uint32_t familiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familiesCount, nullptr);

	if (familiesCount == 0)
		return std::optional<uint32_t>();

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(familiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familiesCount, queueFamilyProperties.data());

	uint32_t i = 0;
	for (auto & properties : queueFamilyProperties) {
		if (properties.queueFlags & queueFlags) {
			return i;
		}
		i++;
	}

	return std::optional<uint32_t>();
}

std::vector<std::string> getSupportedExtensions(const VkPhysicalDevice &physicalDevice) {
	
	uint32_t extensionCount = 0;
	VkResult result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	assert(result == VK_SUCCESS);

	std::vector<std::string> extensionList;

	if (!extensionCount)
		return extensionList;

	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data());
	assert(result == VK_SUCCESS);

	for (auto& itr : extensionProperties)
		extensionList.push_back(itr.extensionName);

	return extensionList;
}

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
