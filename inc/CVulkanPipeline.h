#ifndef  C_VULKAN_PIPELINE_H_
#define  C_VULKAN_PIPELINE_H_

#ifndef VULKAN_H_
#include <vulkan/vulkan.h>
#endif

#include <vector>

namespace VulkanApp {
	class CVulkanPass;
	class CVulkanCore;
	class CVulkanPipeline {
	public:
		CVulkanPipeline(CVulkanCore *pParent, CVulkanPass *pPass, const uint32_t vpWidth, const uint32_t vpHeight, VkPipelineShaderStageCreateInfo *shaderStages);
		~CVulkanPipeline() { Release(); }
		void Initialize();

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

		VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_vkPipeline = VK_NULL_HANDLE;
		CVulkanCore *m_pCore = nullptr;
	};
}


#endif // ! C_VULKAN_PIPELINE_H_
