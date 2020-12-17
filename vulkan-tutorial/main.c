
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linmath.h"
#include <shaderc/shaderc.h>


//Check the suitability of a device for use fo Vulkan API
// This can be a very indepth function, however atm we just look to see if a
//suitable device was found.
bool isDeviceSuitable(VkPhysicalDevice device) {
    return true;
}

VkShaderModule createShaderModule(const char* code, VkDevice device)
{
    VkShaderModuleCreateInfo createInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = sizeof(code),
        .pCode = (const uint32_t*) code
    };


    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS)
    {
        perror("failed to create shader module!\n");
    }
    return shaderModule;
}


//Main function
//TO DO:
// Abstraction once triangle is done.
int main(void) {
// Opening glfw window setup...
    
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    const uint32_t width = 800;
    const uint32_t height = 600;
    

//    Establish validation layer for vulkan
//    This is a long and arguous process..
    const char* validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const char* instance_validation_layers = NULL;
    const bool enableValidationLayers = true;
    VkBool32 foundvalidationLayer = 0;
    uint32_t layerCount;
    
//    Beginning to access and obtain count of validation layers available on system
//    If you are using the LunarG SDK use vulkaninfo.app to verify your systems
//    Vulkan implemenation.
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    instance_validation_layers = (const char*)validationLayers;
    
//    If the number of layers is more than 0 we'll create a temp VkLayerProperties struct
//    and will give it the memory size of the size of the structs constructor multiplied
//    by the number of layers found.
//    After that we re-run vkEnumerateInstanceLayerProperties, but this time provide it
//    with the VkLayerProperties struct we've just created.
    if (layerCount > 0) {
        VkLayerProperties *instance_layers = malloc(sizeof(VkLayerProperties) * layerCount);
        VkResult instanceResult = vkEnumerateInstanceLayerProperties(&layerCount, instance_layers);
        printf("Instance layer enum result: %d\n", instanceResult);
        
//        After retrieving the layers we make sure to have the length of our validationLayers
//        array for iteration.
        int validationLayersLength = sizeof(&validationLayers) / sizeof(&validationLayers[0]);
        printf("Validation Layer Length: %d\n", validationLayersLength);
        
//        At this point we will iterator over our instance_layers struct and compare it
//        with strcmp (from the string.h) to see if our validation layer request is
//        available on out system (this was done by vkEnumerateInstanceLayerProperties)
        for (int i = 0; i < validationLayersLength; i++)
        {
            foundvalidationLayer = 0;
            for (int j = 0; j < layerCount; j++) {
//                printf("System Validation layer: %s \n", instance_layers[j].layerName);
                if (!strcmp(&instance_validation_layers[i], instance_layers[j].layerName))
                {
                    foundvalidationLayer = 1;
                    break;
                }
            }
            if (!foundvalidationLayer) {
                printf("Cannot find validation layer\n");
            }
            else
            {
                printf("Found validation layer: %s\n", &instance_validation_layers[i]);
            }
        }
//        Always free memory alloaction!!!
        free(instance_layers);
    }
//    finally we do a basic check to see if a validation layer was found.
    if (enableValidationLayers && !foundvalidationLayer) {
        printf("validation layers requested, but not available!\n");
    }

    
//    proceed with creating a GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan window", NULL, NULL);
    
    
    
    
//  Vulkan instance setup
    VkInstance instance;
    
    const VkApplicationInfo appInfo =
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Hello Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_2
    };
    
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    
    VkInstanceCreateInfo createInfo =
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions
    };
        
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = (uint32_t)(sizeof(&validationLayers) / sizeof(&validationLayers[0]));
        createInfo.ppEnabledLayerNames = &validationLayers;
    }
    
    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
    
    if (result != VK_SUCCESS)
        printf("failed to create Vulkan instance! Return code: %d\n", (int)result);
    else
        printf("VkInstance successfully created!\n Return code: %d \n", (int)result);

    
//  Check for number of available extension
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    printf("Number of extensions supported: %d \n", extensionCount);
    
//  lets print out the names of the extensions available
    VkExtensionProperties* instance_extensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, instance_extensions);
    for (int i = 0; i < extensionCount; i++) {
        printf("Extension name: %s \n", instance_extensions[i].extensionName);
    }
    
//    free the allocated memory for instance_extensions
    free(instance_extensions);

    
