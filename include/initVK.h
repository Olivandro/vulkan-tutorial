//
//  initVK.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 21/12/2020.
//

#ifndef initVK_h
#define initVK_h

#include "geninc.h"

VkBool32 initValidationLayer(const char* validationLayers);

VkInstance createInstance(const char* validationLayers);

#endif /* initVK_h */
