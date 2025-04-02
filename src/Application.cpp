#include <Application.h>
#include <Renderer.h>

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


HWND CreateSystemWindow(const std::wstring &name, const uint32_t windowWidth, const uint32_t windowHeight) {
	
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEXW wndClassExW = { 0u };
	const wchar_t windowClassName[] = L"DefaultWindowClass";
	
	wndClassExW.lpszClassName = windowClassName;
	wndClassExW.lpfnWndProc = DefWindowProcW;
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
	:	m_mainWindowHandle(CreateSystemWindow(L"VulkanApp", windowWidth, windowHeight)) {
	
	// Specify required Vulkan extensions and layers
	std::vector<std::string> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
	std::vector<std::string> layers = { "VK_LAYER_KHRONOS_validation" };

	// Create the Vulkan instance (library initialization)
	m_vkInstance = CreateVkInstance("VulkanApp", extensions, layers);
	if(m_vkInstance == VK_NULL_HANDLE) {
		throw std::runtime_error("[Runtime error] Failed to create vkInstance");
	}

	// Create the Win32 surface
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	{
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext = nullptr;
		surfaceInfo.hinstance = GetModuleHandle(NULL);
		surfaceInfo.hwnd = static_cast<HWND>(m_mainWindowHandle);
	}

	if (vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceInfo, nullptr, &m_vkSurface) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot create Win32SurfaceKHR");
	}

	// Create physical device
	uint32_t devicesCount = 0;
	if (vkEnumeratePhysicalDevices(m_vkInstance, &devicesCount, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot enumerate physical devices");
	}

	if (devicesCount == 1) {
		vkEnumeratePhysicalDevices(m_vkInstance, &devicesCount, &m_vkPhysicalDevice);
	}
	else {
		throw std::runtime_error("[Runtime error] Multiple physical devices are not supported");
	}

	// Create logical device
	m_vkLogicalDevice = CreateVkLogicalDevice(m_vkPhysicalDevice, m_vkGraphicsQueue, VK_QUEUE_GRAPHICS_BIT);

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, m_vkSurface, &capabilities);

	m_swapchainExtent = capabilities.currentExtent;
	m_vkSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
	m_vkSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	// Create the swapchain
	m_vkSwapchain = CreateVkSwapchain(m_vkPhysicalDevice, m_vkLogicalDevice, m_vkSurface, capabilities, m_vkSurfaceFormat);

	// Retrieve the swap chain images
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(m_vkLogicalDevice, m_vkSwapchain, &imageCount, nullptr);

	m_swapchainImages.resize(imageCount);

	if (vkGetSwapchainImagesKHR(m_vkLogicalDevice, m_vkSwapchain, &imageCount, m_swapchainImages.data()) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot retrieve swapchain images");
	}

	// Create the views to the swapchain images
	m_scImageViews = CreateImageViews(m_vkLogicalDevice, m_swapchainImages, m_vkSurfaceFormat.format);

	new Renderer(this);


	// Create command pool
	std::optional<uint32_t> qfIndex = getQueueFamilies(m_vkPhysicalDevice, VK_QUEUE_GRAPHICS_BIT);
	
	VkCommandPoolCreateInfo commandPoolCI = {};
	commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCI.queueFamilyIndex = qfIndex.value();

	if (vkCreateCommandPool(m_vkLogicalDevice, &commandPoolCI, nullptr, &m_gfxCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Cannot create a command pool");
	}

	// Create command buffer
	VkCommandBufferAllocateInfo commandBufferCI = {};
	commandBufferCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferCI.commandPool = m_gfxCommandPool;
	commandBufferCI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferCI.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_vkLogicalDevice, &commandBufferCI, &m_gfxCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create a command pool");
	}

	VkCommandBufferBeginInfo beginInfoCI = {};
	beginInfoCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfoCI.flags = 0;
	beginInfoCI.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(m_gfxCommandBuffer, &beginInfoCI) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to begin a command buffer");
	}


	VkRenderPassBeginInfo renderPassCI = {};
	renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassCI.renderPass = m_renderers.at(0)->GetRenderPass();
	renderPassCI.framebuffer = m_renderers.at(0)->GetFramebuffer(0);
	renderPassCI.renderArea.offset = { 0, 0 };
	renderPassCI.renderArea.extent = m_swapchainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassCI.clearValueCount = 1;
	renderPassCI.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_gfxCommandBuffer, &renderPassCI, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(m_gfxCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderers.at(0)->GetPipeline());
	vkCmdDraw(m_gfxCommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(m_gfxCommandBuffer);

	if (vkEndCommandBuffer(m_gfxCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to end a command buffer");
	}
}

VulkanApp::Application::~Application() {
	
	vkDestroyCommandPool(m_vkLogicalDevice, m_gfxCommandPool, nullptr);

	for (auto& renderer : m_renderers) {
		delete renderer;
	}

	// Cleanup created Vulkan resources
	vkDestroyDevice(m_vkLogicalDevice, nullptr);
	vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
	vkDestroyInstance(m_vkInstance, nullptr);
	
	for (auto& imageView : m_scImageViews) {
		vkDestroyImageView(m_vkLogicalDevice, imageView, nullptr);
	}
}

VkInstance VulkanApp::Application::CreateVkInstance(
	const std::string &appName, 
	const std::vector<std::string> &extensions,
	const std::vector<std::string> &layers) {

	// Fill Vulkan application descriptor
	VkApplicationInfo vkAppInfo = {};
	{
		vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vkAppInfo.pApplicationName = appName.data();
		vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		vkAppInfo.pEngineName = "";
		vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		vkAppInfo.apiVersion = VK_API_VERSION_1_0;
	}

	// Fill Vulkan instance descriptor
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &vkAppInfo;
	
	// Select required Vulkan extensions (check available ones using getSupportedExtenstions())
	std::vector<const char*> extensionNames;
	for (auto& extension : extensions)
		extensionNames.push_back(extension.data());

	instanceInfo.enabledExtensionCount = extensionNames.size();
	instanceInfo.ppEnabledExtensionNames = extensionNames.data();

	// Select required Vulkan layers (check available ones using getAvailableInstanceLayers())
	std::vector<const char*> layerNames;
	for (auto& layer : layers)
		layerNames.push_back(layer.data());

	instanceInfo.enabledLayerCount = layerNames.size();
	instanceInfo.ppEnabledLayerNames = layerNames.data();
	
	VkInstance vkInstance = nullptr;

	// Create Vulkan instance using collected information
	if (vkCreateInstance(&instanceInfo, nullptr, &vkInstance) != VK_SUCCESS)
		return nullptr;

	return vkInstance;
}

VkDevice VulkanApp::Application::CreateVkLogicalDevice(VkPhysicalDevice physicalDevice, VkQueue &outGraphicsQueue, VkQueueFlags queueFlags)
{
	// Get index of the required queue family
	std::optional<uint32_t> qfIndex = getQueueFamilies(physicalDevice, queueFlags);

	if (!qfIndex.has_value()) {
		throw std::runtime_error("[Runtime error] Required queue family not found");
	}

	VkBool32 presentationSupport = 0;
	presentationSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, qfIndex.value());

	if (!presentationSupport) {
		throw std::runtime_error("[Runtime error] Presentation mode not supported");
	}

	// Prepare the device queue info
	VkDeviceQueueCreateInfo queueInfo = {};
	{
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = qfIndex.value();
		queueInfo.queueCount = 1;
		float priority = 1.0f;
		queueInfo.pQueuePriorities = &priority;
	}

	// Select required device features
	VkPhysicalDeviceFeatures features = {};

	// Prepare logical device info
	VkDeviceCreateInfo deviceInfo = {};
	{
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pQueueCreateInfos = &queueInfo;
		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.pEnabledFeatures = &features;
		const char* extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		deviceInfo.ppEnabledExtensionNames = &extensions;
		deviceInfo.enabledExtensionCount = 1;
	}

	// Create logical device itself
	VkDevice logicalDevice = VK_NULL_HANDLE;
	
	if (vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create a logical device");
	}
	
	VkQueue graphicsQueue = VK_NULL_HANDLE;

	// Retrieve the queue handle
	vkGetDeviceQueue(logicalDevice, qfIndex.value(), 0, &graphicsQueue);
	if (graphicsQueue == VK_NULL_HANDLE) {
		throw std::runtime_error("[Runtime error] Cannot obtain a handle to the queue");
	}
	
	outGraphicsQueue = graphicsQueue;

	return logicalDevice;
}

