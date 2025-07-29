#include <CVulkanCore.h>
#include <CVulkanSwapchain.h>

#include <vector>
#include <stdexcept>
#include <optional>

#ifdef _WIN32

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
constexpr std::string_view cexp_platform_extension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

#elif MAC_OS

#define VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan.h>
constexpr std::string_view cexp_platform_extension = VK_KHR_MACOS_SURFACE_EXTENSION_NAME;

#endif

#include <Utilities.h>

static std::vector<uint32_t> GetQueueFamilyIndexList(const VkPhysicalDevice device, const VkQueueFlags queueFlags) {

	std::vector<uint32_t> result;

	uint32_t familiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familiesCount, nullptr);

	if (familiesCount == 0)
		return result;

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(familiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familiesCount, queueFamilyProperties.data());

	uint32_t i = 0;
	for (auto& properties : queueFamilyProperties) {
		if (properties.queueFlags & queueFlags) {
			result.push_back(i);
		}
		i++;
	}

	return result;
}

VulkanApp::CVulkanCore::CVulkanCore(const std::string& applicationName) : m_applicationName(applicationName) {
	
	VkResult code;
	
	// Create the instance
	if (code = InitVkInstance()) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to create a Vulkan instance", code));
	}

	// Create physical device
	if (code = vkEnumeratePhysicalDevices(m_vkInstance, &m_physicalDevicesCount, nullptr)) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to enumerate physical devices", code));
	}

	if (m_physicalDevicesCount > 0) {
		code = vkEnumeratePhysicalDevices(m_vkInstance, &m_physicalDevicesCount, &m_vkPhysicalDevices);
		if (code) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to obtain enumerated physical devices", code));
		}
	}
	else {
		throw std::runtime_error(UTIL_EXC_MSG_EX("No physical devices found", code));
	}
	
	// Find desired queue family (index)
	auto queueFamilies = GetQueueFamilyIndexList(m_vkPhysicalDevices, VK_QUEUE_GRAPHICS_BIT);

	// Check if the physical device supports presentation
	auto indexItr = std::find_if(queueFamilies.cbegin(), queueFamilies.cend(), [this](uint32_t index)->bool{
#ifdef _WIN32
		// Simplified by checking the first device only
		return vkGetPhysicalDeviceWin32PresentationSupportKHR(m_vkPhysicalDevices, index);
#elif MAC_OS
		return false;
#endif
	});

	if (indexItr == queueFamilies.cend()) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Selected physical device does not support presentation", code));
	}

	uint32_t m_queueFamilyIndex = *indexItr;

	// Declare the queue to be created
	VkDeviceQueueCreateInfo queueCI = {};
	queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCI.queueFamilyIndex = m_queueFamilyIndex;
	queueCI.queueCount = 1u;
	float priority = 1.f;
	queueCI.pQueuePriorities = &priority;

	// Create logical device
	if (code = InitVkLogicalDevice(&queueCI)) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to create a Vulkan logical device", code));
	}

	// Get command queue
	vkGetDeviceQueue(m_vkLogicalDevice, m_queueFamilyIndex, 0, &m_vkQueue);
	if (m_vkQueue == VK_NULL_HANDLE) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot retrieve the command queue", code));
	}

}

VulkanApp::CVulkanCore::~CVulkanCore() {

	if (m_vkLogicalDevice)
		vkDestroyDevice(m_vkLogicalDevice, nullptr);
		
	if (m_vkInstance)
		vkDestroyInstance(m_vkInstance, NULL);
}

static bool IsLayerAvailable(const std::string_view layerName) {

	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	std::vector<std::string> layers;
	for (auto& properties : layerProperties)
		layers.push_back(std::string(properties.layerName));

	auto itr = std::find(layers.cbegin(), layers.cend(), layerName);
	
	return itr != layers.cend();
}

VkResult VulkanApp::CVulkanCore::InitVkInstance() noexcept {

	// Fill Vulkan application descriptor
	VkApplicationInfo vkAppInfo = {};
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pApplicationName = m_applicationName.data();
	vkAppInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	vkAppInfo.pEngineName = "";
	vkAppInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	vkAppInfo.apiVersion = VK_API_VERSION_1_0;

	// Fill Vulkan instance descriptor
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &vkAppInfo;

	// Select required Vulkan extensions (check available ones using getSupportedExtenstions())
	const std::vector<const char*> vulkanExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, cexp_platform_extension.data() };
	instanceInfo.ppEnabledExtensionNames = vulkanExtensions.data();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanExtensions.size());
	
	// Enable validation layer
	const char* cp_validationLayer = "VK_LAYER_KHRONOS_validation";
	if (IsLayerAvailable(cp_validationLayer)) {
		instanceInfo.enabledLayerCount = 1u;
		instanceInfo.ppEnabledLayerNames = &cp_validationLayer;
	}
	else {
		instanceInfo.enabledLayerCount = 0u;
		instanceInfo.ppEnabledLayerNames = NULL;
	}

	// Create Vulkan instance using collected information
	// All child objects created using instance must have 
	// been destroyed prior to destroying instance
	return vkCreateInstance(&instanceInfo, nullptr, &m_vkInstance);		
}

VkResult VulkanApp::CVulkanCore::InitVkLogicalDevice(const VkDeviceQueueCreateInfo *const queueCI) noexcept
{
	// Select required device features
	VkPhysicalDeviceFeatures features = {};

	// Prepare logical device info
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = queueCI;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pEnabledFeatures = &features;
	const char* extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	deviceInfo.ppEnabledExtensionNames = &extensions;
	deviceInfo.enabledExtensionCount = 1;

	// Create logical device itself
	return vkCreateDevice(m_vkPhysicalDevices, &deviceInfo, nullptr, &m_vkLogicalDevice);
}