//    STEP 2 ??? : Next major step
//    Setting up the physical devices
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (deviceCount > 0)
    {
        printf("GPU device found!\n");
        VkPhysicalDevice* physical_device = malloc(sizeof(VkPhysicalDevice) * deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physical_device);
        
        for (int i = 0; i < deviceCount; i++) {
            if (isDeviceSuitable(*physical_device))
            {
                printf("GPU device is suitable for Vulkan API.\n");
                physicalDevice = physical_device[i];
                break;
            }
        }
        if (physicalDevice == VK_NULL_HANDLE)
        {
            printf("GPU device found is not suitable for Vulkan API\n");
        }
        free(physical_device);
    }
    else
    {
        printf("failed to find GPUs with Vulkan support!\n\n");
        return -1;
    }
//    The main device has been detected and varified as suitable for Vulkan API
//    This can be an indepth process of reviewing the device and understanding the GPUs
//    limitations. For not we just grab the device that is suitable for Vulkan API.
    
//    STEP 2.1 :: Queue Family... Whatever that means...!!
    // Logic to find queue family indices to populate struct with
//    Just below are the queue variable
    uint32_t graphicsFamilyIndices = 0;
//    Dont think I need this...
    uint32_t queueCount = 0;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);


    VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
    if (queueFamilyCount >= 1)
    {
        printf("%d Family queues found\n", queueFamilyCount);
        for (int i = 0; i < queueFamilyCount; i++) {
            printf("Family queue num: %d has %u queues within it.\n", i+1, queueFamilies[i].queueCount);
        }
        for (int i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                    graphicsFamilyIndices = i;
                    queueCount = queueFamilies[i].queueCount;
                    printf("Graphics Family indices: %d \n", graphicsFamilyIndices);
                    printf("Queue Flags: %u \n", queueFamilies[i].queueFlags);
                    break;
            }
        }
    }
    free(queueFamilies);

//    Currently here in code review....
//    Looking at this as STEP 3
//    STEP 3:: Setting up a logical devices to interface with.
    VkDevice device;
    
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsFamilyIndices,
        .queueCount = queueCount,
        .pQueuePriorities = &queuePriority
    };
    
//    Instantiate struct of device feature available. With initialised
//    everything is set to true.
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
//    Set all physical device features to false.
//    memset(&deviceFeatures, 0, sizeof(deviceFeatures));
    
    VkDeviceCreateInfo deviceCreateinfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &queueCreateInfo,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &deviceFeatures,
//        .enabledExtensionCount = 0,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = &validationLayers
    };
    
    
//    moved components
    const char* deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    uint32_t deviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, NULL);
    VkExtensionProperties* availableExtension = malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
    
    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, availableExtension);
  
//    Have to re-wite this loop, to be slightly better...
    for (int i = 0; i < deviceExtensionCount; i++)
    {
        if (strcmp((const char*) &deviceExtensions, (const char*) &availableExtension[i].extensionName))
        {
//            we can be even more efficient with this by creating a bool value rtn for found divice extensions *
            printf("%s extension support found\n", deviceExtensions);
            break;
        }
//        printf("%s\n", availableExtension[i].extensionName);
    }
    

//    This has to happen before logical device creation..... Meaning that device extensions...
//    i.e. enabling device extensions is essential during STEP 3...
//    Literally... I hate OOP and this Vulkan tutorial.
    deviceCreateinfo.enabledExtensionCount = 1;
    deviceCreateinfo.ppEnabledExtensionNames = &deviceExtensions;
    free(availableExtension);
//    END of moved components
    
    
    if (vkCreateDevice(physicalDevice, &deviceCreateinfo, NULL, &device) != VK_SUCCESS) {
        printf("failed to create logical device!\n");
    }

    
//    STEP 4??:: Next major step
//    Window surface
    
    VkSurfaceKHR surface;
    
    VkResult surfaceResult = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (surfaceResult != VK_SUCCESS)
    {
        printf("Surface results code: %d \n", (int)surfaceResult);
        printf("Please look up result in vulkan_core.h\n");
    }
    else
        printf("GLFW VK window surface successfully created!\n");
    
    
    uint32_t presentFamily;
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndices, surface, &presentSupport);
    if (presentSupport)
    {
//        Indicate what is the present family being used which also has support.
        presentFamily = graphicsFamilyIndices;
        printf("Present graphics family being used: %u\n", presentFamily);
    }
    else
        presentFamily = 0;
    
//    Completing presentation queue
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, presentFamily, graphicsFamilyIndices, &graphicsQueue);
    
