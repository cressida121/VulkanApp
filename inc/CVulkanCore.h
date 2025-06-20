#ifndef C_VULKAN_CORE_H_
#define C_VULKAN_CORE_H_

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

#define VK_APP_EXC_MSG(msg, code) CreateExceptionMessage(msg, code, __FILE__, __LINE__)

namespace VulkanApp {

	std::string CreateExceptionMessage(const std::string msg,VkResult code, const std::string file, uint32_t line);

	class CVulkanCore {
		friend class CVulkanSwapchain;
	public:	
		CVulkanCore(const std::string& applicationName);
		~CVulkanCore();

	//private:
		VkResult CreateVkInstance() noexcept;
		VkResult CreateVkLogicalDevice(const VkDeviceQueueCreateInfo *const queueCI) noexcept;

		std::string m_applicationName;
		VkInstance m_vkInstance = VK_NULL_HANDLE;
		VkPhysicalDevice m_vkPhysicalDevices = VK_NULL_HANDLE;
		uint32_t m_physicalDevicesCount = 0u;
		uint32_t m_physicalDeviceIndex = 0u;
		VkDevice m_vkLogicalDevice = VK_NULL_HANDLE;
		VkQueue m_vkQueue = VK_NULL_HANDLE;
		uint32_t m_queueFamilyIndex = 0u;

		std::vector<CVulkanSwapchain*> m_pOwnedSwapchains;

	};

}

#endif // !C_VULKAN_CORE_H_
