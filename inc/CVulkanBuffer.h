#ifndef C_BUFFER_H_
#define C_BUFFER_H_

#ifndef VULKAN_H_
#include <vulkan/vulkan.h>
#endif

#include <string>
#include <vector>

namespace VulkanApp {
	class CVulkanCore;

	struct BufferAttribute {
		enum ShaderDataType {
			int2,
			int3,
			int4,
			uint2,
			uint3,
			uint4,
			float2,
			float3,
			float4
		};

		std::string m_name;
		ShaderDataType m_shaderDataType;
		uint32_t m_size;
		uint32_t m_offset;

		BufferAttribute(ShaderDataType shaderDataType, const std::string name);
	};

	class CBufferLayout {
	public:
		CBufferLayout(std::initializer_list<BufferAttribute> bufferAttributes);
		uint32_t GetAttributesCount() const { return m_attributes.size(); };
		uint32_t GetByteSize() const { return m_byteSize; };
		BufferAttribute GetAttribute(uint32_t index) const { return m_attributes[index]; };
	private:
		std::vector<BufferAttribute> m_attributes;
		uint32_t m_byteSize = 0;
	};

	class CVulkanBuffer {
	public:
		CVulkanBuffer(const CVulkanCore* const pCore, const void* data, const uint32_t byteSize, VkBufferUsageFlagBits usage);
		void SetData(const void* data);
		~CVulkanBuffer();
		VkBuffer GetHandle() const { return m_vkBuffer; }
	private:
		const CVulkanCore* const m_pCore = nullptr;
		VkBufferCreateInfo m_vkBufferCI = {};
		VkBuffer m_vkBuffer = VK_NULL_HANDLE;
		VkMemoryAllocateInfo m_vkMemoryAllocateInfo = {};
		VkDeviceMemory m_vkDeviceMemory = VK_NULL_HANDLE;
		int GetMemoryType(VkBuffer buffer, int propertyFlagBits, unsigned long long* requiredSpace);
	};
}
#endif C_BUFFER_H_