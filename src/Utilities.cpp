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


std::vector<std::string> VulkanApp::CapsInfo::GetSupportedExtenstions() {

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> propertiesList(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, propertiesList.data());

	std::vector<std::string> result;
	for (auto& properties : propertiesList)
		result.push_back(std::string(properties.extensionName));

	return result;
}

std::vector<std::string> VulkanApp::CapsInfo::GetAvailableInstanceLayers() {

	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	std::vector<std::string> result;
	for (auto& properties : layerProperties)
		result.push_back(std::string(properties.layerName));

	return result;
}

std::vector<std::string> VulkanApp::CapsInfo::GetAvailableDevices(const VkInstance& instance) {

	uint32_t devicesCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);

	std::vector<std::string> deviceList;

	if (devicesCount == 0)
		return deviceList;

	std::vector<VkPhysicalDevice> physicalDevices(devicesCount);
	result = vkEnumeratePhysicalDevices(instance, &devicesCount, physicalDevices.data());

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

std::optional<uint32_t> VulkanApp::CapsInfo::GetQueueFamilies(const VkPhysicalDevice& device, const VkQueueFlags queueFlags) {

	uint32_t familiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familiesCount, nullptr);

	if (familiesCount == 0)
		return std::optional<uint32_t>();

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(familiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familiesCount, queueFamilyProperties.data());

	uint32_t i = 0;
	for (auto& properties : queueFamilyProperties) {
		if (properties.queueFlags & queueFlags) {
			return i;
		}
		i++;
	}

	return std::optional<uint32_t>();
}

std::vector<std::string> VulkanApp::CapsInfo::GetSupportedExtensions(const VkPhysicalDevice& physicalDevice) {

	uint32_t extensionCount = 0;
	VkResult result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<std::string> extensionList;

	if (!extensionCount)
		return extensionList;

	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data());

	for (auto& itr : extensionProperties)
		extensionList.push_back(itr.extensionName);

	return extensionList;
}
