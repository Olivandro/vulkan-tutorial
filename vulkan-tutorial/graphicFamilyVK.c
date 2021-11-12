//
//  graphicFamilyVK.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#include "graphicFamilyVK.h"


struct graphicsFamiliesAnIndices createGraFamInd(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
        uint32_t graphicsFamilyIndices = 0;
        uint32_t queueCount = 0;
        uint32_t queueFamilyCount = 0;
        
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

        VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
        if (queueFamilyCount >= 1)
        {
            printf("%d Family queues found\n", queueFamilyCount);
    //        for (int i = 0; i < queueFamilyCount; i++) {
    //            printf("Family queue num: %d has %u queues within it.\n", i+1, queueFamilies[i].queueCount);
    //        }
            for (int i = 0; i < queueFamilyCount; i++)
            {
                if (queueFamilies[i].queueFlags && VK_QUEUE_GRAPHICS_BIT)
                {
                        graphicsFamilyIndices = i;
                        queueCount = queueFamilies[i].queueCount;
                        printf("Graphics Family indices: %d \n", graphicsFamilyIndices);
                        printf("Queue Flags: %u \n", queueFamilies[i].queueFlags);
                        break;
                }
            }
        }
        free(queueFamilies);

    //    Global varible that is used throughout various structs
        uint32_t presentFamily = 0;
        
    //    Not used past this below code block
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndices, surface, &presentSupport);
        if (presentSupport)
        {
    //        Indicate what is the present family being used which also has support.
            presentFamily = graphicsFamilyIndices;
            printf("Present graphics family being used: %u\n", presentFamily);
        }
//        else
//            presentFamily = 0;

        
        struct graphicsFamiliesAnIndices queueFamilyIndicesInfo =
        {
            .graphicsFamilyIndices = graphicsFamilyIndices,
            .queueCount = queueCount,
            .presentFamily = presentFamily,
            .queueFamilyIndices = {
                graphicsFamilyIndices,
                presentFamily
            }
            
        };
        
        return queueFamilyIndicesInfo;
}



VkQueue createGraphicsQueue(VkDevice device, struct graphicsFamiliesAnIndices queueFamilyIndicesInfo)
{
        VkQueue graphicsQueue;
        vkGetDeviceQueue(device, queueFamilyIndicesInfo.presentFamily, queueFamilyIndicesInfo.graphicsFamilyIndices, &graphicsQueue);
        return graphicsQueue;
}



struct availablePresentsAnFormats findPresentsAnFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    
        VkSurfaceCapabilitiesKHR capabilities;
        //    Temp trn result for process.
        VkResult surfaceCapabilitesResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        if (surfaceCapabilitesResult != VK_SUCCESS)
        {
            printf("Surface capabilities initialisation failed.\n");
            printf("return code: %u\n", surfaceCapabilitesResult);
            
        }
 
        VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * 1);
        VkPresentModeKHR* presentModes = malloc(sizeof(VkPresentModeKHR) * 1);
    //    Temp function that is only used in this code block.
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
    //    Global struct variable
        struct availablePresentsAnFormats presentsAnFormatsInfo;
//        VkSurfaceFormatKHR availableFormats;
        if (formatCount != 0) {
            formats = realloc(formats, sizeof(VkSurfaceFormatKHR) * formatCount);
    //        temp rtn result function.
            VkResult surfaceFormatResult = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);
            if (surfaceFormatResult != VK_SUCCESS) {
                printf("Something went wrong...\n");
                printf("Return code: %u\n", surfaceFormatResult);
                
//                free(formats);
//                free(presentModes);
//                return -1;
            }
            else
            {
                for (int i = 0; i < formatCount; i++)
                {
                    if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    {
                        presentsAnFormatsInfo.availableFormats = formats[i];
                        printf("Format VK_FORMAT_B8G8R8A8_SRGB and colorspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR found!\n");
                        break;
                    }
                    
                }
            }
        }
        
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
        
        if (presentModeCount != 0) {
            presentModes = realloc(presentModes, sizeof(VkPresentModeKHR) * presentModeCount);
            VkResult surfacePresentModeResult = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);
            if (surfacePresentModeResult != VK_SUCCESS) {
                printf("Something went wrong...\n");
                printf("Return code: %u\n", surfacePresentModeResult);
//                free(presentModes);
//                return -1;
            }
            else
            {
                for (int i = 0; i < presentModeCount; i++)
                {
                    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        presentsAnFormatsInfo.avaiablePresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        printf("VK_PRESENT_MODE_MAILBOX_KHR present mode selected\n");
                        break;
                    }

                }
                presentsAnFormatsInfo.avaiablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
                printf("VK_PRESENT_MODE_FIFO_KHR present mode selected\n");
    //            avaiablePresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    //            printf("%u\n", avaiablePresentMod);
            }
        }
        else
        {
            printf("Your system might not be Vulkan compatible, please double check your system.\n");
            
//            free(formats);
//            free(presentModes);
//            return -1;
        }

    //    free memory allocation for formats and presentMode varibles
        free(formats);
        free(presentModes);
    
        
        /**
         This block can be added to the above availablePresentsAnFormats struct
         */
    
        presentsAnFormatsInfo.currentTransform = capabilities.currentTransform;
    //    capabilities is required for this statement. However this
    //    statement is also not doing anything
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            printf("\ncapabilities.currentExtent.width != UINT32_MAX...\n");
            printf("Still to check if this is bad...\n\n");
        }
        
    //    STEP 6.2:: Setting up the action swap chain functionality
    //    Remember we initialised this extension before in step 5.
        
    //    This is a global variable - its also using capabilities
        presentsAnFormatsInfo.extent = capabilities.currentExtent;
        
    //  This is a reoccuring variable that is used to fill several
    //    data points in other structs
    presentsAnFormatsInfo.imageCount = capabilities.minImageCount; // + 1;
        
        if (capabilities.maxImageCount > 0 && presentsAnFormatsInfo.imageCount > capabilities.maxImageCount)
        {
            presentsAnFormatsInfo.imageCount = capabilities.maxImageCount;
        }
        
        /**
         End of this code block note
         */
    
        return presentsAnFormatsInfo;
}
