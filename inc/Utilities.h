#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <vulkan/vk_enum_string_helper.h>

#include <sstream>
#include <vector>
#include <string>
#include <optional>

#define UTIL_EXC_MSG_EX(msg, code) VulkanApp::CreateExceptionMessage(msg, code, __FILE__, __LINE__)
#define UTIL_EXC_MSG(msg) VulkanApp::CreateExceptionMessage(msg, VK_SUCCESS, __FILE__, __LINE__)

namespace VulkanApp {
	std::string CreateExceptionMessage(const std::string msg, VkResult code, const std::string file, uint32_t line);
	namespace CapsInfo {
		std::vector<std::string> GetSupportedExtenstions();
		std::vector<std::string> GetAvailableInstanceLayers();
		std::vector<std::string> GetAvailableDevices(const VkInstance& instance);
		std::optional<uint32_t> GetQueueFamilies(const VkPhysicalDevice& device, const VkQueueFlags queueFlags);
		std::vector<std::string> GetSupportedExtensions(const VkPhysicalDevice& physicalDevice);
	}
}

#endif // !UTILITIES_H_
