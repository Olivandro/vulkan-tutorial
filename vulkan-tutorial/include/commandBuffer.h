//
//  commandBuffer.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef commandBuffer_h
#define commandBuffer_h

#include "geninc.h"

VkFramebuffer* createSwapChainFramebuffers(VkDevice device, VkImageView* swapChainImageViews, int swapChainImagesCount, VkRenderPass renderPass, VkExtent2D extent);

VkCommandPool createCommandPool(VkDevice device, uint32_t presentFamily);

VkCommandBuffer* createCommandBuffers(VkDevice device, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkFramebuffer* swapChainFramebuffers, VkCommandPool commandPool, int swapChainImagesCount, VkExtent2D extent, VkBuffer vertexBuffers, struct DrawingData* vertices);

struct syncAndFence createSyncAndFence(VkDevice device, const int MAX_FRAMES_IN_FLIGHT, int swapChainImagesCount);

#endif /* commandBuffer_h */
