
#include "geninc.h"
#include "initVK.h"
#include "deviceVK.h"
#include "graphicFamilyVK.h"
#include "swapChain.h"
#include "shaders.h"
#include "pipeline.h"
#include "graphicsDataBuffer.h"
#include "commandBuffer.h"




//  Path to shader files, NOTE: create new new compiler macros for each individual shader file.
#define VERTEX_SHADER_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/shaders/shader.vert"
#define FRAGMENT_SHADER_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/shaders/shader.frag"


#define TEST_TEXTURE_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/assets/statue.jpg"


/**
 Global Variables to this file only
 */
static const char* validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};


// Depth buffer functions

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    foreach(VkFormat format, candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    
    assert();
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
{
    const VkFormat candidates[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    VkFormat format = findSupportedFormat(physicalDevice, candidates,VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    return format;
}

void createDepthResources(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, uint32_t width, uint32_t height, VkImage* depthImage, VkDeviceMemory* depthImageMemory, VkImageView* depthImageView)
{
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT; // findDepthFormat(physicalDevice);
    
//    Create image & memory values
    VkImageCreateInfo imageInfo;
    imageInfo.pNext = NULL;
    imageInfo.flags = VK_NULL_HANDLE;
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, NULL, depthImage) != VK_SUCCESS) {
        printf("failed to create image!\n");
    }

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *depthImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.pNext = NULL;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memProperties);

    if (vkAllocateMemory(device, &allocInfo, NULL, depthImageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!\n");
    }

    vkBindImageMemory(device, *depthImage, *depthImageMemory, 0);
    //    End of image & memory values
    
    VkImageView tempDepthImageView = VK_NULL_HANDLE;
    createTextureImageView(device, *depthImage, depthFormat, &tempDepthImageView, VK_IMAGE_ASPECT_DEPTH_BIT);
    *depthImageView = tempDepthImageView;
    
    transitionImageLayout(device, commandPool, graphicsQueue, *depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

/**
 VkImage depthImage = VK_NULL_HANDLE;
 VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
 VkImageView depthImageView = VK_NULL_HANDLE;
 */

/**
 This is the main draw function
 */

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
//    VkImageView* swapChainImageViews
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
     struct DrawingData vertices[8] =
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        
    };
    
    uint16_t indices[12] =
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
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
    swapChainObj.renderPass = createRenderingPass(device, physicalDevice, presentsAnFormatsInfo.availableFormats.format);

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

    
    
//    12.2 : Lets create the commandpool now..
//    VkCommandPool commandPool = createCommandPool(device, queueFamilyIndicesInfo.presentFamily);
    swapChainObj.commandPool = createCommandPool(device, queueFamilyIndicesInfo.presentFamily);

    
/**
    Depth Buffing
 */
    
    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;
    createDepthResources(device, physicalDevice, swapChainObj.commandPool, graphicsQueue, presentsAnFormatsInfo.extent.width, presentsAnFormatsInfo.extent.height, &depthImage, &depthImageMemory, &depthImageView);
    
    
/**
    End of Depth Buffing
 */
    
//    12 : setting up the command buffer and frame buffers plue the render
//    pass command pool
//    VkFramebuffer* swapChainFramebuffers = createSwapChainFramebuffers(device, swapChainImageViews, swapChainImagesCount, renderPass, presentsAnFormatsInfo.extent);
    swapChainObj.swapChainFramebuffers = createSwapChainFramebuffers(device, depthImageView, swapChainObj.swapChainImageViews, swapChainObj.swapChainImagesCount, swapChainObj.renderPass, presentsAnFormatsInfo.extent);
    
/**
    Texture Mapping
    NOTE:: The assignment and subsequent initalisation of the text values below is the preferred way I believe
    to initialised all the VK variables.
 
    TO DO:
    1. Finish texture implementation.
    2. Re write buffer code to match this control flow.
 */
    
    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;

    
    createTextureImage(device, physicalDevice, swapChainObj.commandPool, graphicsQueue, &textureImage, &textureImageMemory, TEST_TEXTURE_FILE_PATH);
    
    
    
    VkImageView textureImageView = VK_NULL_HANDLE;
    createTextureImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, &textureImageView, VK_IMAGE_ASPECT_COLOR_BIT);
    
    
    VkSampler textureSampler = VK_NULL_HANDLE;
    createTextureSampler(device, physicalDevice, &textureSampler);
    
/**
    END Texture Mapping
 */
    
    
/**
    This is the vertex buffer
 */
    
    VkDeviceSize bufferSize = (sizeof(vertices) / sizeof(vertices[0])) * sizeof(vertices);
    
    
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
    memcpy(data, vertices, (size_t) bufferSize);
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
    
    VkDeviceSize indicesBufferSize = (sizeof(indices) / sizeof(indices[0])) * sizeof(indices);

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
    VkDescriptorSet* descriptorSets = createDescriptorSet(device, swapChainObj.swapChainImagesCount, uniformBuffers, descriptorSetLayout, descriptorPool, textureImageView, textureSampler);
    
    
//    12.3 : Creating the command buffer
//    VkCommandBuffer* commandBuffers = createCommandBuffers(device, renderPass, graphicsPipeline, swapChainFramebuffers, commandPool, swapChainImagesCount, presentsAnFormatsInfo.extent);
    swapChainObj.commandBuffers = createCommandBuffers(device, swapChainObj.renderPass, swapChainObj.graphicsPipeline, swapChainObj.pipelineLayout, swapChainObj.swapChainFramebuffers, swapChainObj.commandPool, swapChainObj.swapChainImagesCount, presentsAnFormatsInfo.extent, vertexBuffer, indicesBuffer, (uint32_t) sizeof(indices) / sizeof(indices[0]), descriptorSets);
        
    
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
        
        vkDestroyImageView(device, depthImageView, NULL);
        vkDestroyImage(device, depthImage, NULL);
        vkFreeMemory(device, depthImageMemory, NULL);
        
        vkDestroySampler(device, textureSampler, NULL);
        vkDestroyImageView(device, textureImageView, NULL);
        
        vkDestroyImage(device, textureImage, NULL);
        vkFreeMemory(device, textureImageMemory, NULL);
        
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
