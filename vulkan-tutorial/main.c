
#include "include/geninc.h"
#include "initVK.h"
#include "deviceVK.h"
#include "graphicFamilyVK.h"
#include "swapChain.h"
#include "shaders.h"
#include "pipeline.h"
#include "commandBuffer.h"

/**
 Maths Library: https://github.com/datenwolf/linmath.h
 */
#include "linmath.h"


//  Path to shader files, NOTE: create new new compiler macros for each individual shader file.
#define VERTEX_SHADER_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/shaders/shader.vert"
#define FRAGMENT_SHADER_FILE_PATH "/Users/olivandro/Apps/vulkan-tutorial/vulkan-tutorial/shaders/shader.frag"


/**
 Global Variables
 */
static const char* validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};


void cleanUpSwapChain(VkDevice device, int swapChainImagesCount, VkSwapchainKHR swapChainKHR, VkImageView* swapChainImageViews, VkPipelineShaderStageCreateInfo* shaderStages, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkFramebuffer* swapChainFramebuffers, VkCommandBuffer* commandBuffers, VkCommandPool commandPool)
{
// Swapchain clean up
    //    Lets clear out all the Framebuffers
    //    We have to run it through a for loop to delete all the nested buffer we created earlier
    //    similar to the swap chain images just below.
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], NULL);
    }
    free(swapChainFramebuffers);

    vkFreeCommandBuffers(device, commandPool, (uint32_t)(swapChainImagesCount + 1), commandBuffers);
    free(commandBuffers);
    
//    Graphics render pass pipeline creation
    vkDestroyPipeline(device, graphicsPipeline, NULL);
    
//    Rendering pass
    vkDestroyRenderPass(device, renderPass, NULL);
    
//    Initial pipeline
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    free(shaderStages);

    for (int i = 0; i < swapChainImagesCount; i++)
    {
        vkDestroyImageView(device, swapChainImageViews[i], NULL);
    }
    free(swapChainImageViews);
    vkDestroySwapchainKHR(device, swapChainKHR, NULL);
// END OF SWAPCHAIN CLEANUP
}


void recreateSwapChain(VkDevice device, VkSurfaceKHR surface, struct availablePresentsAnFormats presentsAnFormatsInfo, struct graphicsFamiliesAnIndices queueFamilyIndicesInfo, VkShaderModule vertShaderModule, VkShaderModule fragShaderModule, VkSwapchainKHR swapChainKHR, int swapChainImagesCount, VkImageView* swapChainImageViews, VkPipelineShaderStageCreateInfo* shaderStages, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkFramebuffer* swapChainFramebuffers, VkCommandBuffer* commandBuffers, VkCommandPool commandPool)
{
    /**
            Wait idle device
     */
    vkDeviceWaitIdle(device);
    /**
            cleanup swapchain
     */
    cleanUpSwapChain(device, swapChainImagesCount, swapChainKHR, swapChainImageViews, shaderStages, pipelineLayout, renderPass, graphicsPipeline, swapChainFramebuffers, commandBuffers, commandPool);
    
    /**
     Creation of the swapchain
     */
        swapChainKHR = createSwapChain(device, surface, presentsAnFormatsInfo, queueFamilyIndicesInfo);
        swapChainImageViews = createImageView(device, swapChainKHR, presentsAnFormatsInfo);
        swapChainImagesCount = findSwapChainImageCount(device, swapChainKHR, presentsAnFormatsInfo);
        shaderStages = CreateShaderStages(vertShaderModule, fragShaderModule);
        pipelineLayout = createPipelineLayout(device);
        renderPass = createRenderingPass(device, presentsAnFormatsInfo.availableFormats.format);
        graphicsPipeline = createGraphicsPipeline(device, pipelineLayout, renderPass, presentsAnFormatsInfo.extent, shaderStages);
        swapChainFramebuffers = createSwapChainFramebuffers(device, swapChainImageViews, swapChainImagesCount, renderPass, presentsAnFormatsInfo.extent);
        commandBuffers = createCommandBuffers(device, renderPass, graphicsPipeline, swapChainFramebuffers, commandPool, swapChainImagesCount, presentsAnFormatsInfo.extent);
}


