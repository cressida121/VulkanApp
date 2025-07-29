#include <CVulkanSwapchain.h>
#include <CVulkanCore.h>

#include <stdexcept>

#include <Utilities.h>

VulkanApp::CVulkanSwapchain::CVulkanSwapchain(
	CVulkanCore *const pParent,
	const uint32_t width,
	const uint32_t height,
	VkSurfaceKHR surface,
	VkSurfaceFormatKHR surfaceFormat) : m_pParent(pParent) {

	if (m_pParent == nullptr) {
		throw std::runtime_error(UTIL_EXC_MSG( "Pointer to parent object was null"));
	}

	VkSwapchainCreateInfoKHR swapchainCI = {};

	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = nullptr;
	swapchainCI.flags = NULL;
	swapchainCI.surface = surface;

	// Retrieve the detalis about swapchains

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pParent->GetVkPhysicalDevice(), surface, &capabilities);
	
	if (width > capabilities.maxImageExtent.width ||
		height > capabilities.maxImageExtent.height ||
		width < capabilities.minImageExtent.width ||
		height < capabilities.minImageExtent.height) {

		throw std::runtime_error(UTIL_EXC_MSG("Invalid swapchain extent"));
	}

	swapchainCI.imageExtent.width = width;
	swapchainCI.imageExtent.height = height;
	swapchainCI.minImageCount = capabilities.minImageCount + 1;
	swapchainCI.imageArrayLayers = capabilities.maxImageArrayLayers;

	// Surface formats for the physical device
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pParent->GetVkPhysicalDevice(), surface, &count, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pParent->GetVkPhysicalDevice(), surface, &count, formats.data());

	// Selecting required format and color space
	bool formatFound = false;
	for (auto& format : formats) {
		if (format.format == surfaceFormat.format &&
			format.colorSpace == surfaceFormat.colorSpace) {
			swapchainCI.imageFormat = surfaceFormat.format;
			swapchainCI.imageColorSpace = surfaceFormat.colorSpace;
			formatFound = true;
			break;
		}
	}

	if (!formatFound) {
		throw std::runtime_error(UTIL_EXC_MSG("Specified VkSurfaceFormatKHR is not supported by the device"));
	}

	swapchainCI.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = nullptr;
	swapchainCI.preTransform = capabilities.currentTransform;
	swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// Available present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_pParent->GetVkPhysicalDevice(), surface, &count, nullptr);

	std::vector<VkPresentModeKHR> presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_pParent->GetVkPhysicalDevice(), surface, &count, presentModes.data());

	bool presentModeFound = false;
	for (auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
			swapchainCI.presentMode = presentMode;
			presentModeFound = true;
			break;
		}
	}

	if (!presentModeFound) {
		throw std::runtime_error(UTIL_EXC_MSG("Specified present mode is not supported"));
	}

	swapchainCI.clipped = VK_TRUE;
	swapchainCI.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(m_pParent->GetVkLogicalDevice(), &swapchainCI, nullptr, &m_vkSwapchain);
	if(result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Swapchain creation failed", result));
	}

	// Retrieve the swap chain images
	uint32_t imageCount = 0;
	result = vkGetSwapchainImagesKHR(m_pParent->GetVkLogicalDevice(), m_vkSwapchain, &imageCount, nullptr);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to obtain swapchain images count", result));
	}

	m_swapchainImages.resize(imageCount);

	result = vkGetSwapchainImagesKHR(m_pParent->GetVkLogicalDevice(), m_vkSwapchain, &imageCount, m_swapchainImages.data());
	if(result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot retrieve swapchain images", result));
	}

	// Create the views to the swapchain images

	m_swapchainImageViews.resize(m_swapchainImages.size());

	for (uint32_t i = 0; i < m_swapchainImageViews.size(); i++) {

		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(m_pParent->GetVkLogicalDevice(), &createInfo, nullptr, m_swapchainImageViews.data() + i);
		if(result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create an image view", result));
		}

	}
}

VulkanApp::CVulkanSwapchain::~CVulkanSwapchain() {
	
	for (auto imageView : m_swapchainImageViews) {
		if (imageView) {
			vkDestroyImageView(m_pParent->GetVkLogicalDevice(), imageView, nullptr);
		}
	}

	if (m_vkSwapchain) {
		vkDestroySwapchainKHR(m_pParent->GetVkLogicalDevice(), m_vkSwapchain, nullptr);
	}

}
