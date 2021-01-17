//
//  pipeline.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#include "pipeline.h"


VkPipelineLayout createPipelineLayout(VkDevice device)
{
    VkPipelineLayout pipelineLayout;
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0, // Optional
        .pSetLayouts = NULL, // Optional
        .pushConstantRangeCount = 0, // Optional
        .pPushConstantRanges = NULL // Optional
    };
    
    
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        printf("failed to create pipeline layout!\n");
    }
    
    return pipelineLayout;
}



VkRenderPass createRenderingPass(VkDevice device, VkFormat VkColourFormat)
{
    VkRenderPass renderPass;
    
//    This is an exact variable... Can be cut and instead etend availableFormats
//    struct type
    VkAttachmentDescription colorAttachment =
    {
        .format = VkColourFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    
    VkAttachmentReference colorAttachmentRef =
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    
    VkSubpassDescription subpass =
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef
    };
    
    VkRenderPassCreateInfo renderPassInfo =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass

    };
    
//    Dependacy can actually happen before renderpass info
    VkSubpassDependency dependency =
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcAccessMask = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };
    
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
//    Creation of the render pass
    if (vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        printf("failed to create render pass!\n");
        assert();
    }
    
    return renderPass;
}


VkPipeline createGraphicsPipeline(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkExtent2D extent, VkPipelineShaderStageCreateInfo shaderStages[])
{
////    EVERYTHING IS REQUIRED FOR THE GRAPHICS PIPELINE
//    Below we are establishing loads of settings for our rendering pipeline
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL, // Optional
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL // Optional
    };
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };
    
    VkViewport viewport =
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) extent.width,
        .height = (float) extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor =
    {
        .offset = {0, 0},
        .extent = extent
    };
    
    VkPipelineViewportStateCreateInfo viewportState =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };
    
    VkPipelineRasterizationStateCreateInfo rasterizer =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f, // Optional
        .depthBiasClamp = 0.0f, // Optional
        .depthBiasSlopeFactor = 0.0f // Optional
    };
    
    VkPipelineMultisampleStateCreateInfo multisampling =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f, // Optional
        .pSampleMask = NULL, // Optional
        .alphaToCoverageEnable = VK_FALSE, // Optional
        .alphaToOneEnable = VK_FALSE // Optional
    };
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment =
    {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .colorBlendOp = VK_BLEND_OP_ADD, // Optional
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .alphaBlendOp = VK_BLEND_OP_ADD // Optional
    };
    
    VkPipelineColorBlendStateCreateInfo colorBlending =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, // Optional
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants[0] = 0.0f, // Optional
        .blendConstants[1] = 0.0f, // Optional
        .blendConstants[2] = 0.0f, // Optional
        .blendConstants[3] = 0.0f // Optional
    };
    
////    END OF FIXING FUNCTION FOR GRAPHICS PIPELINE
    
    
//    This is the creation of the graphics pipeline
//    This struct below uses several structs established before
//    ... could I combine and abstract this setup?
    VkGraphicsPipelineCreateInfo graphicsPipelineInfo =
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = NULL,
        .pColorBlendState = &colorBlending,
        .pDynamicState = NULL,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };
    
    VkPipeline graphicsPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
        printf("failed to create graphics pipeline!\n");
        assert();
    }
    return graphicsPipeline;
}
