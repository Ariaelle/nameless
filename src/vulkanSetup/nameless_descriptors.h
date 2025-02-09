#pragma once

#include "nameless_device.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace nameless {
	class NamelessDescriptorSetLayout {
	public:
		class Builder {
		public:
			Builder(NamelessDevice& device) : namelessDevice{ device } {}

			Builder& addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFalgs, uint32_t count = 1);
			std::unique_ptr<NamelessDescriptorSetLayout> build() const;
		private:
			NamelessDevice& namelessDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		NamelessDescriptorSetLayout(NamelessDevice& namelessDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~NamelessDescriptorSetLayout();
		NamelessDescriptorSetLayout(const NamelessDescriptorSetLayout&) = delete;
		NamelessDescriptorSetLayout operator=(const NamelessDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		NamelessDevice& namelessDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class NamelessDescriptorWriter;
	};

	class NamelessDescriptorPool {
	public:
		class Builder {
		public:
			Builder(NamelessDevice &namelessDevice) : namelessDevice {namelessDevice} {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setpoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<NamelessDescriptorPool> build() const;

		private:
			NamelessDevice& namelessDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		NamelessDescriptorPool(NamelessDevice& namelessDevice, uint32_t maxSets, VkDescriptorSetLayoutCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~NamelessDescriptorPool();
		NamelessDescriptorPool(const NamelessDescriptorPool&) = delete;
		NamelessDescriptorPool operator=(const NamelessDescriptorPool&) = delete;

		bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
		void resetPool();

	private:
		NamelessDevice& namelessDevice;
		VkDescriptorPool descriptorPool;

		friend class NamelessDescriptorWriter;
	};

	class NamelessDescriptorWriter {
	public:
		NamelessDescriptorWriter(NamelessDescriptorSetLayout& setLayout, NamelessDescriptorPool& pool);

		NamelessDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		NamelessDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		NamelessDescriptorSetLayout& setLayout;
		NamelessDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};
}