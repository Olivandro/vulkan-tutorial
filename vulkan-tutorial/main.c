
#include "geninc.h"
#include "initVK.h"
#include "deviceVK.h"
#include "graphicFamilyVK.h"
#include "swapChain.h"
#include "shaders.h"
#include "pipeline.h"
#include "commandBuffer.h"


//  Path to shader files, NOTE: create new new compiler macros for each individual shader file.
#define VERTEX_SHADER_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/shaders/shader.vert"
#define FRAGMENT_SHADER_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/shaders/shader.frag"


/**
 Global Variables to this file only
 */
static const char* validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};



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


void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

//    The problem with this function is that tutorial only implements values needed for version.
//    Whereas current version of Vulkan requires all structs to have information filled before reaching submit..
//    Working now..
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
    
    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

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
    
    VkDescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.flags = VK_NULL_HANDLE;
    layoutInfo.pNext = NULL;
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

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

    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = swapChainImagesCount;
    
    VkDescriptorPoolCreateInfo poolInfo;
    poolInfo.flags = VK_NULL_HANDLE;
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = NULL;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = swapChainImagesCount;

    if (vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        printf("failed to create descriptor pool!");
        assert();
    }
    return descriptorPool;
}


VkDescriptorSet* createDescriptorSet(VkDevice device, uint32_t swapChainImagesCount, VkBuffer* uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
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
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(struct UniformBufferObject);
        
        VkWriteDescriptorSet descriptorWrite;
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.pNext = NULL;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = NULL; // Optional
        descriptorWrite.pTexelBufferView = NULL; // Optional
        
        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
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


// Implementing Struct SwapChainObj, going to try and pass reference to see what happens.
// VkSwapchainKHR swapChainKHR, VkCommandBuffer* commandBuffers
//  Pased a reference to swapChainObj to reduce variable overhead.
void drawCall(VkDevice device, VkQueue graphicsQueue, VkExtent2D extent, struct SwapChainObj* swapChainObj, struct syncAndFence syc, VkDeviceMemory* uniformBufferMemory, const int MAX_FRAMES_IN_FLIGHT, clock_t start)
{
    
            static size_t currentFrame = 0;
    //        We tell our program to wait for the fence that we set up in section 9.
            vkWaitForFences(device, 1, &syc.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            
            static uint32_t imageIndex = 0;
            VkResult result = vkAcquireNextImageKHR(device, swapChainObj->swapChainKHR, UINT64_MAX, syc.imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
        
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                printf("Out of date swap chain image!\n");
//                This function does not work currently...
//                recreateSwapChain(device, surface, presentsAnFormatsInfo, queueFamilyIndicesInfo, vertShaderModule, fragShaderModule, swapChainObj);
//                return;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                printf("failed to acquire swap chain image!\n");
            }
    //        We have to make sure that these two values are equal for it'll crash the progarm
            if (currentFrame == imageIndex)
            {
                if (syc.imagesInFlight[imageIndex] != VK_NULL_HANDLE)
                {
                    vkWaitForFences(device, 1, &syc.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
                }
            }
            
    //      Mark the image as now being in use by this frame
            syc.imagesInFlight[imageIndex] = syc.inFlightFences[currentFrame];
    
            updateUniformBuffer(device, extent, imageIndex, start, uniformBufferMemory);
    
            VkSubmitInfo submitInfo =
            {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .commandBufferCount = 1,
                .signalSemaphoreCount = 1
            };
            
            VkSemaphore waitSemaphores[] = {syc.imageAvailableSemaphore[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
    
            submitInfo.pCommandBuffers = &swapChainObj->commandBuffers[currentFrame]; // I had to change this from imageIndex - imageIndex goes to 2 (or 3), while the
//          commandBuffer is the size of 2...

            VkSemaphore signalSemaphores[] = {syc.renderFinishedSemaphore[currentFrame]};
            submitInfo.pSignalSemaphores = signalSemaphores;
            
            vkResetFences(device, 1, &syc.inFlightFences[currentFrame]);
            
            if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syc.inFlightFences[currentFrame]) != VK_SUCCESS) {
                printf("failed to submit draw command buffer!\n");
            }
                
            VkPresentInfoKHR presentInfo =
            {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = signalSemaphores
            };
            
            VkSwapchainKHR swapChains[] = {swapChainObj->swapChainKHR};
            
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = NULL; // Optional
            
            result = vkQueuePresentKHR(graphicsQueue, &presentInfo);
    
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                printf("Out of date swap chain image!\n");
//                This function does not work currently...
//                recreateSwapChain(device, surface, presentsAnFormatsInfo, queueFamilyIndicesInfo, vertShaderModule, fragShaderModule, swapChainObj);
//                return;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                printf("failed to acquire swap chain image!\n");
            }
            
            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}





/**
 This is the main function
 */

int main(void) {
    
    clock_t start = clock();
    
// Opening glfw window setup...
    if (glfwInit() != GLFW_TRUE)
    {
        printf("Error initialising GLFW");
        assert();
    }


    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//    Currently can resize but causes a "Out of date swap chain image!\n" error message.
//    Cannot recreate swap chain at the moment.
    
    static uint32_t width = 800;
    static uint32_t height = 600;
    
//    proceed with creating a GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, WINDOW_NAME, NULL, NULL);
    
 
//    1. Initialising the validation layer.
//    Currently if its shut off the program crashes.
//    TO DO: FIX Disable functionality
    initValidationLayer(validationLayers);
        
//    2. Vulkan instance setup
//    Create our instances
    VkInstance instance = createInstance(validationLayers);
        
//    3.VkPhysicalDevice lets set up the physical devices our program is runnign on.
//    Setting up the physical devices
    VkPhysicalDevice physicalDevice = createPhysicalDevice(instance);

//    4. Create physical device surface
    VkSurfaceKHR surface = createSurface(instance, window);
    
//    5 :: Queue and graphics Family selections.
    struct graphicsFamiliesAnIndices queueFamilyIndicesInfo = createGraFamInd(physicalDevice, surface);

//  6 :: Setting up a logical devices to interface with.
//    This is the main device that is used through out the rest of the program
    VkDevice device = createLogicalDevice(physicalDevice, validationLayers, queueFamilyIndicesInfo.graphicsFamilyIndices, queueFamilyIndicesInfo.queueCount);
    
//   7.  Completing presentation queue
//    Global variable that is required for cleanup
    VkQueue graphicsQueue = createGraphicsQueue(device, queueFamilyIndicesInfo);
    
//    8 : We are now checking for compatibilities we desire and whats possible with the
//    physicalDevice.
    struct availablePresentsAnFormats presentsAnFormatsInfo = findPresentsAnFormats(physicalDevice, surface);

    
/**
 Creation of the swapchain
 */
    
    
//    9. Global struct that pertains to the swapchain
//    VkSwapchainKHR swapChainKHR = createSwapChain(device, surface, presentsAnFormatsInfo, queueFamilyIndicesInfo);
//    Initiallising struct SwapChainObj swapChainObj
    struct SwapChainObj swapChainObj =
    {
        .swapChainKHR = createSwapChain(device, surface, presentsAnFormatsInfo, queueFamilyIndicesInfo)
    };
    
/**
 This block pertains to    createImageViews(); from tutorial
 */
//  10. setup swapchain images
//    VkImageView* swapChainImageViews = createImageView(device, swapChainKHR, presentsAnFormatsInfo);
    swapChainObj.swapChainImageViews = createImageView(device, swapChainObj.swapChainKHR, presentsAnFormatsInfo);
//    Had to do this... its an incredibly important variable that I constantly use.
//    int swapChainImagesCount = findSwapChainImageCount(device, swapChainKHR, presentsAnFormatsInfo);
    swapChainObj.swapChainImagesCount = findSwapChainImageCount(device, swapChainObj.swapChainKHR, presentsAnFormatsInfo);
    
/**
 Creating the graphics pipeline - includes 5 steps
 */
//    11 :: Graphics pipeline :: SHADERS
//    After a long arguous process, we are finally at the graphics pipe line!

//    this whole section can be abstracted into functions and return a single variable
//    Functions needed: (1) parseShaders; (2) compileShaders; (3) createShaderProgram

    
//    TO DO:
//    Add Vertex data here using the Vertex struct create above. Cast it here!!
    
     
    /**
            Creating the vertex buffer for drawing data coordinates.
     */
     struct DrawingData vertices[4] =
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    
    uint16_t indices[6] =
    {
        0, 1, 2, 2, 3, 0
    };
    

     
     
    
//    11.1 : Shaders (Vertex Shader first)::
    VkShaderModule vertShaderModule = createShaderProgram(device, VERTEX_SHADER_FILE_PATH, "main.vert", "main", 0);

    //    Shaders (Vertex Shader first)::
    VkShaderModule fragShaderModule = createShaderProgram(device, FRAGMENT_SHADER_FILE_PATH, "main.frag", "main", 1);

    
//    Final shader stage info for graphics pipeline
//    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
//    VkPipelineShaderStageCreateInfo* shaderStages = CreateShaderStages(vertShaderModule, fragShaderModule);
    swapChainObj.shaderStages = CreateShaderStages(vertShaderModule, fragShaderModule);
    
//    11.2 These are the fixing functions... i.e. we create the data structs for initialising the
//    Graphics pipeline and layout.
//    This is the next major step
//    In Vulkan you have to be explicit about everything, from viewport size to color blending function.
   
    
    /**
        To insert: VkDescriptorSetLayout descriptorSetLayout --in this location.
        NOTE: should this be in SwapChainObj??
     */
    VkDescriptorSetLayout descriptorSetLayout = createDescriptorSetLayout(device);
    
//    11.3 : Creating the graphics pipeline
//    VkPipelineLayout pipelineLayout = createPipelineLayout(device);
    swapChainObj.pipelineLayout = createPipelineLayout(device, descriptorSetLayout);
    
    
//    11.4 : Creating the rendering pass
//    VkRenderPass renderPass = createRenderingPass(device, presentsAnFormatsInfo.availableFormats.format);
    swapChainObj.renderPass = createRenderingPass(device, presentsAnFormatsInfo.availableFormats.format);

//    11.5 Create the Graphics pipeline
//    TO DO:
//    Vertex data input will happen here. Have to decide how to implement it.. i.e. by reference or single internal
//    verible..
//    VkPipeline graphicsPipeline = createGraphicsPipeline(device, pipelineLayout, renderPass, presentsAnFormatsInfo.extent, shaderStages);
    swapChainObj.graphicsPipeline = createGraphicsPipeline(device, swapChainObj.pipelineLayout, swapChainObj.renderPass, presentsAnFormatsInfo.extent, swapChainObj.shaderStages);
    
/**
 Create the Command buffer, frame buffer and render pass command pool
 NOTE: This is were the errors in running the program are
 */
//    12 : setting up the command buffer and frame buffers plue the render
//    pass command pool
//    VkFramebuffer* swapChainFramebuffers = createSwapChainFramebuffers(device, swapChainImageViews, swapChainImagesCount, renderPass, presentsAnFormatsInfo.extent);
    swapChainObj.swapChainFramebuffers = createSwapChainFramebuffers(device, swapChainObj.swapChainImageViews, swapChainObj.swapChainImagesCount, swapChainObj.renderPass, presentsAnFormatsInfo.extent);
    
    
//    12.2 : Lets create the commandpool now..
//    VkCommandPool commandPool = createCommandPool(device, queueFamilyIndicesInfo.presentFamily);
    swapChainObj.commandPool = createCommandPool(device, queueFamilyIndicesInfo.presentFamily);


    
/**
    This is the vertex buffer
 */
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * sizeof(vertices);
    
    
    /**
        These 3 functions are combined into on function in the Vulkan tutorial...
     */
//    this is the staging buffer
    VkBuffer stagingBuffer = createVertexBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    VkDeviceMemory stagingBufferMemory = createVertexBufferMemory(device, physicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);
    /**
        End of.. function in the Vulkan tutorial...
     */
    
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);
    
    /**
        These 3 functions are combined into on function in the Vulkan tutorial...
     */
    //    This is the vertext buffer
    VkBuffer vertexBuffer = createVertexBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    VkDeviceMemory vertexBufferMemory = createVertexBufferMemory(device, physicalDevice, vertexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
    /**
        End of.. function in the Vulkan tutorial...
     */
    
    copyBuffer(device, swapChainObj.commandPool, graphicsQueue, stagingBuffer, vertexBuffer, bufferSize);
    
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
    
    /**
            End of Vertex Buffer implementation...
     */
    
    VkDeviceSize indicesBufferSize = sizeof(indices[0]) * sizeof(indices);

    /**
        These 3 functions are combined into on function in the Vulkan tutorial...
     */
//    this is the indices staging buffer
    VkBuffer indicesStagingBuffer = createVertexBuffer(device, indicesBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    VkDeviceMemory indicesStagingBufferMemory = createVertexBufferMemory(device, physicalDevice, indicesStagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkBindBufferMemory(device, indicesStagingBuffer, indicesStagingBufferMemory, 0);
    /**
        End of.. function in the Vulkan tutorial...
     */
    

    void* indicesdData;
    vkMapMemory(device, indicesStagingBufferMemory, 0, indicesBufferSize, 0, &indicesdData);
    memcpy(indicesdData, indices, indicesBufferSize);
    vkUnmapMemory(device, indicesStagingBufferMemory);

    
    /**
        These 3 functions are combined into on function in the Vulkan tutorial...
     */
//    this is the indices buffer
    VkBuffer indicesBuffer = createVertexBuffer(device, indicesBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    VkDeviceMemory indicesBufferMemory = createVertexBufferMemory(device, physicalDevice, indicesBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkBindBufferMemory(device, indicesBuffer, indicesBufferMemory, 0);
    /**
        End of.. function in the Vulkan tutorial...
     */

    copyBuffer(device, swapChainObj.commandPool, graphicsQueue, indicesStagingBuffer, indicesBuffer, indicesBufferSize);

    vkDestroyBuffer(device, indicesStagingBuffer, NULL);
    vkFreeMemory(device, indicesStagingBufferMemory, NULL);
    
    /**
            End of Index Buffer implementation...
     */
    
    VkBuffer uniformBuffers[swapChainObj.swapChainImagesCount];
    VkDeviceMemory uniformBufferMemory[swapChainObj.swapChainImagesCount];
    
    createUniformBuffers(device, physicalDevice, swapChainObj.swapChainImagesCount, uniformBuffers, uniformBufferMemory);
    
    
    
    VkDescriptorPool descriptorPool = createDescriptorPool(device, swapChainObj.swapChainImagesCount);
    VkDescriptorSet* descriptorSets = createDescriptorSet(device, swapChainObj.swapChainImagesCount, uniformBuffers, descriptorSetLayout, descriptorPool);
    
    
//    12.3 : Creating the command buffer
//    VkCommandBuffer* commandBuffers = createCommandBuffers(device, renderPass, graphicsPipeline, swapChainFramebuffers, commandPool, swapChainImagesCount, presentsAnFormatsInfo.extent);
    swapChainObj.commandBuffers = createCommandBuffers(device, swapChainObj.renderPass, swapChainObj.graphicsPipeline, swapChainObj.pipelineLayout, swapChainObj.swapChainFramebuffers, swapChainObj.commandPool, swapChainObj.swapChainImagesCount, presentsAnFormatsInfo.extent, vertexBuffer, indicesBuffer, (uint32_t) sizeof(indices), descriptorSets);
        
    
//  13 : rasterisation and Presentation of the triangle that we are drawing
//    Both variables are requested throughout the rest of the program
    const int MAX_FRAMES_IN_FLIGHT = 2;
    
    struct syncAndFence syc = createSyncAndFence(device, MAX_FRAMES_IN_FLIGHT, swapChainObj.swapChainImagesCount);
    
    


//    Main loop
//        This is the game loops
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        /**
        Draw call
         */
        drawCall(device, graphicsQueue, presentsAnFormatsInfo.extent, &swapChainObj, syc, uniformBufferMemory, MAX_FRAMES_IN_FLIGHT, start);
//        drawCall(device, graphicsQueue, presentsAnFormatsInfo.extent, &swapChainObj, syc, MAX_FRAMES_IN_FLIGHT);
        
    }

    
    if (vkDeviceWaitIdle(device) == VK_SUCCESS)
    {

        // Swapchain clean up
        cleanUpSwapChain(device, swapChainObj);
        
        // Cleanup uniform buffers
        for (size_t i = 0; i < (size_t) swapChainObj.swapChainImagesCount; i++)
        {
                vkDestroyBuffer(device, uniformBuffers[i], NULL);
                vkFreeMemory(device, uniformBufferMemory[i], NULL);
        }
//        free(uniformBuffers);
//        free(uniformBufferMemory);
        
        vkDestroyDescriptorPool(device, descriptorPool, NULL);
        
        // Cleanup vkDestroyDescriptorSetLayout
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);

        // Cleanup for indicesBuffers
        vkDestroyBuffer(device, indicesBuffer, NULL);
        vkFreeMemory(device, indicesBufferMemory, NULL);
        
        // Cleanup for VertexBuffer and memory allocation.
        vkDestroyBuffer(device, vertexBuffer, NULL);
        vkFreeMemory(device, vertexBufferMemory, NULL);
        // End.
        
        // Program clean up
        // Will need to figure out how to abstract this effectively
        free(syc.imagesInFlight);
        
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(device, syc.renderFinishedSemaphore[i], NULL);
            vkDestroySemaphore(device, syc.imageAvailableSemaphore[i], NULL);
            vkDestroyFence(device, syc.inFlightFences[i], NULL);
        }
        free(syc.imageAvailableSemaphore);
        free(syc.renderFinishedSemaphore);
        free(syc.inFlightFences);

        // Cleanup for commandpool
        vkDestroyCommandPool(device, swapChainObj.commandPool, NULL);
        
    
    
        vkDestroyShaderModule(device, vertShaderModule, NULL);
        vkDestroyShaderModule(device, fragShaderModule, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    else
        return -1;
    
    return 0;
}
