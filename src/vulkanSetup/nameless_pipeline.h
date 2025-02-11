#pragma once
#include <string>
#include <vector>
#include "nameless_device.h"

namespace nameless {
	struct PipelineConfigInfo {
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		//dont just leave this defaulted when making one, it won't work
		PipelineConfigInfo() = default;
		

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class NamelessPipeline {
	public:
		NamelessPipeline(NamelessDevice &device, const std::string& vertFilePath, const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo);

		~NamelessPipeline();

		NamelessPipeline(const NamelessPipeline&) = delete;
		NamelessPipeline& operator=(const NamelessPipeline&) = delete;
		NamelessPipeline() = default;

		void bind(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
		static void enableAlphaBlending(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);


		NamelessDevice& namelessDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};


}