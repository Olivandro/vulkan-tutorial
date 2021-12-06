//
//  graphicsDataBuffer.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 28/11/2021.
//

#ifndef graphicsDataBuffer_h
#define graphicsDataBuffer_h

#include "geninc.h"

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

#endif /* graphicsDataBuffer_h */
