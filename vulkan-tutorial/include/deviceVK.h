//
//  deviceVK.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef deviceVK_h
#define deviceVK_h

#include "geninc.h"

bool isDeviceSuitable(VkPhysicalDevice device);

VkPhysicalDevice createPhysicalDevice(VkInstance instance);

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, const char* validationLayers, uint32_t graphicsFamilyIndices, uint32_t queueCount);

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window);

#endif /* deviceVK_h */
