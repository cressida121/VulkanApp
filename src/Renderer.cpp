#include "Renderer.h"
#include "Application.h"
#include <CVulkanSwapchain.h>

#include <stdexcept>
#include <fstream>


VulkanApp::Renderer::Renderer(Application* parent, const uint32_t viewportWidth, const uint32_t viewportHeight)
	: m_parent(parent),m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight) {

	// Initialize shaders

	m_vertexShaderModule = LoadCompiledShader("C:\\Users\\patry\\source\\repos\\VulkanApp\\shaders\\compiled\\VertexShader.spv");
	m_fragmentShaderModule = LoadCompiledShader("C:\\Users\\patry\\source\\repos\\VulkanApp\\shaders\\compiled\\FragmentShader.spv");

	VkPipelineShaderStageCreateInfo vertexShaderCI = {};
	vertexShaderCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderCI.module = m_vertexShaderModule;
	vertexShaderCI.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderCI = {};
	fragmentShaderCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderCI.module = m_fragmentShaderModule;
	fragmentShaderCI.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCI, fragmentShaderCI };

	// Create render pass

	VkAttachmentDescription attachmentDesc = {};
	attachmentDesc.format = m_parent->m_vkSurfaceFormat.format;
	attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassCI = {};
	renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCI.attachmentCount = 1;
	renderPassCI.pAttachments = &attachmentDesc;
	renderPassCI.subpassCount = 1;
	renderPassCI.pSubpasses = &subpassDesc;
	renderPassCI.dependencyCount = 1;
	renderPassCI.pDependencies = &dependency;

	if (vkCreateRenderPass(m_parent->m_vkCore.GetVkLogicalDevice(), &renderPassCI, nullptr, &m_vkRenderPass) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create render pass");
	}

	// Create the pipeline
		// Vertex input

	VkPipelineVertexInputStateCreateInfo vertexInputStateCI = {};
	vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCI.vertexBindingDescriptionCount = 0;
	vertexInputStateCI.pVertexBindingDescriptions = nullptr;
	vertexInputStateCI.vertexAttributeDescriptionCount = 0;
	vertexInputStateCI.pVertexAttributeDescriptions = nullptr;

		// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI = {};
	inputAssemblyCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCI.primitiveRestartEnable = VK_FALSE;

		// Viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = m_viewportWidth;
	viewport.height = m_viewportHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent.width = m_viewportWidth;
	scissor.extent.height = m_viewportHeight;

	VkPipelineViewportStateCreateInfo viewportStateCI = {};
	viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCI.viewportCount = 1;
	viewportStateCI.pViewports = &viewport;
	viewportStateCI.scissorCount = 1;
	viewportStateCI.pScissors = &scissor;

		// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerStateCI = {};
	rasterizerStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerStateCI.depthClampEnable = VK_FALSE;
	rasterizerStateCI.rasterizerDiscardEnable = VK_FALSE;
	rasterizerStateCI.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerStateCI.lineWidth = 1.0f;
	rasterizerStateCI.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerStateCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerStateCI.depthBiasEnable = VK_FALSE;
	rasterizerStateCI.depthBiasConstantFactor = 0.0f;
	rasterizerStateCI.depthBiasClamp = 0.0f;
	rasterizerStateCI.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisamplingStateCI = {};
	multisamplingStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingStateCI.sampleShadingEnable = VK_FALSE;
	multisamplingStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplingStateCI.minSampleShading = 1.0f;
	multisamplingStateCI.pSampleMask = nullptr;
	multisamplingStateCI.alphaToCoverageEnable = VK_FALSE;
	multisamplingStateCI.alphaToOneEnable = VK_FALSE;

		// Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachmentCI = {};
	colorBlendAttachmentCI.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentCI.blendEnable = VK_FALSE;
	colorBlendAttachmentCI.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentCI.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentCI.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentCI.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentCI.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentCI.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendingCI = {};
	colorBlendingCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingCI.logicOpEnable = VK_FALSE;
	colorBlendingCI.attachmentCount = 1;
	colorBlendingCI.pAttachments = &colorBlendAttachmentCI;

		// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	
	if (vkCreatePipelineLayout(m_parent->m_vkCore.GetVkLogicalDevice(), &pipelineLayoutCI, nullptr, &m_vkPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipelineCI = {};
	pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCI.stageCount = 2;
	pipelineCI.pStages = shaderStages;
	pipelineCI.pVertexInputState = &vertexInputStateCI;
	pipelineCI.pInputAssemblyState = &inputAssemblyCI;
	pipelineCI.pViewportState = &viewportStateCI;
	pipelineCI.pRasterizationState = &rasterizerStateCI;
	pipelineCI.pMultisampleState = &multisamplingStateCI;
	pipelineCI.pDepthStencilState = nullptr;
	pipelineCI.pColorBlendState = &colorBlendingCI;
	pipelineCI.layout = m_vkPipelineLayout;
	pipelineCI.renderPass = m_vkRenderPass;
	pipelineCI.subpass = 0;
	pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCI.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(m_parent->m_vkCore.GetVkLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &m_vkPipeline) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create pipeline");
	}
}

