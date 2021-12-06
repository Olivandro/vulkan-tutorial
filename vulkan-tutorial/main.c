
#include "geninc.h"
#include "initVK.h"
#include "deviceVK.h"
#include "graphicFamilyVK.h"
#include "swapChain.h"
#include "shaders.h"
#include "pipeline.h"
#include "graphicsDataBuffer.h"
#include "commandBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"


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


void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    VkBufferCreateInfo bufferInfo;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.flags = 0;
    bufferInfo.pNext = NULL;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        printf("failed to create buffer!\n");
    }
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties, memProperties);

    if (vkAllocateMemory(device, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
        printf("failed to allocate buffer memory!\n");
    }

    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
    
    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = NULL;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
//    barrier.srcAccessMask = 0; // TODO
//    barrier.dstAccessMask = 0; // TODO
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        printf("unsupported layout transition!\n");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );

    endSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);
}

void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
    
    VkBufferImageCopy region =
    {
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1}
    };
    
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);
}



void createTextureImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage* textureImage, VkDeviceMemory* textureImageMemory)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(TEST_TEXTURE_FILE_PATH, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        printf("failed to load texture image!\n");
    }
    
    createBuffer(device, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, (size_t) imageSize);
    vkUnmapMemory(device, stagingBufferMemory);
    
    stbi_image_free(pixels);

    
//    Create image & memory values
    VkImageCreateInfo imageInfo;
    imageInfo.pNext = NULL;
    imageInfo.flags = VK_NULL_HANDLE;
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texWidth;
    imageInfo.extent.height = texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, NULL, textureImage) != VK_SUCCESS) {
        printf("failed to create image!\n");
    }

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.pNext = NULL;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memProperties);

    if (vkAllocateMemory(device, &allocInfo, NULL, textureImageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!\n");
    }

    vkBindImageMemory(device, *textureImage, *textureImageMemory, 0);
//    End of image & memory values
    
    transitionImageLayout(device, commandPool, graphicsQueue, *textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    
    copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer, *textureImage, (uint32_t) texWidth, (uint32_t) texHeight);
    
    transitionImageLayout(device, commandPool, graphicsQueue, *textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

}



void createTextureImageView(VkDevice device, VkImage textureImage, VkFormat format, VkImageView* textureImageView)
{
    VkImageViewCreateInfo viewInfo;
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = NULL;
    viewInfo.flags = VK_NULL_HANDLE;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewInfo, NULL, textureImageView) != VK_SUCCESS) {
        printf("failed to create texture image view!\n");
    }

}


void createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice, VkSampler* textureSampler)
{
    VkSamplerCreateInfo samplerInfo;
    samplerInfo.pNext = NULL;
    samplerInfo.flags = VK_NULL_HANDLE;
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    samplerInfo.anisotropyEnable = VK_TRUE;
    
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    
    if (vkCreateSampler(device, &samplerInfo, NULL, textureSampler) != VK_SUCCESS) {
            printf("failed to create texture sampler!\n");
        }
    
}



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
     struct DrawingData vertices[4] =
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
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
    Texture Mapping
    NOTE:: The assignment and subsequent initalisation of the text values below is the preferred way I believe
    to initialised all the VK variables.
 
    TO DO:
    1. Finish texture implementation.
    2. Re write buffer code to match this control flow.
 */
    
    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;

    
    createTextureImage(device, physicalDevice, swapChainObj.commandPool, graphicsQueue, &textureImage, &textureImageMemory);
    
    
    
    VkImageView textureImageView = VK_NULL_HANDLE;
    createTextureImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, &textureImageView);
    
    
    VkSampler textureSampler = VK_NULL_HANDLE;
    createTextureSampler(device, physicalDevice, &textureSampler);
    
/**
    END Texture Mapping
 */
    
    
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
    VkDescriptorSet* descriptorSets = createDescriptorSet(device, swapChainObj.swapChainImagesCount, uniformBuffers, descriptorSetLayout, descriptorPool, textureImageView, textureSampler);
    
    
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
