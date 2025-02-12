#pragma once
#include "nameless_device.h"
#include "nameless_buffer.h"
#include <memory>
#include "stb_image.h"

namespace nameless {
	class NamelessTexture {
	public:
		NamelessTexture(NamelessDevice& namelessDevice, VkFormat format);
		~NamelessTexture();

		NamelessTexture(const NamelessTexture&) = delete;
		NamelessTexture& operator=(const NamelessTexture&) = delete;

		void createTextureImage(const char* textureFilePath);

		void createTextureImageView();

	private:
		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
		VkImageCreateInfo createImageInfo(uint32_t width, uint32_t height, VkImageTiling tiling, VkImageUsageFlags usage);
		NamelessDevice& namelessDevice;
		VkImage textureImage = VK_NULL_HANDLE;
		VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
		VkImageView textureImageView = VK_NULL_HANDLE;
		VkFormat format;
	};
}