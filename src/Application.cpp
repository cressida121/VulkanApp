#include <Application.h>
#include <Renderer.h>
#include <CVulkanSwapchain.h>
#include <Utilities.h>

#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <optional>
#include <algorithm>


// Vulkan information retrieval functions

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
		m_vkCore("VulkanApp") {

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	{
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext = nullptr;
		surfaceInfo.hinstance = GetModuleHandle(NULL);
		surfaceInfo.hwnd = static_cast<HWND>(m_mainWindowHandle);
	}

	if (vkCreateWin32SurfaceKHR(m_vkCore.GetVkInstance(), &surfaceInfo, nullptr, &m_vkSurface) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot create Win32SurfaceKHR");
	}

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkCore.GetVkPhysicalDevice(), m_vkSurface, &capabilities);

	m_surfaceExtent = capabilities.currentExtent;
	m_vkSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
	m_vkSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	
	m_vkSwapchain = new CVulkanSwapchain(&m_vkCore, capabilities.currentExtent.width, capabilities.currentExtent.height, m_vkSurface, m_vkSurfaceFormat);

	m_renderer = new Renderer(this, capabilities.currentExtent.width, capabilities.currentExtent.height);
	m_renderer->SetupFramebuffers(m_vkSwapchain->GetImageViews());

	// Create command pool
	std::optional<uint32_t> qfIndex = getQueueFamilies(m_vkCore.GetVkPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);
	
	VkCommandPoolCreateInfo commandPoolCI = {};
	commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCI.queueFamilyIndex = qfIndex.value();

	if (vkCreateCommandPool(m_vkCore.GetVkLogicalDevice(), &commandPoolCI, nullptr, &m_gfxCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot create a command pool");
	}

	// Create command buffer
	VkCommandBufferAllocateInfo commandBufferCI = {};
	commandBufferCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferCI.commandPool = m_gfxCommandPool;
	commandBufferCI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferCI.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_vkCore.GetVkLogicalDevice(), &commandBufferCI, &m_gfxCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create a command pool");
	}

	// Create synchronization objects

	VkSemaphoreCreateInfo semaphoreCI = {};
	semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCI = {};
	fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(m_vkCore.GetVkLogicalDevice(), &semaphoreCI, nullptr, &m_imageAvailableSem) != VK_SUCCESS ||
		vkCreateSemaphore(m_vkCore.GetVkLogicalDevice(), &semaphoreCI, nullptr, &m_renderFinishedSem) != VK_SUCCESS ||
		vkCreateFence(m_vkCore.GetVkLogicalDevice(), &fenceCI, nullptr, &m_inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] failed to create semaphores");
	}
}

VulkanApp::Application::~Application() {
	// Cleanup created Vulkan resources
	vkDeviceWaitIdle(m_vkCore.GetVkLogicalDevice());
	vkDestroySemaphore(m_vkCore.GetVkLogicalDevice(), m_imageAvailableSem, nullptr);
	vkDestroySemaphore(m_vkCore.GetVkLogicalDevice(), m_renderFinishedSem, nullptr);
	vkDestroyFence(m_vkCore.GetVkLogicalDevice(), m_inFlightFence, nullptr);
	vkDestroyCommandPool(m_vkCore.GetVkLogicalDevice(), m_gfxCommandPool, nullptr);
	delete m_renderer;
	delete m_vkSwapchain;
	vkDestroySurfaceKHR(m_vkCore.GetVkInstance(), m_vkSurface, nullptr);
}

bool VulkanApp::Application::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t rendererIndex) {
	

	VkCommandBufferBeginInfo beginInfoCI = {};
	beginInfoCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfoCI.flags = 0;
	beginInfoCI.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfoCI) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to begin a command buffer");
	}

	VkRenderPassBeginInfo renderPassCI = {};
	renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassCI.renderPass = m_renderer->GetRenderPass();
	renderPassCI.framebuffer = m_renderer->GetFramebuffer(imageIndex);
	renderPassCI.renderArea.offset = { 0, 0 };
	renderPassCI.renderArea.extent = m_surfaceExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassCI.clearValueCount = 1;
	renderPassCI.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassCI, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderer->GetPipeline());
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to end a command buffer");
	}

	return true;
}


void VulkanApp::Application::Run() {

	ShowWindow(static_cast<HWND>(m_mainWindowHandle), SW_SHOW);

	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		RenderFrame();
	}
}

void VulkanApp::Application::RenderFrame() {
	
	vkWaitForFences(m_vkCore.GetVkLogicalDevice(), 1u, &m_inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_vkCore.GetVkLogicalDevice(), 1u, &m_inFlightFence);

	uint32_t imgIndex = 0u;
	VkResult result = vkAcquireNextImageKHR(
		m_vkCore.GetVkLogicalDevice(), 
		m_vkSwapchain->GetSwapchain(),
		UINT64_MAX,
		m_imageAvailableSem,
		NULL,
		&imgIndex);

	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot acquire a swapchain image", result));
	}

	vkResetCommandBuffer(m_gfxCommandBuffer, 0);
	RecordCommandBuffer(m_gfxCommandBuffer, imgIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_imageAvailableSem; // Semaphore will be signaled by vkAcquireNextImage 
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_gfxCommandBuffer;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_renderFinishedSem;

	result = vkQueueSubmit(m_vkCore.m_vkQueue, 1, &submitInfo, m_inFlightFence);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Queue submit failed", result));
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_renderFinishedSem;

	VkSwapchainKHR swapChains[] = { m_vkSwapchain->GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imgIndex;

	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(m_vkCore.m_vkQueue, &presentInfo);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Frame presentation failed", result));
	}

}
