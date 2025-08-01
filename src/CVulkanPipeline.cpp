#include <CVulkanPipeline.h>
#include <CVulkanCore.h>
#include <CVulkanPass.h>
#include <Utilities.h>

#include <stdexcept>

VulkanApp::CVulkanPipeline::CVulkanPipeline(CVulkanCore* pParent, CVulkanPass* pPass, const uint32_t vpWidth, const uint32_t vpHeight, VkPipelineShaderStageCreateInfo* shaderStages)
	: m_pCore(pParent)
{
	// Initialize descriptors with deafult values
	m_vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertexInputStateCI.vertexBindingDescriptionCount = 0;
	m_vertexInputStateCI.pVertexBindingDescriptions = nullptr;
	m_vertexInputStateCI.vertexAttributeDescriptionCount = 0;
	m_vertexInputStateCI.pVertexAttributeDescriptions = nullptr;

	m_inputAssemblyCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	m_inputAssemblyCI.primitiveRestartEnable = VK_FALSE;

	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	m_viewport.width = static_cast<float>(vpWidth);
	m_viewport.height = static_cast<float>(vpHeight);
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	m_scissorRect.offset = { 0,0 };
	m_scissorRect.extent.width = vpWidth;
	m_scissorRect.extent.height = vpHeight;

	m_viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewportStateCI.viewportCount = 1;
	m_viewportStateCI.pViewports = &m_viewport;
	m_viewportStateCI.scissorCount = 1;
	m_viewportStateCI.pScissors = &m_scissorRect;

	m_rasterizerStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterizerStateCI.depthClampEnable = VK_FALSE;
	m_rasterizerStateCI.rasterizerDiscardEnable = VK_FALSE;
	m_rasterizerStateCI.polygonMode = VK_POLYGON_MODE_FILL;
	m_rasterizerStateCI.lineWidth = 1.0f;
	m_rasterizerStateCI.cullMode = VK_CULL_MODE_BACK_BIT;
	m_rasterizerStateCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
	m_rasterizerStateCI.depthBiasEnable = VK_FALSE;
	m_rasterizerStateCI.depthBiasConstantFactor = 0.0f;
	m_rasterizerStateCI.depthBiasClamp = 0.0f;
	m_rasterizerStateCI.depthBiasSlopeFactor = 0.0f;

	m_multisamplingStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisamplingStateCI.sampleShadingEnable = VK_FALSE;
	m_multisamplingStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_multisamplingStateCI.minSampleShading = 1.0f;
	m_multisamplingStateCI.pSampleMask = nullptr;
	m_multisamplingStateCI.alphaToCoverageEnable = VK_FALSE;
	m_multisamplingStateCI.alphaToOneEnable = VK_FALSE;

	m_colorBlendAttachmentCI.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	m_colorBlendAttachmentCI.blendEnable = VK_FALSE;
	m_colorBlendAttachmentCI.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachmentCI.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	m_colorBlendAttachmentCI.colorBlendOp = VK_BLEND_OP_ADD;
	m_colorBlendAttachmentCI.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachmentCI.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	m_colorBlendAttachmentCI.alphaBlendOp = VK_BLEND_OP_ADD;

	m_colorBlendingCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlendingCI.logicOpEnable = VK_FALSE;
	m_colorBlendingCI.attachmentCount = 1;
	m_colorBlendingCI.pAttachments = &m_colorBlendAttachmentCI;

	m_pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	m_pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	m_pipelineCI.stageCount = 2;
	m_pipelineCI.pStages = shaderStages;
	m_pipelineCI.pVertexInputState = &m_vertexInputStateCI;
	m_pipelineCI.pInputAssemblyState = &m_inputAssemblyCI;
	m_pipelineCI.pViewportState = &m_viewportStateCI;
	m_pipelineCI.pRasterizationState = &m_rasterizerStateCI;
	m_pipelineCI.pMultisampleState = &m_multisamplingStateCI;
	m_pipelineCI.pDepthStencilState = nullptr;
	m_pipelineCI.pColorBlendState = &m_colorBlendingCI;
	m_pipelineCI.layout = m_vkPipelineLayout;
	m_pipelineCI.renderPass = pPass->GetRenderPass();
	m_pipelineCI.subpass = 0;
	m_pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
	m_pipelineCI.basePipelineIndex = -1;

	Initialize();

}

void VulkanApp::CVulkanPipeline::Initialize() {
	
	Release();

	VkResult result = vkCreatePipelineLayout(m_pCore->GetVkLogicalDevice(), &m_pipelineLayoutCI, nullptr, &m_vkPipelineLayout);

	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create pipeline layout", result));
	}

	result = vkCreateGraphicsPipelines(m_pCore->GetVkLogicalDevice(), VK_NULL_HANDLE, 1, &m_pipelineCI, nullptr, &m_vkPipeline);

	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create pipeline", result));
	}

}

void VulkanApp::CVulkanPipeline::Release() {

	vkDeviceWaitIdle(m_pCore->GetVkLogicalDevice());

	if (m_vkPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(m_pCore->GetVkLogicalDevice(), m_vkPipeline, nullptr);
		m_vkPipeline = VK_NULL_HANDLE;
	}

	if (m_vkPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(m_pCore->GetVkLogicalDevice(), m_vkPipelineLayout, nullptr);
		m_vkPipelineLayout = VK_NULL_HANDLE;
	}

}
