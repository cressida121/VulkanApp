#include <CVulkanBuffer.h>
#include <CVulkanCore.h>

#include <Utilities.h>

namespace VulkanApp {
	static uint32_t GetShaderDataTypeSize(BufferAttribute::ShaderDataType shaderDataType) {
		switch (shaderDataType)
		{
		case BufferAttribute::ShaderDataType::int3:		return 4 * 3;
		case BufferAttribute::ShaderDataType::int4:		return 4 * 4;
		case BufferAttribute::ShaderDataType::uint3:		return 4 * 3;
		case BufferAttribute::ShaderDataType::uint4:		return 4 * 4;
		case BufferAttribute::ShaderDataType::float3:	return 4 * 3;
		case BufferAttribute::ShaderDataType::float4:	return 4 * 4;
		default: break;
		}
		return 0;
	}

	BufferAttribute::BufferAttribute(ShaderDataType shaderDataType, const std::string name)
		: m_name(name),
		m_shaderDataType(shaderDataType),
		m_size(GetShaderDataTypeSize(shaderDataType)),
		m_offset(0) {
	}


	CBufferLayout::CBufferLayout(std::initializer_list<BufferAttribute> bufferAttributes)
		: m_attributes(bufferAttributes.begin(), bufferAttributes.end()) {
		uint32_t offset = 0;
		for (auto& attribute : m_attributes) {
			attribute.m_offset = offset;
			offset += attribute.m_size;
		}
		m_byteSize = offset;
	};


	CVulkanBuffer::CVulkanBuffer(const CVulkanCore* const pCore, const void* data, const uint32_t byteSize, VkBufferUsageFlagBits usage)
		: m_pCore(pCore){

		m_vkBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		m_vkBufferCI.size = byteSize;
		m_vkBufferCI.usage = usage;
		m_vkBufferCI.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(m_pCore->GetVkLogicalDevice(), &m_vkBufferCI, nullptr, &m_vkBuffer);

		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Buffer creation failed.", result));
		}

		int memoryTypeIndex = GetMemoryType(m_vkBuffer, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), &m_vkMemoryAllocateInfo.allocationSize);
		
		if (memoryTypeIndex == -1) {
			throw std::runtime_error(UTIL_EXC_MSG("Unable to find required memory type."));
		}

		m_vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
		m_vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		result = vkAllocateMemory(m_pCore->GetVkLogicalDevice(), &m_vkMemoryAllocateInfo, nullptr, &m_vkDeviceMemory);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Memory allocation failed.", result));
		}

		result = vkBindBufferMemory(m_pCore->GetVkLogicalDevice(), m_vkBuffer, m_vkDeviceMemory, 0);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Buffer memory binding failed.", result));
		}

		SetData(data);
	}

	void CVulkanBuffer::SetData(const void* data) {

		void *pMappedData = nullptr;
		VkResult result = vkMapMemory(m_pCore->GetVkLogicalDevice(), m_vkDeviceMemory, 0, m_vkBufferCI.size, 0, &pMappedData);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Buffer memory mapping failed.", result));
		}

		memcpy(pMappedData, data, m_vkBufferCI.size);
		vkUnmapMemory(m_pCore->GetVkLogicalDevice(), m_vkDeviceMemory);
	}

	CVulkanBuffer::~CVulkanBuffer() {

		if (m_vkDeviceMemory != VK_NULL_HANDLE) {
			vkFreeMemory(m_pCore->GetVkLogicalDevice(), m_vkDeviceMemory, nullptr);
		}

		if (m_vkBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(m_pCore->GetVkLogicalDevice(), m_vkBuffer, nullptr);
		}
	}

	int CVulkanBuffer::GetMemoryType(VkBuffer buffer, int propertyFlagBits, unsigned long long *requiredSpace) {
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(m_pCore->GetVkLogicalDevice(), buffer, &memoryRequirements);
		*requiredSpace = memoryRequirements.size;

		VkPhysicalDeviceMemoryProperties deviceMemoryProps = {};
		vkGetPhysicalDeviceMemoryProperties(m_pCore->GetVkPhysicalDevice(), &deviceMemoryProps);

		for (int i = 0; i < deviceMemoryProps.memoryTypeCount; i++) {
			if ((memoryRequirements.memoryTypeBits & (1 << i)) && 
				(deviceMemoryProps.memoryTypes[i].propertyFlags & propertyFlagBits)) {
				return i;
			}
		}
		return -1;
	}

}
