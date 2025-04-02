#pragma once
#include <string>
#include <vector>

#ifndef VULKAN_H_
#include <vulkan/vulkan.h>
#endif

namespace VulkanApp {
	class Application;
	class Renderer {
	public:
		Renderer(Application* parent);
		~Renderer();
		VkRenderPass GetRenderPass() const;
		VkFramebuffer GetFramebuffer(uint32_t index) const;
		VkPipeline GetPipeline() const;

	private:
		VkShaderModule LoadCompiledShader(const std::string& filePath);

		Application *m_parent = nullptr;
		VkShaderModule m_vertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_fragmentShaderModule = VK_NULL_HANDLE;
		VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
		VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_vkPipeline = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_vkFramebuffers;
	};
}
