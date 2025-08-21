#include <CVulkanSwapchain.h>
#include <CVulkanCore.h>

#include <stdexcept>

#include <Utilities.h>

VulkanApp::CVulkanSwapchain::CVulkanSwapchain(
	const CVulkanCore *const pCore,
	const uint32_t width,
	const uint32_t height,
	const VkSurfaceKHR surface,
	const VkSurfaceFormatKHR surfaceFormat,
	const VkRenderPass renderPass) : m_pCore(pCore) , m_vkRenderPass(renderPass) {

	if (m_pCore == nullptr) {
		throw std::runtime_error(UTIL_EXC_MSG( "Pointer to parent object was null"));
	}

	m_swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	m_swapchainCI.pNext = nullptr;
	m_swapchainCI.flags = NULL;
	m_swapchainCI.surface = surface;

	// Retrieve the detalis about swapchains
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pCore->GetVkPhysicalDevice(), surface, &capabilities);

	m_swapchainCI.minImageCount = capabilities.minImageCount + 1;
	m_swapchainCI.imageArrayLayers = capabilities.maxImageArrayLayers;
	
	if (!SetImageSize(width, height)) {
		throw std::runtime_error(UTIL_EXC_MSG("Invalid swapchain extent"));
	}

	if (!SetImageFormat(surfaceFormat)) {
		throw std::runtime_error(UTIL_EXC_MSG("Specified VkSurfaceFormatKHR is not supported by the device"));
	}

	m_swapchainCI.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	m_swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_swapchainCI.queueFamilyIndexCount = 0;
	m_swapchainCI.pQueueFamilyIndices = nullptr;
	m_swapchainCI.preTransform = capabilities.currentTransform;
	m_swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	if (!SetPresentMode(VK_PRESENT_MODE_FIFO_KHR)) {
		throw std::runtime_error(UTIL_EXC_MSG("Specified present mode is not supported"));
	}

	m_swapchainCI.clipped = VK_TRUE;
	m_swapchainCI.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(m_pCore->GetVkLogicalDevice(), &m_swapchainCI, nullptr, &m_vkSwapchain);
	if(result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Swapchain creation failed", result));
	}

	InitializeFramebuffer();
}

VulkanApp::CVulkanSwapchain::~CVulkanSwapchain() {
	ReleaseFramebuffer();
	if (m_vkSwapchain) {
		vkDestroySwapchainKHR(m_pCore->GetVkLogicalDevice(), m_vkSwapchain, nullptr);
		m_vkSwapchain = VK_NULL_HANDLE;
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
	VkResult result = vkGetSwapchainImagesKHR(m_pCore->GetVkLogicalDevice(), m_vkSwapchain, &imageCount, nullptr);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to obtain swapchain images count", result));
	}

	std::vector<VkImage> swapchainImages(imageCount);

	result = vkGetSwapchainImagesKHR(m_pCore->GetVkLogicalDevice(), m_vkSwapchain, &imageCount, swapchainImages.data());
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot retrieve swapchain images", result));
	}

	// Create the views to the swapchain images

	m_swapchainImageViews.resize(swapchainImages.size());

	VkImageViewCreateInfo imageViewCI = {};
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCI.format = m_swapchainCI.imageFormat;
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
		result = vkCreateImageView(m_pCore->GetVkLogicalDevice(), &imageViewCI, nullptr, m_swapchainImageViews.data() + i);
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
	framebufferCI.width = m_swapchainCI.imageExtent.width;
	framebufferCI.height = m_swapchainCI.imageExtent.height;
	framebufferCI.layers = 1;

	for (auto imageView : m_swapchainImageViews) {
		framebufferCI.pAttachments = &imageView;
		if (vkCreateFramebuffer(m_pCore->GetVkLogicalDevice(), &framebufferCI, nullptr, &m_framebuffers[i++]) != VK_SUCCESS) {
			throw std::runtime_error("[Runtime error] Failed to create framebuffer");
		}
	}
}

