
#include "include/geninc.h"
#include "initVK.h"
#include "deviceVK.h"
#include "graphicFamilyVK.h"
#include "swapChain.h"
#include "shaders.h"
#include "pipeline.h"
#include "commandBuffer.h"
#include "drawcall.h"




/**
 Global Variables
 */
static const char* validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};





/**
 This is the main function
 */

int main(void) {
    
// Opening glfw window setup...
    glfwInit();
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
    char vertPath[] =
                    "#version 450\n"
                    "#extension GL_ARB_separate_shader_objects : enable\n"
                    "layout(location = 0) out vec3 fragColor;\n"
                    "vec2 positions[3] = vec2[](vec2(0.0, -0.5),vec2(0.5, 0.5),vec2(-0.5, 0.5));\n"
                    "vec3 colors[3] = vec3[](vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));\n"
                    "void main(){gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);fragColor = colors[gl_VertexIndex];}";


    VkShaderModule vertShaderModule = createShaderProgram(device, vertPath, "main.vert", "main", 0);

    //    Shaders (Vertex Shader first)::
        char fragPath[] =
                        "#version 450\n"
                        "#extension GL_ARB_separate_shader_objects : enable\n"
                        "layout(location = 0) in vec3 fragColor;\n"
                        "layout(location = 0) out vec4 outColor;\n"
                        "void main() {outColor = vec4(fragColor, 1.0);}";

    VkShaderModule fragShaderModule = createShaderProgram(device, fragPath, "main.frag", "main", 1);


//    Lets setup the structs for situating the shaders in the graphics pipline
//    Reduce this down to another function... We are doubling up on size of the program
//    by having the shaderModules in three different places... i.e. the original, the struct shaderStageInfo
//    and the array shaderStages....

    
    VkPipelineShaderStageCreateInfo vertShaderStageInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main"
    };


    VkPipelineShaderStageCreateInfo fragShaderStageInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main"
    };

//    Final shader stage info for graphics pipeline
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    
    

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
            free(commandBuffers);
            
            
        //    Lets clear out all the Framebuffers
        //    We have to run it through a for loop to delete all the nested buffer we created earlier
        //    similar to the swap chain images just below.
            for (int i = 0; i < swapChainImagesCount; i++)
            {
                vkDestroyFramebuffer(device, swapChainFramebuffers[i], NULL);
            }
            free(swapChainFramebuffers);
            
        //    Graphics render pass pipeline creation
            vkDestroyPipeline(device, graphicsPipeline, NULL);
            
        //    Rendering pass
            vkDestroyRenderPass(device, renderPass, NULL);
            
        //    Initial pipeline
            vkDestroyPipelineLayout(device, pipelineLayout, NULL);
            
            vkDestroyShaderModule(device, vertShaderModule, NULL);
            vkDestroyShaderModule(device, fragShaderModule, NULL);
            
            
            
            for (int i = 0; i < swapChainImagesCount; i++)
            {
                vkDestroyImageView(device, swapChainImageViews[i], NULL);
            }
            free(swapChainImageViews);

            vkDestroySwapchainKHR(device, swapChainKHR, NULL);
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
