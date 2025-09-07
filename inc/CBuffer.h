#ifndef C_BUFFER_H_
#define C_BUFFER_H_

#include <string>
#include <vector>

namespace VulkanApp {

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

		BufferAttribute(ShaderDataType shaderDataType, const std::string name)
			: m_name(name),
			m_shaderDataType(shaderDataType),
			m_size(GetShaderDataTypeSize(shaderDataType)),
			m_offset(0) {}

		static uint32_t GetShaderDataTypeSize(ShaderDataType shaderDataType) {
			switch (shaderDataType)
			{
			case ShaderDataType::int3:		return 4 * 3;
			case ShaderDataType::int4:		return 4 * 4;
			case ShaderDataType::uint3:		return 4 * 3;
			case ShaderDataType::uint4:		return 4 * 4;
			case ShaderDataType::float3:	return 4 * 3;
			case ShaderDataType::float4:	return 4 * 4;
			default: break;
			}
			return 0;
		}
	};

	class CBufferLayout {
	public:
		CBufferLayout(std::initializer_list<BufferAttribute> bufferAttributes)
			: m_attributes(bufferAttributes.begin(), bufferAttributes.end()) {
			uint32_t offset = 0;
			for (auto& attribute : m_attributes) {
				attribute.m_offset = offset;
				offset += attribute.m_size;
			}
			m_byteSize = offset;
		};
		uint32_t GetAttributesCount() const { return m_attributes.size(); };
		uint32_t GetByteSize() const { return m_byteSize; };
		BufferAttribute GetAttribute(uint32_t index) const { return m_attributes[index]; };
	private:
		std::vector<BufferAttribute> m_attributes;
		uint32_t m_byteSize = 0;
	};
}
#endif C_BUFFER_H_