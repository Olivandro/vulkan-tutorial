//
//  graphicFamilyVK.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef graphicFamilyVK_h
#define graphicFamilyVK_h

#include "geninc.h"


struct graphicsFamiliesAnIndices createGraFamInd(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

VkQueue createGraphicsQueue(VkDevice device, struct graphicsFamiliesAnIndices queueFamilyIndicesInfo);

struct availablePresentsAnFormats findPresentsAnFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);




#endif /* graphicFamilyVK_h */
