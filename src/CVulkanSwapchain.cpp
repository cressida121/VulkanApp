#include <CVulkanSwapchain.h>
#include <CVulkanCore.h>

#include <stdexcept>

#include <Utilities.h>

VulkanApp::CVulkanSwapchain::CVulkanSwapchain(
	CVulkanCore *const pParent,
	const uint32_t width,
	const uint32_t height,
	VkSurfaceKHR surface,
	VkSurfaceFormatKHR surfaceFormat,
	VkRenderPass renderPass) : m_pParent(pParent) , m_vkSurfaceFormat(surfaceFormat), m_width(width), m_height(height), m_vkRenderPass(renderPass), m_vkSurface(surface) {

	if (m_pParent == nullptr) {
		throw std::runtime_error(UTIL_EXC_MSG( "Pointer to parent object was null"));
	}

	m_swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	m_swapchainCI.pNext = nullptr;
	m_swapchainCI.flags = NULL;
	m_swapchainCI.surface = m_vkSurface;

	// Retrieve the detalis about swapchains

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pParent->GetVkPhysicalDevice(), m_vkSurface, &capabilities);
	
	if (m_width > capabilities.maxImageExtent.width ||
		m_height > capabilities.maxImageExtent.height ||
		m_width < capabilities.minImageExtent.width ||
		m_height < capabilities.minImageExtent.height) {

		throw std::runtime_error(UTIL_EXC_MSG("Invalid swapchain extent"));
	}

	m_swapchainCI.imageExtent.width = m_width;
	m_swapchainCI.imageExtent.height = m_height;
	m_swapchainCI.minImageCount = capabilities.minImageCount + 1;
	m_swapchainCI.imageArrayLayers = capabilities.maxImageArrayLayers;

	if (!SurfaceFormatAvailable(m_vkSurfaceFormat)) {
		throw std::runtime_error(UTIL_EXC_MSG("Specified VkSurfaceFormatKHR is not supported by the device"));
	}
	m_swapchainCI.imageFormat = m_vkSurfaceFormat.format;
	m_swapchainCI.imageColorSpace = m_vkSurfaceFormat.colorSpace;

	m_swapchainCI.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	m_swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_swapchainCI.queueFamilyIndexCount = 0;
	m_swapchainCI.pQueueFamilyIndices = nullptr;
	m_swapchainCI.preTransform = capabilities.currentTransform;
	m_swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	if (!PresentModeAvailable(VK_PRESENT_MODE_FIFO_KHR)) {
		throw std::runtime_error(UTIL_EXC_MSG("Specified present mode is not supported"));
	}
	m_swapchainCI.presentMode = VK_PRESENT_MODE_FIFO_KHR;

	m_swapchainCI.clipped = VK_TRUE;
	m_swapchainCI.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(m_pParent->GetVkLogicalDevice(), &m_swapchainCI, nullptr, &m_vkSwapchain);
	if(result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Swapchain creation failed", result));
	}

	InitializeFramebuffer();
}

VulkanApp::CVulkanSwapchain::~CVulkanSwapchain() {
	
	for (auto framebuffer : m_framebuffers) {
		if (framebuffer) {
			vkDestroyFramebuffer(m_pParent->GetVkLogicalDevice(), framebuffer, nullptr);
		}
	}

	for (auto imageView : m_swapchainImageViews) {
		if (imageView) {
			vkDestroyImageView(m_pParent->GetVkLogicalDevice(), imageView, nullptr);
		}
	}

	if (m_vkSwapchain) {
		vkDestroySwapchainKHR(m_pParent->GetVkLogicalDevice(), m_vkSwapchain, nullptr);
	}

}

const VkFramebuffer VulkanApp::CVulkanSwapchain::GetFramebuffer(const uint32_t index) {
	if (index >= 0 && index < m_framebuffers.size()) {
		return m_framebuffers[index];
	}
	return VK_NULL_HANDLE;
}

void VulkanApp::CVulkanSwapchain::InitializeFramebuffer() {

	// Retrieve the swap chain images
	uint32_t imageCount = 0;
	VkResult result = vkGetSwapchainImagesKHR(m_pParent->GetVkLogicalDevice(), m_vkSwapchain, &imageCount, nullptr);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to obtain swapchain images count", result));
	}

	std::vector<VkImage> swapchainImages(imageCount);

	result = vkGetSwapchainImagesKHR(m_pParent->GetVkLogicalDevice(), m_vkSwapchain, &imageCount, swapchainImages.data());
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot retrieve swapchain images", result));
	}

	// Create the views to the swapchain images

	m_swapchainImageViews.resize(swapchainImages.size());

	VkImageViewCreateInfo imageViewCI = {};
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCI.format = m_vkSurfaceFormat.format;
	imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = 1;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = 1;

	for (uint32_t i = 0; i < m_swapchainImageViews.size(); i++) {
		imageViewCI.image = swapchainImages[i];
		result = vkCreateImageView(m_pParent->GetVkLogicalDevice(), &imageViewCI, nullptr, m_swapchainImageViews.data() + i);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create an image view", result));
		}
	}

	// Create framebuffers

	m_framebuffers.resize(m_swapchainImageViews.size());
	uint32_t i = 0;

	VkFramebufferCreateInfo framebufferCI = {};
	framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCI.attachmentCount = 1;
	framebufferCI.renderPass = m_vkRenderPass;
	framebufferCI.width = m_width;
	framebufferCI.height = m_height;
	framebufferCI.layers = 1;

	for (auto imageView : m_swapchainImageViews) {
		framebufferCI.pAttachments = &imageView;
		if (vkCreateFramebuffer(m_pParent->GetVkLogicalDevice(), &framebufferCI, nullptr, &m_framebuffers[i++]) != VK_SUCCESS) {
			throw std::runtime_error("[Runtime error] Failed to create framebuffer");
		}
	}
}

bool VulkanApp::CVulkanSwapchain::PresentModeAvailable(VkPresentModeKHR mode) const {
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_pParent->GetVkPhysicalDevice(), m_vkSurface, &count, nullptr);

	std::vector<VkPresentModeKHR> presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_pParent->GetVkPhysicalDevice(), m_vkSurface, &count, presentModes.data());

	return std::find(presentModes.cbegin(), presentModes.cend(), mode) != presentModes.cend();
}

bool VulkanApp::CVulkanSwapchain::SurfaceFormatAvailable(VkSurfaceFormatKHR surfaceFormat) const {
	// Surface formats for the physical device
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pParent->GetVkPhysicalDevice(), m_vkSurface, &count, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pParent->GetVkPhysicalDevice(), m_vkSurface, &count, formats.data());

	// Selecting required format and color space
	bool formatFound = false;
	for (auto& format : formats) {
		if (format.format == surfaceFormat.format &&
			format.colorSpace == surfaceFormat.colorSpace) {
			return true;
		}
	}
	return false;
}
