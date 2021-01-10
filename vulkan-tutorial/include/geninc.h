//
//  geninc.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 05/01/2021.
//

#ifndef geninc_h
#define geninc_h

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ENABLE_VALIDATION_LAYERS true
#define APP_NAME "Hello Triangle"
#define ENGINE_NAME "No Engine"
#define WINDOW_NAME "Vulkan window"

//We redefine assert for our own purposes
#define assert() return -1


/**
 Vulkan data Structs 
 */
struct graphicsFamiliesAnIndices
{
    uint32_t graphicsFamilyIndices;
    uint32_t queueCount;
    uint32_t presentFamily;
    uint32_t queueFamilyIndices[2];
    
};

struct availablePresentsAnFormats
{
    VkSurfaceFormatKHR availableFormats;
    VkPresentModeKHR avaiablePresentMode;
    VkExtent2D extent;
    VkSurfaceTransformFlagBitsKHR currentTransform;
    uint32_t imageCount;
};

struct syncAndFence
{
    VkSemaphore* imageAvailableSemaphore;
    VkSemaphore* renderFinishedSemaphore;
    VkFence* inFlightFences;
    VkFence* imagesInFlight;
};

#endif /* geninc_h */
