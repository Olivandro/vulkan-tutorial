//
//  shaders.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 24/12/2020.
//

#ifndef shaders_h
#define shaders_h

#include "geninc.h"

#include <shaderc/shaderc.h>





VkShaderModule createShaderProgram(VkDevice device, char* shaderPath, const char* input_file_name, const char* entry_point_name, int shaderType);


#endif /* shaders_h */