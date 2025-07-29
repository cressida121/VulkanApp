#ifndef C_VULKAN_CORE_H_
#define C_VULKAN_CORE_H_

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

namespace VulkanApp {
	class CVulkanSwapchain;
	class CVulkanCore {
	public:	
		CVulkanCore(const std::string& applicationName);
		~CVulkanCore();
		const VkInstance GetVkInstance() const { return m_vkInstance; };
		const VkDevice GetVkLogicalDevice() const { return m_vkLogicalDevice; };
		const VkPhysicalDevice GetVkPhysicalDevice() const { return m_vkPhysicalDevices; };

		VkQueue m_vkQueue = VK_NULL_HANDLE; // To be removed

	private:
		VkResult InitVkInstance() noexcept;
		VkResult InitVkLogicalDevice(const VkDeviceQueueCreateInfo *const queueCI) noexcept;

		std::string m_applicationName;
		VkInstance m_vkInstance = VK_NULL_HANDLE;
		VkPhysicalDevice m_vkPhysicalDevices = VK_NULL_HANDLE;
		uint32_t m_physicalDevicesCount = 0u;
		VkDevice m_vkLogicalDevice = VK_NULL_HANDLE;
		uint32_t m_queueFamilyIndex = 0u;
	};

}

#endif // !C_VULKAN_CORE_H_
