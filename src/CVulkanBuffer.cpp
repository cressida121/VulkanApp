#include <CVulkanBuffer.h>
#include <CVulkanCore.h>

#include <Utilities.h>

/*
CBuffer:
-> Vertex buffer
-> Index buffer
-> Uniform buffer
-> Texture buffer



*/


namespace VulkanApp {
	static uint32_t GetShaderDataTypeSize(BufferAttribute::ShaderDataType shaderDataType) {
		switch (shaderDataType)
		{
		case BufferAttribute::ShaderDataType::int3:		return 4 * 3;
		case BufferAttribute::ShaderDataType::int4:		return 4 * 4;
		case BufferAttribute::ShaderDataType::uint3:	return 4 * 3;
		case BufferAttribute::ShaderDataType::uint4:	return 4 * 4;
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
		: m_pCore(pCore), m_byteSize(byteSize){

		m_vkBuffer = CreateBuffer(
			m_pCore,
			byteSize,
			usage,
			(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
			VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
			&m_vkBufferMemory);

		VkResult result = vkMapMemory(m_pCore->GetVkLogicalDevice(), m_vkBufferMemory, 0, m_byteSize, 0, &m_pMappedData);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Buffer memory mapping failed.", result));
		}

		SetData(data);
	}

	void CVulkanBuffer::SetData(const void* data) {
		memcpy(m_pMappedData, data, m_byteSize);
	}

	CVulkanBuffer::~CVulkanBuffer() {

		vkUnmapMemory(m_pCore->GetVkLogicalDevice(), m_vkBufferMemory);

		if (m_vkBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(m_pCore->GetVkLogicalDevice(), m_vkBufferMemory, nullptr);
		}

		if (m_vkBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(m_pCore->GetVkLogicalDevice(), m_vkBuffer, nullptr);
		}
	}

	VkBuffer CVulkanBuffer::CreateBuffer(const CVulkanCore *const pCore, const uint32_t byteSize, const uint32_t bufferUsageFlagBits,
		const uint32_t memoryPropertyFlagBits, const VkSharingMode sharingMode, VkDeviceMemory *pBufferMemoryOut)
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkBufferCreateInfo bufferCI = {};
		bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCI.size = byteSize;
		bufferCI.usage = bufferUsageFlagBits;
		bufferCI.sharingMode = sharingMode;

		VkResult result = vkCreateBuffer(pCore->GetVkLogicalDevice(), &bufferCI, nullptr, &buffer);

		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Buffer creation failed.", result));
		}

		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(pCore->GetVkLogicalDevice(), buffer, &memoryRequirements);

		VkPhysicalDeviceMemoryProperties bufferMemoryProps = {};
		vkGetPhysicalDeviceMemoryProperties(pCore->GetVkPhysicalDevice(), &bufferMemoryProps);

		int memoryTypeIndex = -1;
		for (int i = 0; i < bufferMemoryProps.memoryTypeCount; i++) {
			if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
				(bufferMemoryProps.memoryTypes[i].propertyFlags & memoryPropertyFlagBits)) {
				memoryTypeIndex = i;
				break;
			}
		}

		if (memoryTypeIndex == -1) {
			throw std::runtime_error(UTIL_EXC_MSG("Unable to find required memory type."));
		}

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		result = vkAllocateMemory(pCore->GetVkLogicalDevice(), &memoryAllocateInfo, nullptr, &bufferMemory);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Memory allocation failed.", result));
		}

		result = vkBindBufferMemory(pCore->GetVkLogicalDevice(), buffer, bufferMemory, 0);
		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Buffer memory binding failed.", result));
		}
		*pBufferMemoryOut = bufferMemory;

		return buffer;
	}
}
