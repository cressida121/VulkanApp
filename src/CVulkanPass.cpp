#include <CVulkanPass.h>
#include <CVulkanCore.h>
#include <Utilities.h>
#include <fstream>

VulkanApp::CVulkanPass::CVulkanPass(CVulkanCore *pParent, std::vector<VkImageView> renderTargets, const uint32_t fbWidth, const uint32_t fbHeight)
	: m_pCore(pParent), m_fbWidth(fbWidth), m_fbHeight(fbHeight)
{
	m_attachmentDesc.format = VkFormat::VK_FORMAT_R8G8B8_SRGB;
	m_attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	m_attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	m_attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	m_attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	m_colorAttachmentRef.attachment = 0;
	m_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	m_subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	m_subpassDesc.colorAttachmentCount = 1;
	m_subpassDesc.pColorAttachments = &m_colorAttachmentRef;

	m_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	m_dependency.dstSubpass = 0;
	m_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	m_dependency.srcAccessMask = 0;
	m_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	m_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	m_renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	m_renderPassCI.attachmentCount = 1;
	m_renderPassCI.pAttachments = &m_attachmentDesc;
	m_renderPassCI.subpassCount = 1;
	m_renderPassCI.pSubpasses = &m_subpassDesc;
	m_renderPassCI.dependencyCount = 1;
	m_renderPassCI.pDependencies = &m_dependency;

	Initialize();
	SetupFramebuffers(renderTargets);
}

VulkanApp::CVulkanPass::~CVulkanPass() {
	ReleaseFramebuffers();
	Release();
}
void VulkanApp::CVulkanPass::Initialize() {
	Release();
	VkResult result = vkCreateRenderPass(m_pCore->GetVkLogicalDevice(), &m_renderPassCI, nullptr, &m_vkRenderPass);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create render pass", result));
	}
}
void VulkanApp::CVulkanPass::Release() {
	if (m_vkRenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(m_pCore->GetVkLogicalDevice(), m_vkRenderPass, nullptr);
		m_vkRenderPass = VK_NULL_HANDLE;
	}
}
void VulkanApp::CVulkanPass::SetupFramebuffers(std::vector<VkImageView> renderTargets) {
	
	ReleaseFramebuffers();

	m_vkFramebuffers.resize(renderTargets.size());
	uint32_t i = 0;

	for (auto renderTarget : renderTargets) {

		VkFramebufferCreateInfo framebufferCI = {};
		framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCI.attachmentCount = 1;
		framebufferCI.pAttachments = &renderTarget;
		framebufferCI.renderPass = m_vkRenderPass;
		framebufferCI.width = m_fbWidth;
		framebufferCI.height = m_fbHeight;
		framebufferCI.layers = 1;

		VkResult result = vkCreateFramebuffer(m_pCore->GetVkLogicalDevice(), &framebufferCI, nullptr, &m_vkFramebuffers[i++]);

		if (result != VK_SUCCESS) {
			throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create framebuffer", result));
		}
	}
}

void VulkanApp::CVulkanPass::ReleaseFramebuffers() {
	for (auto framebuffer : m_vkFramebuffers) {
		vkDestroyFramebuffer(m_pCore->GetVkLogicalDevice(), framebuffer, nullptr);
	}
	m_vkFramebuffers.clear();
}