void VulkanApp::CVulkanSwapchain::ReleaseFramebuffer() {

	for (auto framebuffer : m_framebuffers) {
		if (framebuffer) {
			vkDestroyFramebuffer(m_pCore->GetVkLogicalDevice(), framebuffer, nullptr);
		}
	}

	m_framebuffers.clear();

	for (auto imageView : m_swapchainImageViews) {
		if (imageView) {
			vkDestroyImageView(m_pCore->GetVkLogicalDevice(), imageView, nullptr);
		}
	}

	m_swapchainImageViews.clear();
}

bool VulkanApp::CVulkanSwapchain::PresentModeAvailable(const VkPresentModeKHR mode) const {
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_pCore->GetVkPhysicalDevice(), m_swapchainCI.surface, &count, nullptr);

	std::vector<VkPresentModeKHR> presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_pCore->GetVkPhysicalDevice(), m_swapchainCI.surface, &count, presentModes.data());

	return std::find(presentModes.cbegin(), presentModes.cend(), mode) != presentModes.cend();
}

bool VulkanApp::CVulkanSwapchain::SurfaceFormatAvailable(const VkSurfaceFormatKHR surfaceFormat) const {
	// Surface formats for the physical device
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pCore->GetVkPhysicalDevice(), m_swapchainCI.surface, &count, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_pCore->GetVkPhysicalDevice(), m_swapchainCI.surface, &count, formats.data());

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

void VulkanApp::CVulkanSwapchain::Update() {

	ReleaseFramebuffer();

	m_swapchainCI.oldSwapchain = m_vkSwapchain;

	VkResult result = vkCreateSwapchainKHR(m_pCore->GetVkLogicalDevice(), &m_swapchainCI, nullptr, &m_vkSwapchain);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Swapchain creation failed", result));
	}

	vkDestroySwapchainKHR(m_pCore->GetVkLogicalDevice(), m_swapchainCI.oldSwapchain, nullptr);

	InitializeFramebuffer();
}

bool VulkanApp::CVulkanSwapchain::SetPresentMode(const VkPresentModeKHR mode) {
	if (!PresentModeAvailable(mode)) {
		return false;
	}
	m_swapchainCI.presentMode = mode;
	return true;
}

bool VulkanApp::CVulkanSwapchain::SetImageFormat(const VkSurfaceFormatKHR surfaceFormat)
{
	if (!SurfaceFormatAvailable(surfaceFormat)) {
		return false;
	}
	m_swapchainCI.imageFormat = surfaceFormat.format;
	m_swapchainCI.imageColorSpace = surfaceFormat.colorSpace;
	return true;
}

bool VulkanApp::CVulkanSwapchain::SetImageSize(const uint32_t width, const uint32_t height)
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pCore->GetVkPhysicalDevice(), m_swapchainCI.surface, &capabilities);

	if (width > capabilities.maxImageExtent.width ||
		height > capabilities.maxImageExtent.height ||
		width < capabilities.minImageExtent.width ||
		height < capabilities.minImageExtent.height) {

		return false;
	}

	m_swapchainCI.imageExtent.width = width;
	m_swapchainCI.imageExtent.height = height;
	return true;
}

uint32_t VulkanApp::CVulkanSwapchain::GetNextImageIndex(VkSemaphore signalImgReady) const {
	uint32_t index = 0u;
	VkResult result = vkAcquireNextImageKHR(
		m_pCore->GetVkLogicalDevice(),
		m_vkSwapchain,
		UINT64_MAX,
		signalImgReady,
		NULL,
		&index);

	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot acquire a swapchain image", result));
	}

	return index;
}

void VulkanApp::CVulkanSwapchain::PresentFrame(uint32_t index, VkSemaphore waitFor) const {
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &waitFor;
	presentInfo.swapchainCount = 1u;
	presentInfo.pSwapchains = &m_vkSwapchain;
	presentInfo.pImageIndices = &index;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(m_pCore->m_vkQueue, &presentInfo);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Swapchain recreation failure", result));
	}
}