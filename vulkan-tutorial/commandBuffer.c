//
//  commandBuffer.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#include "commandBuffer.h"


VkFramebuffer* createSwapChainFramebuffers(VkDevice device, VkImageView* swapChainImageViews, int swapChainImagesCount, VkRenderPass renderPass, VkExtent2D extent)
{
    VkFramebuffer* swapChainFramebuffers = malloc(sizeof(VkFramebuffer) * swapChainImagesCount);
    
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        VkImageView attachments[] =
        {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1
        };


        if (vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            printf("failed to create framebuffer!\n");
            assert();
        }
    }
    return swapChainFramebuffers;
}


VkCommandPool createCommandPool(VkDevice device, uint32_t presentFamily)
{
    VkCommandPool commandPool;
    
    VkCommandPoolCreateInfo poolInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = presentFamily,
        .flags = 0 // Optional
    };

    if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        printf("failed to create command pool!\n");
        assert();
    }
    
    return commandPool;
}


VkCommandBuffer* createCommandBuffers(VkDevice device, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkFramebuffer* swapChainFramebuffers, VkCommandPool commandPool, int swapChainImagesCount, VkExtent2D extent)
{

    VkCommandBuffer* commandBuffers = malloc(sizeof(VkCommandBuffer) * (size_t)(swapChainImagesCount + 1));


    VkCommandBufferAllocateInfo allocInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) swapChainImagesCount + 1 // This use to be sizeof(commandBuffers), however that value is 8 (for number of bits),
//        When  diveded by [0] of the commandBuffer array, this equals 1. This cause the program to crash as VkSubmitQueue(). value of 2 also
//        crashes... 3 is the minimum value.
    };

    
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) != VK_SUCCESS) {
        printf("failed to allocate command buffers!\n");
        assert();
    }

    //    Starting the command buffer recording
//    int commandBufferSize = (int) sizeof(commandBuffers) / sizeof(commandBuffers[0]);

    //    All values in this code block are temporary expect the variables that are being changed.
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0, // Optional
            .pInheritanceInfo = NULL // Optional
        };
          

       if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
           printf("failed to begin recording command buffer!\n");
           assert();
       }
        
        VkRenderPassBeginInfo renderPassInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = swapChainFramebuffers[i],
            .renderArea.offset = {0, 0},
            .renderArea.extent = extent
        };
        
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        
//        Vulkan draw command
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        
        vkCmdEndRenderPass(commandBuffers[i]);
        
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            printf("failed to record command buffer!\n");
            assert();
        }
       
    }
    
    return commandBuffers;
}


struct syncAndFence createSyncAndFence(VkDevice device, const int MAX_FRAMES_IN_FLIGHT, int swapChainImagesCount)
{
    
    struct syncAndFence syc;
    
    syc.imageAvailableSemaphore = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    syc.renderFinishedSemaphore = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    syc.inFlightFences = malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);
    syc.imagesInFlight = malloc(sizeof(VkFence) * swapChainImagesCount);

    
//    We have to iterate of imageInFlight to set it up with NULL in each array position
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        syc.imagesInFlight[i] = VK_NULL_HANDLE;
    }
        
    VkSemaphoreCreateInfo semaphoreInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    
    VkFenceCreateInfo fenceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
      
    
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &syc.imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, NULL, &syc.renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, NULL, &syc.inFlightFences[i]) != VK_SUCCESS ) {

            printf("failed to create synchronization objects for a frame!\n");
        }
    }
    
    return syc;
}
