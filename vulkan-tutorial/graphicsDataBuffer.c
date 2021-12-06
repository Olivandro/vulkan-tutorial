//
//  graphicsDataBuffer.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 28/11/2021.
//

#include "graphicsDataBuffer.h"

VkBuffer createVertexBuffer(VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags usage)
{
    VkBuffer vertexBuffer;
    VkBufferCreateInfo bufferInfo =
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferSize,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    

    if (vkCreateBuffer(device, &bufferInfo, NULL, &vertexBuffer) != VK_SUCCESS) {
        printf("failed to create vertex buffer!\n");
        assert();
    }
    
    return vertexBuffer;
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties,  VkPhysicalDeviceMemoryProperties memProperties)
{
    uint32_t memoryType = 0;
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memoryType = i;
            return memoryType;
        }
    }
    printf("failed to find suitable memory type!\n");
    assert();
}

VkDeviceMemory createVertexBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer vertexBuffer, VkMemoryPropertyFlags properties)
{
    VkDeviceMemory vertexBufferMemory;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo =
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties, memProperties)
    };
    
    if (vkAllocateMemory(device, &allocInfo, NULL, &vertexBufferMemory) != VK_SUCCESS) {
        printf("failed to allocate vertex buffer memory!\n");
        assert();
    }
    return vertexBufferMemory;
}


VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.pNext = NULL;
    beginInfo.pInheritanceInfo = NULL;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    return commandBuffer;
}

void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.pNext = NULL;
    submitInfo.pSignalSemaphores = NULL;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.waitSemaphoreCount = 0;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{

//    The problem with this function is that tutorial only implements values needed for version.
//    Whereas current version of Vulkan requires all structs to have information filled before reaching submit..
//    Working now..
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
    
    
    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

//    One function
    endSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);

}


/**
 Uniform buffer
 */
VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = NULL;
    
    VkDescriptorSetLayoutBinding samplerLayoutBinding;
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = NULL;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    VkDescriptorSetLayoutBinding bindings[2] = { uboLayoutBinding, samplerLayoutBinding };
    
    VkDescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.flags = VK_NULL_HANDLE;
    layoutInfo.pNext = NULL;
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = (uint32_t) sizeof(bindings) / sizeof(bindings[0]);//(uint32_t) sizeof(bindings); // 1;
    layoutInfo.pBindings = bindings; // &uboLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS) {
        printf("failed to create descriptor set layout!\n");
        assert();
    }
    
    return descriptorSetLayout;
}

void createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t swapChainImagesCount, VkBuffer* uniformBuffers, VkDeviceMemory* uniformBufferMemory)
{
    VkDeviceSize bufferSize = sizeof(struct UniformBufferObject);


    for (size_t i = 0; i < (size_t) swapChainImagesCount; i++) {
        
        uniformBuffers[i] = createVertexBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        uniformBufferMemory[i] = createVertexBufferMemory(device, physicalDevice, uniformBuffers[i], VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(device, uniformBuffers[i], uniformBufferMemory[i], 0);
        
    }
}


VkDescriptorPool createDescriptorPool(VkDevice device, uint32_t swapChainImagesCount)
{
    VkDescriptorPool descriptorPool;

    VkDescriptorPoolSize poolSize[2];
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize[0].descriptorCount = swapChainImagesCount;
    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount = swapChainImagesCount;
  
    
    
    VkDescriptorPoolCreateInfo poolInfo;
    poolInfo.flags = VK_NULL_HANDLE;
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = NULL;
    poolInfo.poolSizeCount = 2; //(uint32_t) sizeof(poolSize); // 1;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = swapChainImagesCount;

    if (vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        printf("failed to create descriptor pool!");
        assert();
    }
    return descriptorPool;
}






VkDescriptorSet* createDescriptorSet(VkDevice device, uint32_t swapChainImagesCount, VkBuffer* uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, VkImageView textureImageView, VkSampler textureSampler)
{

    VkDescriptorSetLayout layouts[swapChainImagesCount];
    for (size_t i = 0; i < (size_t) swapChainImagesCount; i++) {
        layouts[i] = descriptorSetLayout;
    }
    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.pNext = NULL;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = swapChainImagesCount;
    allocInfo.pSetLayouts = layouts;
    
    VkDescriptorSet* descriptorSets = malloc((size_t) swapChainImagesCount);

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets) != VK_SUCCESS) {
        printf("failed to allocate descriptor sets!\n");
        assert();
    }

    for (size_t i = 0; i < (size_t) swapChainImagesCount; i++) {
        const VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = uniformBuffers[i],
            .offset = 0,
            .range = sizeof(struct UniformBufferObject)
        };
        
        const VkDescriptorImageInfo imageInfo =
        {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = textureImageView,
            .sampler = textureSampler
        };
        
        
        VkWriteDescriptorSet descriptorWrite[2];
        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].pNext = NULL;
        descriptorWrite[0].dstSet = descriptorSets[i];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pBufferInfo = &bufferInfo;
//            descriptorWrite[0].pImageInfo = VK_NULL_HANDLE,
//            descriptorWrite[0].pTexelBufferView = VK_NULL_HANDLE,
            
        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].pNext = NULL;
        descriptorWrite[1].dstSet = descriptorSets[i];
        descriptorWrite[1].dstBinding = (uint32_t) 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pImageInfo = &imageInfo;

        uint32_t descriptorSetsSize = (uint32_t) sizeof(descriptorWrite) / sizeof(descriptorWrite[0]);
        vkUpdateDescriptorSets(device, descriptorSetsSize, descriptorWrite, 0, NULL);
    }
    
    return descriptorSets;
    
}


void updateUniformBuffer(VkDevice device, VkExtent2D extent, uint32_t currentImage, clock_t start, VkDeviceMemory* uniformBufferMemory)
{

    clock_t current = clock() - start;
    float time = (float) current / CLOCKS_PER_SEC;
    
    struct UniformBufferObject ubo;
    mat4x4_identity(ubo.model);
    mat4x4_rotate(ubo.model, ubo.model, 0.0f, 0.0f, 1.0f, time * (90.0f / 57.29578f));
    mat4x4_look_at(ubo.view, (vec3){2.0f, 2.0f, 2.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f});
    mat4x4_perspective(ubo.proj, (45.0f / 57.29578f), extent.width / (float) extent.height, 0.1f, 10.0f);
    
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(device, uniformBufferMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, uniformBufferMemory[currentImage]);

}
