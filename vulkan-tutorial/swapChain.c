//
//  swapChain.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#include "swapChain.h"

VkSwapchainKHR createSwapChain(VkDevice device, VkSurfaceKHR surface, struct availablePresentsAnFormats presentsAnFormatsInfo, struct graphicsFamiliesAnIndices queueFamilyIndicesInfo)
{
        VkSwapchainCreateInfoKHR createSwapChainInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = surface,
            .minImageCount = presentsAnFormatsInfo.imageCount,
            .imageFormat = presentsAnFormatsInfo.availableFormats.format,
            .imageColorSpace = presentsAnFormatsInfo.availableFormats.colorSpace,
            .presentMode = presentsAnFormatsInfo.avaiablePresentMode,
            .imageExtent = presentsAnFormatsInfo.extent,
            .imageArrayLayers = 1,
            .preTransform = presentsAnFormatsInfo.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        };
        
        
        if (queueFamilyIndicesInfo.graphicsFamilyIndices != queueFamilyIndicesInfo.presentFamily)
        {
            printf("Graphics family does not equal present family\n\n");
            createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createSwapChainInfo.queueFamilyIndexCount = 2;
            createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndicesInfo.queueFamilyIndices;
        }
        else
        {
            printf("Graphics family equal present family\n\n");
            createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createSwapChainInfo.queueFamilyIndexCount = 0; // Optional
            createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndicesInfo.queueFamilyIndices; // Optional
        }
        
    //    global variable - this is our swapchain
        VkSwapchainKHR swapChainKHR;
    //    temp rtn variable for swapchain creation
        VkResult swapChainCreateResult = vkCreateSwapchainKHR(device, &createSwapChainInfo, NULL, &swapChainKHR);
        if (swapChainCreateResult != VK_SUCCESS)
        {
            printf("failed to create swap chain!\n");
            return -1;
        }
        
        return swapChainKHR;
}



VkImageView* createImageView(VkDevice device, VkSwapchainKHR swapChainKHR, struct availablePresentsAnFormats presentsAnFormatsInfo)
{
    //    local malloc variable that is freed at the end of this code block
        VkImage* swapChainImages = malloc(sizeof(VkImage) * 1);
        
    //    These two functions should really be wrapped in if statements to make sure they are successful
        vkGetSwapchainImagesKHR(device, swapChainKHR, &presentsAnFormatsInfo.imageCount, NULL);
        vkGetSwapchainImagesKHR(device, swapChainKHR, &presentsAnFormatsInfo.imageCount, swapChainImages);
        
    //    reoccurring variable for tracking swapchain image count
        int swapChainImagesCount = (int) sizeof(swapChainImages) / (int) swapChainImages[0];
        
    //    Global malloc variable that is freed at the end of program
        VkImageView* swapChainImageViews = malloc(sizeof(VkImageView) * swapChainImagesCount);
        
        
        for (int i = 0; i < swapChainImagesCount; i++)
        {
            VkImageViewCreateInfo createImageViewInfo =
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = swapChainImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = presentsAnFormatsInfo.availableFormats.format,
                .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
                .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .subresourceRange.baseMipLevel = 0,
                .subresourceRange.levelCount = 1,
                .subresourceRange.baseArrayLayer = 0,
                .subresourceRange.layerCount = 1
            };
            
            
            if (vkCreateImageView(device, &createImageViewInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
                printf("failed to create image views!\n");
            }

        }
        free(swapChainImages);
        
        return swapChainImageViews;
}



int findSwapChainImageCount(VkDevice device, VkSwapchainKHR swapChainKHR, struct availablePresentsAnFormats presentsAnFormatsInfo)
{
        //    local malloc variable that is freed at the end of this code block
            VkImage* swapChainImages = malloc(sizeof(VkImage) * 1);
            
        //    These two functions should really be wrapped in if statements to make sure they are successful
            vkGetSwapchainImagesKHR(device, swapChainKHR, &presentsAnFormatsInfo.imageCount, NULL);
            vkGetSwapchainImagesKHR(device, swapChainKHR, &presentsAnFormatsInfo.imageCount, swapChainImages);
            
        //    reoccurring variable for tracking swapchain image count
        int swapChainImagesCount = (int) sizeof(swapChainImages) / (int) swapChainImages[0];
        free(swapChainImages);
    
        return swapChainImagesCount;
}

void cleanUpSwapChain(VkDevice device, struct SwapChainObj swapChainObj)
{
// Swapchain clean up
    //    Lets clear out all the Framebuffers
    //    We have to run it through a for loop to delete all the nested buffer we created earlier
    //    similar to the swap chain images just below.
    for (int i = 0; i < swapChainObj.swapChainImagesCount; i++)
    {
        vkDestroyFramebuffer(device, swapChainObj.swapChainFramebuffers[i], NULL);
    }
    free(swapChainObj.swapChainFramebuffers);

    vkFreeCommandBuffers(device, swapChainObj.commandPool, (uint32_t)(swapChainObj.swapChainImagesCount + 1), swapChainObj.commandBuffers);
    free(swapChainObj.commandBuffers);
    
//    Graphics render pass pipeline creation
    vkDestroyPipeline(device, swapChainObj.graphicsPipeline, NULL);
    
//    Rendering pass
    vkDestroyRenderPass(device, swapChainObj.renderPass, NULL);
    
//    Initial pipeline
    vkDestroyPipelineLayout(device, swapChainObj.pipelineLayout, NULL);
    free(swapChainObj.shaderStages);

    for (int i = 0; i < swapChainObj.swapChainImagesCount; i++)
    {
        vkDestroyImageView(device, swapChainObj.swapChainImageViews[i], NULL);
    }
    free(swapChainObj.swapChainImageViews);
    vkDestroySwapchainKHR(device, swapChainObj.swapChainKHR, NULL);
// END OF SWAPCHAIN CLEANUP
}
