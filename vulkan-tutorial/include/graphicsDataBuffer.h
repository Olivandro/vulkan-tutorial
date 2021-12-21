//
//  graphicsDataBuffer.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 28/11/2021.
//

#ifndef graphicsDataBuffer_h
#define graphicsDataBuffer_h

#include "geninc.h"


// Vertex and index buffer
VkBuffer createVertexBuffer(VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags usage);

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties,  VkPhysicalDeviceMemoryProperties memProperties);

VkDeviceMemory createVertexBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer vertexBuffer, VkMemoryPropertyFlags properties);

VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);

void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer);

void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);

void createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t swapChainImagesCount, VkBuffer* uniformBuffers, VkDeviceMemory* uniformBufferMemory);

VkDescriptorPool createDescriptorPool(VkDevice device, uint32_t swapChainImagesCount);

VkDescriptorSet* createDescriptorSet(VkDevice device, uint32_t swapChainImagesCount, VkBuffer* uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, VkImageView textureImageView, VkSampler textureSampler);

void updateUniformBuffer(VkDevice device, VkExtent2D extent, uint32_t currentImage, clock_t start, VkDeviceMemory* uniformBufferMemory);

// Texture Mapping
void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

void createTextureImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage* textureImage, VkDeviceMemory* textureImageMemory, char* TEST_TEXTURE_FILE_PATH);

void createTextureImageView(VkDevice device, VkImage textureImage, VkFormat format, VkImageView* textureImageView, VkImageAspectFlags aspectFlags);

void createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice, VkSampler* textureSampler);

// Depth Buffer functions
void createDepthResources(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, uint32_t width, uint32_t height, VkImage* depthImage, VkDeviceMemory* depthImageMemory, VkImageView* depthImageView);

#endif /* graphicsDataBuffer_h */
