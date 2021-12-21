//
//  commandBuffer.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef commandBuffer_h
#define commandBuffer_h

#include "geninc.h"

VkFramebuffer* createSwapChainFramebuffers(VkDevice device, VkImageView depthImageView, VkImageView* swapChainImageViews, int swapChainImagesCount, VkRenderPass renderPass, VkExtent2D extent);

VkCommandPool createCommandPool(VkDevice device, uint32_t presentFamily);

VkCommandBuffer* createCommandBuffers(VkDevice device, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkPipelineLayout pipelineLayout, VkFramebuffer* swapChainFramebuffers, VkCommandPool commandPool, int swapChainImagesCount, VkExtent2D extent, VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t num_vertices, uint32_t indicesSize, VkDescriptorSet* descriptorSets);

struct syncAndFence createSyncAndFence(VkDevice device, const int MAX_FRAMES_IN_FLIGHT, int swapChainImagesCount);

#endif /* commandBuffer_h */
