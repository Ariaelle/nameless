//Encapsulates a vulkan buffer. based off VulkanBuffer by Sascha Willems

#include "nameless_buffer.h"

#include <cassert>
#include <cstring>

namespace nameless {
	//Returns minimum size for compatibility with device minOffsetAlignment
	VkDeviceSize NamelessBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
		if (minOffsetAlignment > 0) {
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	NamelessBuffer::NamelessBuffer(NamelessDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount,
		VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment) :
		namelessDevice{ device }, instanceSize{ instanceSize }, instanceCount{ instanceCount }, usageFlags{ usageFlags },
		memoryPropertyFlags{ memoryPropertyFlags } 	{
		alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
		bufferSize = alignmentSize * instanceCount;
		device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
	}
	NamelessBuffer::~NamelessBuffer() {
		unmap();
		vkDestroyBuffer(namelessDevice.device(), buffer, nullptr);
		vkFreeMemory(namelessDevice.device(), memory, nullptr);
	}

	VkResult NamelessBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
		assert(buffer && memory && "Called map on buffer before creating");
		return vkMapMemory(namelessDevice.device(), memory, offset, size, 0, &mapped);
	}

	void NamelessBuffer::unmap() {
		if (mapped) {
			vkUnmapMemory(namelessDevice.device(), memory);
			mapped = nullptr;
		}
	}

	void NamelessBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
		assert(mapped && "Cannot copy to unmapped buffer");

		if (size == VK_WHOLE_SIZE) {
			memcpy(mapped, data, bufferSize);
		}
		else {
			char* memOffset = (char*)mapped;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	/*
	* Flush a memory range of the buffer to make visible to device.
	* @note Only required for non-coherent memory.
	*/
	VkResult NamelessBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(namelessDevice.device(), 1, &mappedRange);
	}

	/*
	* Invalid a memory range of the buffer to make it visible to the host.
	* @note Only required for non-coherent memory
	*/
	VkResult NamelessBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(namelessDevice.device(), 1, &mappedRange);
	}

	VkDescriptorBufferInfo NamelessBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
		return VkDescriptorBufferInfo{ buffer, offset, size };
	}

	/*
	* Copies "instanceSize" bytes of data to mapped buffer at offset of index * alignmentsize
	*/
	void NamelessBuffer::writeToIndex(void* data, int index) {
		writeToBuffer(data, instanceSize, index * alignmentSize);
	}

	VkResult NamelessBuffer::flushIndex(int index) {
		return flush(alignmentSize, index * alignmentSize);
	}

	VkDescriptorBufferInfo NamelessBuffer::descriptorInfoForIndex(int index) {
		return descriptorInfo(alignmentSize, index * alignmentSize);
	}

	VkResult NamelessBuffer::invalidateIndex(int index) {
		return invalidate(alignmentSize, alignmentSize * index);
	}

}