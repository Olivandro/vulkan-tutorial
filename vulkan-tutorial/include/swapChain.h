//
//  swapChain.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef swapChain_h
#define swapChain_h

#include "geninc.h"

VkSwapchainKHR createSwapChain(VkDevice device, VkSurfaceKHR surface, struct availablePresentsAnFormats presentsAnFormatsInfo, struct graphicsFamiliesAnIndices queueFamilyIndicesInfo);

VkImageView* createImageView(VkDevice device, VkSwapchainKHR swapChainKHR, struct availablePresentsAnFormats presentsAnFormatsInfo);

int findSwapChainImageCount(VkDevice device, VkSwapchainKHR swapChainKHR, struct availablePresentsAnFormats presentsAnFormatsInfo);

#endif /* swapChain_h */
