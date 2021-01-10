//
//  pipeline.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef pipeline_h
#define pipeline_h

#include "geninc.h"

VkPipelineLayout createPipelineLayout(VkDevice device);

VkRenderPass createRenderingPass(VkDevice device, VkFormat VkColourFormat);

VkPipeline createGraphicsPipeline(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkExtent2D extent, VkPipelineShaderStageCreateInfo shaderStages[]);



#endif /* pipeline_h */
