#include <Utilities.h>

std::string VulkanApp::CreateExceptionMessage(const std::string msg, VkResult code, const std::string file, uint32_t line) {

	auto ritr = file.rbegin();
	while (*(++ritr) != '\\');

	std::string fileTruncated;
	if (ritr.base() != file.begin())
		std::copy(ritr.base(), file.cend(), std::back_inserter(fileTruncated));

	std::stringstream stream;
	stream << "[EXCEPTION MESSAGE] " << msg << "\n";
	if (code != VK_SUCCESS) {
		stream << "[VKRESULT] " << string_VkResult(code) << "\n";
	}
	stream << "[FILE] " << fileTruncated << "\n";
	stream << "[LINE] " << std::to_string(line) << "\n";

	return stream.str();
}