//    STEP 5:: Sway change and looking to see if we have support
//    First thing is to see if we have swap chain support.
//    const char* deviceExtensions =
//    {
//        VK_KHR_SWAPCHAIN_EXTENSION_NAME
//    };
//
//    uint32_t deviceExtensionCount;
//    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, NULL);
//    VkExtensionProperties* availableExtension = malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
//
//    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, availableExtension);
//
////    Have to re-wite this loop, to be slightly better...
//    for (int i = 0; i < deviceExtensionCount; i++)
//    {
//        if (strcmp((const char*) &deviceExtensions, (const char*) &availableExtension[i].extensionName))
//        {
////            we can be even more efficient with this by creating a bool value rtn for found divice extensions *
//            printf("%s extension support found\n", deviceExtensions);
//            break;
//        }
////        printf("%s\n", availableExtension[i].extensionName);
//    }
//
//
////    This has to happen before logical device creation..... Meaning that device extensions...
////    i.e. enabling device extensions is essential during STEP 3...
////    Literally... I hate OOP and this Vulkan tutorial.
//    deviceCreateinfo.enabledExtensionCount = ((uint32_t) sizeof(deviceExtensions));
//    deviceCreateinfo.ppEnabledExtensionNames = &deviceExtensions;
//    free(availableExtension);
    
//    Both moved from different locations....
//    if (vkCreateDevice(physicalDevice, &deviceCreateinfo, NULL, &device) != VK_SUCCESS) {
//        printf("failed to create logical device!\n");
//    }
//    vkGetDeviceQueue(device, presentFamily, graphicsFamilyIndices, &graphicsQueue);
//    END OF MOVED COMPONENTS
    
//    STEP 5.2: We are now checking for compatibilitoes
    
    VkSurfaceCapabilitiesKHR capabilities;
    VkResult surfaceCapabilitesResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    if (surfaceCapabilitesResult != VK_SUCCESS) {
        printf("Surface capabilities initialisation failed.\n");
        printf("return code: %u\n", surfaceCapabilitesResult);
    }
    
//    Initial these two VK structs...
//    more information on stucts and dat inside to come...
//    The reason we initialise these here is because we will use them just below, however the resizing is
//    happening after we check the result back from surface formats KHR
//    This is to prevent them being un-initialised.
    VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * 1);
    VkPresentModeKHR* presentModes = malloc(sizeof(VkPresentModeKHR) * 1);
    
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
    VkSurfaceFormatKHR availableFormats;
    if (formatCount != 0) {
        formats = realloc(formats, sizeof(VkSurfaceFormatKHR) * formatCount);
        VkResult surfaceFormatResult = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);
        if (surfaceFormatResult != VK_SUCCESS) {
            printf("Something went wrong...\n");
            printf("Return code: %u\n", surfaceFormatResult);
        }
        else
        {
            for (int i = 0; i < formatCount; i++)
            {
                if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    availableFormats = formats[i];
                    printf("Format VK_FORMAT_B8G8R8A8_SRGB and colorspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR found!\n");
                    break;
                }
                
            }
        }
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
    VkPresentModeKHR avaiablePresentMode;
    
    if (presentModeCount != 0) {
        presentModes = realloc(presentModes, sizeof(VkPresentModeKHR) * presentModeCount);
        VkResult surfacePresentModeResult = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);
        if (surfacePresentModeResult != VK_SUCCESS) {
            printf("Something went wrong...\n");
            printf("Return code: %u\n", surfacePresentModeResult);
            free(presentModes);
            return -1;
        }
        else
        {
            for (int i = 0; i < presentModeCount; i++)
            {
                if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                    avaiablePresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                    printf("VK_PRESENT_MODE_MAILBOX_KHR present mode selected\n");
                    break;
                }
//                else if(i == presentModeCount && presentModes[i] != VK_PRESENT_MODE_MAILBOX_KHR)
//                {
//                    avaiablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
//                    printf("VK_PRESENT_MODE_FIFO_KHR present mode selected\n");
//                }
            }
            avaiablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
            printf("VK_PRESENT_MODE_FIFO_KHR present mode selected\n");
//            avaiablePresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
//            printf("%u\n", avaiablePresentMod);
        }
    }


