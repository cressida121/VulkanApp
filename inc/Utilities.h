#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <vulkan/vk_enum_string_helper.h>

#include <sstream>

#define UTIL_EXC_MSG_EX(msg, code) VulkanApp::CreateExceptionMessage(msg, code, __FILE__, __LINE__)
#define UTIL_EXC_MSG(msg) VulkanApp::CreateExceptionMessage(msg, VK_SUCCESS, __FILE__, __LINE__)

namespace VulkanApp {
	std::string CreateExceptionMessage(const std::string msg, VkResult code, const std::string file, uint32_t line);
}

#endif // !UTILITIES_H_