VulkanApp::Renderer::~Renderer() {

	for (auto& fbuff : m_vkFramebuffers) {
		vkDestroyFramebuffer(m_parent->m_vkCore.GetVkLogicalDevice(), fbuff, nullptr);
	}

	vkDestroyPipeline(m_parent->m_vkCore.GetVkLogicalDevice(), m_vkPipeline, nullptr);
	vkDestroyPipelineLayout(m_parent->m_vkCore.GetVkLogicalDevice(), m_vkPipelineLayout, nullptr);
	vkDestroyRenderPass(m_parent->m_vkCore.GetVkLogicalDevice(), m_vkRenderPass, nullptr);
	vkDestroyShaderModule(m_parent->m_vkCore.GetVkLogicalDevice(), m_vertexShaderModule, nullptr);
	vkDestroyShaderModule(m_parent->m_vkCore.GetVkLogicalDevice(), m_fragmentShaderModule, nullptr);
}

VkRenderPass VulkanApp::Renderer::GetRenderPass() const {
	return m_vkRenderPass;
}

VkFramebuffer VulkanApp::Renderer::GetFramebuffer(uint32_t index) const {
	if (index >= 0 && index < m_vkFramebuffers.size())
		return m_vkFramebuffers[index];
	
	return VK_NULL_HANDLE;
}

VkPipeline VulkanApp::Renderer::GetPipeline() const {
	return m_vkPipeline;
}

void VulkanApp::Renderer::SetupFramebuffers(std::vector<VkImageView> renderTargets) {

	for (auto& fbuff : m_vkFramebuffers) {
		vkDestroyFramebuffer(m_parent->m_vkCore.GetVkLogicalDevice(), fbuff, nullptr);
	}
	
	m_vkFramebuffers.resize(renderTargets.size());
	uint32_t i = 0;

	for (auto renderTarget : renderTargets) {

		VkFramebufferCreateInfo framebufferCI = {};
		framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCI.attachmentCount = 1;
		framebufferCI.pAttachments = &renderTarget;
		framebufferCI.renderPass = m_vkRenderPass;
		framebufferCI.width = m_viewportWidth;
		framebufferCI.height = m_viewportHeight;
		framebufferCI.layers = 1;

		if (vkCreateFramebuffer(m_parent->m_vkCore.GetVkLogicalDevice(), &framebufferCI, nullptr, &m_vkFramebuffers[i++]) != VK_SUCCESS) {
			throw std::runtime_error("[Runtime error] Failed to create framebuffer");
		}
	}
}

VkShaderModule VulkanApp::Renderer::LoadCompiledShader(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("[Runtime error] Failed to open compiled shader file");
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo shaderModuleCI = {};
	shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCI.codeSize = buffer.size();
	shaderModuleCI.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VkShaderModule shaderModule = VK_NULL_HANDLE;
	if (vkCreateShaderModule(m_parent->m_vkCore.GetVkLogicalDevice(), &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("[Runtime error] Failed to create shader module");
	}

	return shaderModule;
}
