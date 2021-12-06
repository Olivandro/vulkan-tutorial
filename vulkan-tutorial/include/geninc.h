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
#include <stddef.h>
// This is required for offsetof() Marco
#include <time.h>
#include <stdalign.h>

/**
 Maths Library: https://github.com/datenwolf/linmath.h
 TO DO: move to geninc.h
 */
#include "linmath.h"


//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif


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

struct DrawingData {
    vec2 pos;
    vec3 color;
    vec2 texCoord;
};

struct UniformBufferObject {
    
    _Alignas(16) mat4x4 model;
    _Alignas(16) mat4x4 view;
    _Alignas(16) mat4x4 proj;
    
};

struct SwapChainObj {
    VkSwapchainKHR swapChainKHR;
    VkImageView* swapChainImageViews;
    int swapChainImagesCount; // fix to uint32_t
    VkPipelineShaderStageCreateInfo* shaderStages;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;
    VkFramebuffer* swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
};

struct syncAndFence
{
    VkSemaphore* imageAvailableSemaphore;
    VkSemaphore* renderFinishedSemaphore;
    VkFence* inFlightFences;
    VkFence* imagesInFlight;
};

#endif /* geninc_h */