void drawCall(VkDevice device,  VkQueue graphicsQueue, VkSwapchainKHR swapChainKHR, VkCommandBuffer* commandBuffers, struct syncAndFence syc, const int MAX_FRAMES_IN_FLIGHT)
{
    
            static size_t currentFrame = 0;
    //        We tell our program to wait for the fence that we set up in section 9.
            vkWaitForFences(device, 1, &syc.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            
            
            static uint32_t imageIndex = 0;
            VkResult result = vkAcquireNextImageKHR(device, swapChainKHR, UINT64_MAX, syc.imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
        
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                printf("Out of date swap chain image!\n");
//                recreateSwapChain();
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
    
            
            submitInfo.pCommandBuffers = &commandBuffers[currentFrame]; // I had to change this from imageIndex - imageIndex goes to 2 (or 3), while the
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
            

            VkSwapchainKHR swapChains[] = {swapChainKHR};
            
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = NULL; // Optional
            
            vkQueuePresentKHR(graphicsQueue, &presentInfo);
            
            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}




// Vertex buffer Works
struct Vectex {
    vec2 position;
    vec3 color;
    VkVertexInputBindingDescription bindingDescription;
    VkVertexInputAttributeDescription attributeDescriptions;
} Vertex;


/**
 This is the main function
 */

int main(void) {
    
// Opening glfw window setup...
    if (glfwInit() != GLFW_TRUE)
    {
        printf("Error initialising GLFW");
        assert();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    const uint32_t width = 800;
    const uint32_t height = 600;
    
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
    VkSwapchainKHR swapChainKHR = createSwapChain(device, surface, presentsAnFormatsInfo, queueFamilyIndicesInfo);
    
    
/**
 This block pertains to    createImageViews(); from tutorial
 */
//  10. setup swapchain images
    VkImageView* swapChainImageViews = createImageView(device, swapChainKHR, presentsAnFormatsInfo);
    
//    Had to do this... its an incredibly important variable that I constantly use.
    int swapChainImagesCount = findSwapChainImageCount(device, swapChainKHR, presentsAnFormatsInfo);
    
    
/**
 Creating the graphics pipeline - includes 5 steps
 */
//    11 :: Graphics pipeline :: SHADERS
//    After a long arguous process, we are finally at the graphics pipe line!

//    this whole section can be abstracted into functions and return a single variable
//    Functions needed: (1) parseShaders; (2) compileShaders; (3) createShaderProgram

//    11.1 : Shaders (Vertex Shader first)::
    VkShaderModule vertShaderModule = createShaderProgram(device, VERTEX_SHADER_FILE_PATH, "main.vert", "main", 0);

    //    Shaders (Vertex Shader first)::
    VkShaderModule fragShaderModule = createShaderProgram(device, FRAGMENT_SHADER_FILE_PATH, "main.frag", "main", 1);

    
//    Final shader stage info for graphics pipeline
//    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    VkPipelineShaderStageCreateInfo* shaderStages = CreateShaderStages(vertShaderModule, fragShaderModule);
    
//    11.2 These are the fixing functions... i.e. we create the data structs for initialising the
//    Graphics pipeline and layout.
//    This is the next major step
//    In Vulkan you have to be explicit about everything, from viewport size to color blending function.
   
//    11.3 : Creating the graphics pipeline
    VkPipelineLayout pipelineLayout = createPipelineLayout(device);
    
    
//    11.4 : Creating the rendering pass
    VkRenderPass renderPass = createRenderingPass(device, presentsAnFormatsInfo.availableFormats.format);
    

//    11.5 Create the Graphics pipeline
    VkPipeline graphicsPipeline = createGraphicsPipeline(device, pipelineLayout, renderPass, presentsAnFormatsInfo.extent, shaderStages);
    
    
/**
 Create the Command buffer, frame buffer and render pass command pool
 NOTE: This is were the errors in running the program are
 */
//    12 : setting up the command buffer and frame buffers plue the render
//    pass command pool
    VkFramebuffer* swapChainFramebuffers = createSwapChainFramebuffers(device, swapChainImageViews, swapChainImagesCount, renderPass, presentsAnFormatsInfo.extent);
    
    
//    12.2 : Lets create the commandpool now..
    VkCommandPool commandPool = createCommandPool(device, queueFamilyIndicesInfo.presentFamily);
    

//    12.3 : Creating the command buffer
    VkCommandBuffer* commandBuffers = createCommandBuffers(device, renderPass, graphicsPipeline, swapChainFramebuffers, commandPool, swapChainImagesCount, presentsAnFormatsInfo.extent);
    
    
//  13 : rasterisation and Presentation of the triangle that we are drawing
    
//    Both variables are requested throughout the rest of the program
    const int MAX_FRAMES_IN_FLIGHT = 2;
    
    struct syncAndFence syc = createSyncAndFence(device, MAX_FRAMES_IN_FLIGHT, swapChainImagesCount);
    
    


//    Main loop
//        This is the game loops
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        /**
        Draw call
         */
        drawCall(device, graphicsQueue, swapChainKHR, commandBuffers, syc, MAX_FRAMES_IN_FLIGHT);
        
    }

    
    if (vkDeviceWaitIdle(device) == VK_SUCCESS)
    {

            

// Swapchain clean up
        cleanUpSwapChain(device, swapChainImagesCount, swapChainKHR, swapChainImageViews, shaderStages, pipelineLayout, renderPass, graphicsPipeline, swapChainFramebuffers, commandBuffers, commandPool);

            
//            Program clean up
//            Will need to figure out how to abstract this effectively
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

        //    Cleanup for commandpool
            vkDestroyCommandPool(device, commandPool, NULL);
            
        
        
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