//    free memory allocation for formats and presentMode varibles
    free(formats);
    free(presentModes);
    
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
//        printf("capabilities.currentExtent.width : %u\n", capabilities.currentExtent.width);
//        printf("capabilities.minImageExtent.width : %u\n", capabilities.minImageExtent.width);
        printf("\ncapabilities.currentExtent.width != UINT32_MAX...\n");
        printf("Still to check if this is bad...\n\n");
    }
    
//    STEP 5.2:: Setting up the action swap chain functionality
    VkExtent2D extent = capabilities.currentExtent;
    
    uint32_t imageCount = capabilities.minImageCount + 1;
    
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createSwapChainInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = availableFormats.format,
        .imageColorSpace = availableFormats.colorSpace,
        .presentMode = avaiablePresentMode,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };
    
//    if (avaiablePresentMode) {
//        createSwapChainInfo.presentMode = avaiablePresentMode;
//    }
//    else
//    {
//        createSwapChainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
//    }
    
    uint32_t queueFamilyIndices[] =
    {
        graphicsFamilyIndices,
        presentFamily
    };
    
    if (graphicsFamilyIndices != presentFamily)
    {
        printf("Graphics family does not equal present family\n\n");
        createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createSwapChainInfo.queueFamilyIndexCount = 2;
        createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        printf("Graphics family does equal present family\n\n");
        createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createSwapChainInfo.queueFamilyIndexCount = 0; // Optional
        createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndices; // Optional
    }
    
    VkSwapchainKHR swapChainKHR;
    VkResult swapChainCreateResult = vkCreateSwapchainKHR(device, &createSwapChainInfo, NULL, &swapChainKHR);
    if (swapChainCreateResult != VK_SUCCESS)
    {
        printf("failed to create swap chain!\n");
    }
    
    VkImage* swapChainImages = malloc(sizeof(VkImage) * 1);
    vkGetSwapchainImagesKHR(device, swapChainKHR, &imageCount, NULL);
//    imageCount = realloc(imageCount, sizeof(imageCount) * imageCount);
    vkGetSwapchainImagesKHR(device, swapChainKHR, &imageCount, swapChainImages);
    
    int swapChainImagesCount = (int) sizeof(swapChainImages) / (int) swapChainImages[0];
    
    VkImageView* swapChainImageViews = malloc(sizeof(VkImageView) * swapChainImagesCount);
    
//    int swapChainImageViewCount = (int) sizeof(swapChainImageViews) / (int) swapChainImageViews[0];
    
   
    
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        VkImageViewCreateInfo createImageViewInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = availableFormats.format,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };
        
        
        if (vkCreateImageView(device, &createImageViewInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
            printf("failed to create image views!\n");
        }

    }
//    int swapChainImageViewCount = (int) sizeof(swapChainImageViews) / (int) swapChainImageViews[0];
    
    
//    After a long arguous process, we are finally at the graphics pipe line!
//    STEP 6:: Graphics pipeline
    
//    Shaders (Vertex Shader first)::
    char vertPath[] =
                    "#version 450\n"
                    "#extension GL_ARB_separate_shader_objects : enable\n"
                    "layout(location = 0) out vec3 fragColor;\n"
                    "vec2 positions[3] = vec2[](vec2(0.0, -0.5),vec2(0.5, 0.5),vec2(-0.5, 0.5));\n"
                    "vec3 colors[3] = vec3[](vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));\n"
                    "void main(){gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);fragColor = colors[gl_VertexIndex];}";
    
//    char vertPath[] = "#version 450\nvoid main(){}";
    
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    
    shaderc_compilation_result_t compilerVertResult = shaderc_compile_into_spv(compiler, vertPath, sizeof(vertPath) - 1,shaderc_glsl_vertex_shader, "main.vert", "main", NULL);
    
         // Do stuff with compilation results.
    size_t numOfErrors = shaderc_result_get_num_errors(compilerVertResult);
    if (numOfErrors != 0)
    {
        printf("%lu\n", numOfErrors);
        shaderc_compilation_status compileStatusResults = shaderc_result_get_compilation_status(compilerVertResult);
        printf("%u\n", compileStatusResults);
        const char* compilerErrorMessages = shaderc_result_get_error_message(compilerVertResult);
        printf("Compiler error message: %s\n", compilerErrorMessages);
        return -1;
    }
    
    
    size_t compiledVertByteSize = shaderc_result_get_length(compilerVertResult);
    const char* compiledVertBytes = shaderc_result_get_bytes(compilerVertResult);
    
