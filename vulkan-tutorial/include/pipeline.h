//
//  pipeline.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#ifndef pipeline_h
#define pipeline_h

#include "geninc.h"

VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);

VkRenderPass createRenderingPass(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat VkColourFormat);

VkPipeline createGraphicsPipeline(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkExtent2D extent, VkPipelineShaderStageCreateInfo shaderStages[]);



#endif /* pipeline_h */
