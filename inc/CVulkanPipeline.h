#ifndef  C_VULKAN_PIPELINE_H_
#define  C_VULKAN_PIPELINE_H_

#ifndef VULKAN_H_
#include <vulkan/vulkan.h>
#endif

#include <vector>
#include <string>

namespace VulkanApp {
	class CVulkanPass;
	class CVulkanCore;
	class CVulkanPipeline {
	public:
		CVulkanPipeline(const CVulkanCore * const pCore, const CVulkanPass *const pPass, const uint32_t vpWidth, const uint32_t vpHeight, const VkPipelineShaderStageCreateInfo *const shaderStages);
		~CVulkanPipeline() { Release(); }
		VkPipeline GetHandle() const { return m_vkPipeline; };
		void Initialize();
		static VkShaderModule LoadCompiledShader(const VkDevice device, const std::string& filePath);
		
		VkPipelineVertexInputStateCreateInfo m_vertexInputStateCI = {};
		VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyCI = {};
		VkViewport m_viewport = {};
		VkRect2D m_scissorRect = {};
		VkPipelineViewportStateCreateInfo m_viewportStateCI = {};
		VkPipelineRasterizationStateCreateInfo m_rasterizerStateCI = {};
		VkPipelineMultisampleStateCreateInfo m_multisamplingStateCI = {};
		VkPipelineColorBlendAttachmentState m_colorBlendAttachmentCI = {};
		VkPipelineColorBlendStateCreateInfo m_colorBlendingCI = {};
		VkPipelineLayoutCreateInfo m_pipelineLayoutCI = {};
		VkGraphicsPipelineCreateInfo m_pipelineCI = {};

	private:
		void Release();

		VkPipeline m_vkPipeline = VK_NULL_HANDLE;
		const CVulkanCore *const m_pCore = nullptr;
	};
}


#endif // ! C_VULKAN_PIPELINE_H_
