//
//  drawcall.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#include "drawcall.h"


//void drawCall(VkDevice device,  VkQueue graphicsQueue, VkSwapchainKHR swapChainKHR, VkCommandBuffer* commandBuffers, struct syncAndFence syc, const int MAX_FRAMES_IN_FLIGHT)
//{
//    
//            static size_t currentFrame = 0;
//    //        We tell our program to wait for the fence that we set up in section 9.
//            vkWaitForFences(device, 1, &syc.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
//            
//            
//            static uint32_t imageIndex = 0;
//            vkAcquireNextImageKHR(device, swapChainKHR, UINT64_MAX, syc.imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
//        
//    //        We have to make sure that these two values are equal for it'll crash the progarm
//            if (currentFrame == imageIndex)
//            {
//                if (syc.imagesInFlight[imageIndex] != VK_NULL_HANDLE)
//                {
//                    vkWaitForFences(device, 1, &syc.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
//                }
//            }
//            
//    //      Mark the image as now being in use by this frame
//            syc.imagesInFlight[imageIndex] = syc.inFlightFences[currentFrame];
//            
//            VkSubmitInfo submitInfo =
//            {
//                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
//                .waitSemaphoreCount = 1,
//                .commandBufferCount = 1,
//                .signalSemaphoreCount = 1
//            };
//            
//            VkSemaphore waitSemaphores[] = {syc.imageAvailableSemaphore[currentFrame]};
//            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
//            submitInfo.pWaitSemaphores = waitSemaphores;
//            submitInfo.pWaitDstStageMask = waitStages;
//    
//            
//            submitInfo.pCommandBuffers = &commandBuffers[currentFrame]; // I had to change this from imageIndex - imageIndex goes to 2 (or 3), while the
////          commandBuffer is the size of 2...
//
//            VkSemaphore signalSemaphores[] = {syc.renderFinishedSemaphore[currentFrame]};
//            submitInfo.pSignalSemaphores = signalSemaphores;
//            
//            vkResetFences(device, 1, &syc.inFlightFences[currentFrame]);
//            
//            if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syc.inFlightFences[currentFrame]) != VK_SUCCESS) {
//                printf("failed to submit draw command buffer!\n");
//            }
//                
//            
//            VkPresentInfoKHR presentInfo =
//            {
//                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
//                .waitSemaphoreCount = 1,
//                .pWaitSemaphores = signalSemaphores
//            };
//            
//
//            VkSwapchainKHR swapChains[] = {swapChainKHR};
//            
//            presentInfo.swapchainCount = 1;
//            presentInfo.pSwapchains = swapChains;
//            presentInfo.pImageIndices = &imageIndex;
//            presentInfo.pResults = NULL; // Optional
//            
//            vkQueuePresentKHR(graphicsQueue, &presentInfo);
//            
//            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//
//}