VkSwapchainKHR VulkanApp::Application::CreateVkSwapchain(
	VkPhysicalDevice physicalDevice, 
	VkDevice logicalDevice,
	VkSurfaceKHR surface,
	VkSurfaceCapabilitiesKHR surfaceCapabilities,
	VkSurfaceFormatKHR requiredFormat)
{
	VkSwapchainCreateInfoKHR swapchainInfo = {};

	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.pNext = nullptr;
	swapchainInfo.flags = NULL;
	swapchainInfo.surface = surface;

	// Retrieve the detalis about swapchains

	swapchainInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
	swapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
	swapchainInfo.imageArrayLayers = surfaceCapabilities.maxImageArrayLayers;

	// Surface formats for the physical device
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());

	// Selecting required format and color space
	bool formatFound = false;
	for (auto& format : formats) {
		if (format.format == requiredFormat.format &&
			format.colorSpace == requiredFormat.colorSpace) {
			swapchainInfo.imageFormat = requiredFormat.format;
			swapchainInfo.imageColorSpace = requiredFormat.colorSpace;
			formatFound = true;
			break;
		}
	}

	if (!formatFound) {
		throw std::runtime_error("[Runtime error] Specified VkSurfaceFormatKHR is not supported by the device");
	}

	swapchainInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = nullptr;
	swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// Available present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);

	std::vector<VkPresentModeKHR> presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, presentModes.data());

	bool presentModeFound = false;
	for (auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
			swapchainInfo.presentMode = presentMode;
			presentModeFound = true;
			break;
		}
	}

	if (!presentModeFound) {
		throw std::runtime_error("[Runtime error] Specified present mode not supported");
	}

	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(logicalDevice, &swapchainInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Swapchain creation failed");
	}
	
	return swapchain;
}

std::vector<VkImageView> VulkanApp::Application::CreateImageViews(VkDevice logicalDevice, const std::vector<VkImage>& images, VkFormat format)
{
	std::vector<VkImageView> imageViews;

	imageViews.resize(images.size());

	for (uint32_t i = 0; i < images.size(); i++) {

		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &createInfo, nullptr, imageViews.data() + i) != VK_SUCCESS) {
			throw std::runtime_error("[Runtime error] Cannot create an image view");
		}

	}

	return imageViews;
}


void VulkanApp::Application::Run() {

	ShowWindow(static_cast<HWND>(m_mainWindowHandle), SW_SHOW);

	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

}