//    Lets create the Vertinfo struct - this
    VkShaderModuleCreateInfo createVertInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = compiledVertByteSize,
        .pCode = (const uint32_t*) compiledVertBytes
    };
//
//
    VkShaderModule vertShaderModule;
    if (vkCreateShaderModule(device, &createVertInfo, NULL, &vertShaderModule) != VK_SUCCESS)
    {
        perror("failed to create shader module!\n");
        return -1;
    }
    
    //    Once we've finished with the Vertex shader compilation and reead. lets release
//   the result the comiler result to use for the frag shader.

    
    
    //    Shaders (Vertex Shader first)::
    char fragPath[] =
                    "#version 450\n"
                    "#extension GL_ARB_separate_shader_objects : enable\n"
                    "layout(location = 0) in vec3 fragColor;\n"
                    "layout(location = 0) out vec4 outColor;\n"
                    "void main() {outColor = vec4(fragColor, 1.0);}";
    
    
    shaderc_compilation_result_t compilerFragResult = shaderc_compile_into_spv(compiler, fragPath, sizeof(fragPath) - 1,shaderc_glsl_fragment_shader, "main.frag", "main", NULL);
    
    numOfErrors = shaderc_result_get_num_errors(compilerFragResult);
    if (numOfErrors != 0)
    {
        printf("%lu\n", numOfErrors);
        shaderc_compilation_status compileStatusResults = shaderc_result_get_compilation_status(compilerFragResult);
        printf("%u\n", compileStatusResults);
        const char* compilerErrorMessages = shaderc_result_get_error_message(compilerFragResult);
        printf("Compiler error message: %s\n", compilerErrorMessages);
        return -1;
    }
    
    size_t compiledFragByteSize = shaderc_result_get_length(compilerFragResult);
    const char* compiledFragBytes = shaderc_result_get_bytes(compilerFragResult);
    
//    Lets create the Vertinfo struct - this
    VkShaderModuleCreateInfo createFragInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = compiledFragByteSize,
        .pCode = (const uint32_t*) compiledFragBytes
    };
//
//
    VkShaderModule fragShaderModule;
    if (vkCreateShaderModule(device, &createFragInfo, NULL, &fragShaderModule) != VK_SUCCESS)
    {
        perror("failed to create shader module!\n");
        return -1;
    }
    
    
//    Lets setup the structs for situating the shaders in the graphics pipline
    
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
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    
    //    Now that we have our frag shader we free the compiled results, and free the compiler itself
    shaderc_result_release(compilerVertResult);
    shaderc_result_release(compilerFragResult);
    shaderc_compiler_release(compiler);

    
    
//    Fixing functions
//    This is the next major step
    
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
        .alphaBlendOp = VK_BLEND_OP_ADD, // Optional
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
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

//    This is the rendering pass phase
//    Next major stage
    
    VkFormat VkColourFormat = availableFormats.format;
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
    
    
    VkRenderPass renderPass;
//    VkPipelineLayout renderPipelineLayout;
    
    VkRenderPassCreateInfo renderPassInfo =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass

    };
    
    if (vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        printf("failed to create render pass!\n");
    }
    
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
    }


    
    
    
    
    
////    Math library tests
//    mat4x4 matrix;
//    vec4 r;
//    vec4 v;
//    mat4x4_identity(matrix);
//
//    mat4x4_mul_vec4(r, matrix, v);
//    uint32_t veclen = vec4_len(r);
//    printf("Vec4 variable r length: %u \n", veclen);


//    Main loop
    while(!glfwWindowShouldClose(window))
    {
            glfwPollEvents();
    }

    
//    Program clean up
    vkDestroyPipeline(device, graphicsPipeline, NULL);
//    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    
//    Second pipeline
//    vkDestroyPipelineLayout(device, renderPipelineLayout, NULL);
    vkDestroyRenderPass(device, renderPass, NULL);
    
//    Initial pipeline
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    
    vkDestroyShaderModule(device, vertShaderModule, NULL);
    vkDestroyShaderModule(device, fragShaderModule, NULL);

    
//    //    Now that we have our frag shader we free the compiled results, and free the compiler itself
//    shaderc_result_release(compilerVertResult);
//    shaderc_result_release(compilerFragResult);
//    shaderc_compiler_release(compiler);
    
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        vkDestroyImageView(device, swapChainImageViews[i], NULL);
    }
    free(swapChainImageViews);
    free(swapChainImages);
    vkDestroySwapchainKHR(device, swapChainKHR, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    
    return 0;
}
