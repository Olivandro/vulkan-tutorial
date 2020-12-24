
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <shaderc/shaderc.h>

#define ENABLE_VALIDATION_LAYERS true
#define APP_NAME "Hello Triangle"
#define ENGINE_NAME "No Engine"
#define WINDOW_NAME "Vulkan window"

/**
 Global Variables
 */
static const char* validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};

/**
 Stuct constructors
 */
struct graphicsFamiliesAnIndices
{
    uint32_t graphicsFamilyIndices;
    uint32_t queueCount;
    uint32_t presentFamily;
    uint32_t queueFamilyIndices[2];
    
} graphicsFamiliesAnIndices;


struct availablePresentsAnFormats
{
    VkSurfaceFormatKHR availableFormats;
    VkPresentModeKHR avaiablePresentMode;
    
} availablePresentsAnFormats;

/**
    Program functions
 */
void initValidationLayer(const char* validationLayers, const char* instance_validation_layers)
{
        VkBool32 foundvalidationLayer = 0;
        uint32_t layerCount = 0;
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
//            can remove this and wrap the vkEnumerateInstanceLayerProperties in a if logincal statement.
            VkResult instanceResult = vkEnumerateInstanceLayerProperties(&layerCount, instance_layers);
            if (instanceResult != VK_SUCCESS)
            {
//                COULD DO:
//                Develop switch statement that runs through all the warnings and makes it easier to debug
                printf("Enumerated Instance layer properties failed\n");
                printf("Instance layer enum result: %d\n", instanceResult);
            }
            
    //        After retrieving the layers we make sure to have the length of our validationLayers
    //        array for iteration.
            int validationLayersLength = sizeof(&validationLayers) / sizeof(&validationLayers[0]);
            
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
        if (ENABLE_VALIDATION_LAYERS && !foundvalidationLayer) {
            printf("validation layers requested, but not available!\n");
        }
}


VkInstance createInstance(const char* validationLayers)
{
    uint32_t extensionCount = 0;
    VkInstance instance;
    VkApplicationInfo appInfo;
    VkInstanceCreateInfo createInfo;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    
//    2. Initial VK Instance
//    These values can be #defined varibles at the top of the page
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = APP_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

//    Get required glfw extensions
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    
//    Enable debug validation layer
//    TO DO: fix disable validation layer
    if (ENABLE_VALIDATION_LAYERS)
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
    
    return instance;
}

//Check the suitability of a device for use fo Vulkan API
// This can be a very indepth function, however atm we just look to see if a
//suitable device was found.
bool isDeviceSuitable(VkPhysicalDevice device) {
    return true;
}

VkPhysicalDevice createPhysicalDevice(VkInstance instance)
{
    //    Reoccurring variable
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        
    //    Variable is not used beyond line 200
        uint32_t deviceCount = 0;

        vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
        if (deviceCount > 0)
        {
            printf("GPU device found!\n");
            
    //        This can be and internalised variable in a function
            VkPhysicalDevice* physical_device = malloc(sizeof(VkPhysicalDevice) * deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, physical_device);
            
        //    The main device has been detected and varified as suitable for Vulkan API
        //    This can be an indepth process of reviewing the device and understanding the GPUs
        //    limitations. For not we just grab the device that is suitable for Vulkan API.
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
    return physicalDevice;
}


VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, const char* validationLayers, uint32_t graphicsFamilyIndices, uint32_t queueCount)
{
    VkDevice device;
    //    This variable is used once in struct VkDeviceQueueCreateInfo
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
            .enabledLayerCount = 1
            
        };
        
        if (ENABLE_VALIDATION_LAYERS)
        {
            deviceCreateinfo.ppEnabledLayerNames = &validationLayers;
        }
        
    //    Here we have to enable the swapchain extension
    //    This has to happen before logical device creation.....
        
    //    This is a global variable - changable depending on setup?
        const char* deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        
        uint32_t deviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, NULL);
    //    Temp dynamic malloc variable, typeof struct
        VkExtensionProperties* availableExtension = malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
        
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &deviceExtensionCount, availableExtension);
      
        for (int i = 0; i < deviceExtensionCount; i++)
        {
            if (strcmp((const char*) &deviceExtensions, (const char*) &availableExtension[i].extensionName))
            {
    //            we can be even more efficient with this by creating a bool value rtn for found divice extensions *
                printf("%s extension support found\n", deviceExtensions);
                break;
            }
        }
        
        deviceCreateinfo.enabledExtensionCount = 1;
        deviceCreateinfo.ppEnabledExtensionNames = &deviceExtensions;
        free(availableExtension);

    //    Creating the logical device.
        if (vkCreateDevice(physicalDevice, &deviceCreateinfo, NULL, &device) != VK_SUCCESS) {
            printf("failed to create logical device!\n");
        }
    return device;
}



VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window)
{
//    Golbal variable - also neceszary for cleanup
    VkSurfaceKHR surface;
    
//    Temp rtn result fpr window surface creation
    VkResult surfaceResult = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (surfaceResult != VK_SUCCESS)
    {
        printf("Surface results code: %d \n", (int)surfaceResult);
        printf("Please look up result in vulkan_core.h\n");
    }
    else
        printf("GLFW VK window surface successfully created!\n");
    
    return surface;
}



struct graphicsFamiliesAnIndices createGraFamInd(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    uint32_t graphicsFamilyIndices = 0;
    uint32_t queueCount = 0;
    uint32_t queueFamilyCount = 0;
    
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
    if (queueFamilyCount >= 1)
    {
        printf("%d Family queues found\n", queueFamilyCount);
//        for (int i = 0; i < queueFamilyCount; i++) {
//            printf("Family queue num: %d has %u queues within it.\n", i+1, queueFamilies[i].queueCount);
//        }
        for (int i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags && VK_QUEUE_GRAPHICS_BIT)
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

//    Global varible that is used throughout various structs
    uint32_t presentFamily;
    
//    Not used past this below code block
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

    
    struct graphicsFamiliesAnIndices queueFamilyIndicesInfo =
    {
        .graphicsFamilyIndices = graphicsFamilyIndices,
        .queueCount = queueCount,
        .presentFamily = presentFamily,
        .queueFamilyIndices = {
            graphicsFamilyIndices,
            presentFamily
        }
        
    };
    
    return queueFamilyIndicesInfo;

}

VkQueue createGraphicsQueue(VkDevice device, struct graphicsFamiliesAnIndices queueFamilyIndicesInfo)
{
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, queueFamilyIndicesInfo.presentFamily, queueFamilyIndicesInfo.graphicsFamilyIndices, &graphicsQueue);
    return graphicsQueue;
}


VkSurfaceCapabilitiesKHR checkPhysicalDeviceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    VkSurfaceCapabilitiesKHR capabilities;
    //    Temp trn result for process.
    VkResult surfaceCapabilitesResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    if (surfaceCapabilitesResult != VK_SUCCESS)
    {
        printf("Surface capabilities initialisation failed.\n");
        printf("return code: %u\n", surfaceCapabilitesResult);
        
    }
    return capabilities;
}


struct availablePresentsAnFormats findPresentsAnFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    //    Initial these two VK structs...
    //    more information on stucts and dat inside to come...
    //    The reason we initialise these here is because we will use them just below, however the resizing is
    //    happening after we check the result back from surface formats KHR
    //    This is to prevent them being un-initialised.
        
    //    Both these variables are temp malloc that are freed at the end of this code block
        VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * 1);
        VkPresentModeKHR* presentModes = malloc(sizeof(VkPresentModeKHR) * 1);
    //    Temp function that is only used in this code block.
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
    //    Global struct variable
        struct availablePresentsAnFormats presentsAnFormatsInfo;
//        VkSurfaceFormatKHR availableFormats;
        if (formatCount != 0) {
            formats = realloc(formats, sizeof(VkSurfaceFormatKHR) * formatCount);
    //        temp rtn result function.
            VkResult surfaceFormatResult = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);
            if (surfaceFormatResult != VK_SUCCESS) {
                printf("Something went wrong...\n");
                printf("Return code: %u\n", surfaceFormatResult);
                
//                free(formats);
//                free(presentModes);
//                return -1;
            }
            else
            {
                for (int i = 0; i < formatCount; i++)
                {
                    if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    {
                        presentsAnFormatsInfo.availableFormats = formats[i];
                        printf("Format VK_FORMAT_B8G8R8A8_SRGB and colorspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR found!\n");
                        break;
                    }
                    
                }
            }
        }
        
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
        
        if (presentModeCount != 0) {
            presentModes = realloc(presentModes, sizeof(VkPresentModeKHR) * presentModeCount);
            VkResult surfacePresentModeResult = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);
            if (surfacePresentModeResult != VK_SUCCESS) {
                printf("Something went wrong...\n");
                printf("Return code: %u\n", surfacePresentModeResult);
//                free(presentModes);
//                return -1;
            }
            else
            {
                for (int i = 0; i < presentModeCount; i++)
                {
                    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        presentsAnFormatsInfo.avaiablePresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        printf("VK_PRESENT_MODE_MAILBOX_KHR present mode selected\n");
                        break;
                    }

                }
                presentsAnFormatsInfo.avaiablePresentMode = VK_PRESENT_MODE_FIFO_KHR;
                printf("VK_PRESENT_MODE_FIFO_KHR present mode selected\n");
    //            avaiablePresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    //            printf("%u\n", avaiablePresentMod);
            }
        }
        else
        {
            printf("Your system might not be Vulkan compatible, please double check your system.\n");
            
//            free(formats);
//            free(presentModes);
//            return -1;
        }

    //    free memory allocation for formats and presentMode varibles
        free(formats);
        free(presentModes);
    
    return presentsAnFormatsInfo;
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
    
    
    const char* instance_validation_layers = NULL;
 
//    1. Initialising the validation layer.
//    Currently if its shut off the program crashes.
//    TO DO: FIX Disable functionality
    initValidationLayer(validationLayers, instance_validation_layers);


//    proceed with creating a GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, WINDOW_NAME, NULL, NULL);
    
    
//    2. Vulkan instance setup
//    Create our instances
    VkInstance instance = createInstance(validationLayers);
        
//    3.VkPhysicalDevice lets set up the physical devices our program is runnign on.
//    Setting up the physical devices
    VkPhysicalDevice physicalDevice = createPhysicalDevice(instance);

//    4. Create physical device surface
    VkSurfaceKHR surface = createSurface(instance, window);
    
    
//    5 :: Queue and graphics Family selections.
    // Logic to find queue family indices to populate struct with
//    Just below are the queue variable
    struct graphicsFamiliesAnIndices queueFamilyIndicesInfo = createGraFamInd(physicalDevice, surface);

    
//  6 :: Setting up a logical devices to interface with.
//    This is the main device that is used through out the rest of the program
    
//    Just like instance, this is a global variable
    VkDevice device = createLogicalDevice(physicalDevice, validationLayers, queueFamilyIndicesInfo.graphicsFamilyIndices, queueFamilyIndicesInfo.queueCount);
    
    
    
//   7.  Completing presentation queue
//    Global variable that is required for cleanup
    VkQueue graphicsQueue = createGraphicsQueue(device, queueFamilyIndicesInfo);
    
//    8 : We are now checking for compatibilities we desire and whats possible with the
//    physicalDevice.
    
//    This can be abstracted quickly
//    Global variable that is required for cleanup
    VkSurfaceCapabilitiesKHR capabilities = checkPhysicalDeviceCapabilities(physicalDevice, surface);

//    Lets create a custom struct that is needed for the swap chain and the commandbuffers
//    Could extend this to include VkExtent2D extent &  uint32_t imageCount
    struct availablePresentsAnFormats presentsAnFormatsInfo = findPresentsAnFormats(physicalDevice, surface);
    
    
    /**
     This block can be added to the above availablePresentsAnFormats struct
     */
//    capabilities is required for this statement. However this
//    statement is also not doing anything
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        printf("\ncapabilities.currentExtent.width != UINT32_MAX...\n");
        printf("Still to check if this is bad...\n\n");
    }
    
//    STEP 6.2:: Setting up the action swap chain functionality
//    Remember we initialised this extension before in step 5.
    
//    This is a global variable - its also using capabilities
    VkExtent2D extent = capabilities.currentExtent;
    
//  This is a reoccuring variable that is used to fill several
//    data points in other structs
    uint32_t imageCount = capabilities.minImageCount + 1;
    
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }
    
    /**
     End of this code block note
     */
    
    
    
    
/**
 Creation of the swapchain
 */
    
    
//    Global struct that pertains to the swapchain
    VkSwapchainCreateInfoKHR createSwapChainInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = presentsAnFormatsInfo.availableFormats.format,
        .imageColorSpace = presentsAnFormatsInfo.availableFormats.colorSpace,
        .presentMode = presentsAnFormatsInfo.avaiablePresentMode,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };
    
    
    if (queueFamilyIndicesInfo.graphicsFamilyIndices != queueFamilyIndicesInfo.presentFamily)
    {
        printf("Graphics family does not equal present family\n\n");
        createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createSwapChainInfo.queueFamilyIndexCount = 2;
        createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndicesInfo.queueFamilyIndices;
    }
    else
    {
        printf("Graphics family equal present family\n\n");
        createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createSwapChainInfo.queueFamilyIndexCount = 0; // Optional
        createSwapChainInfo.pQueueFamilyIndices = queueFamilyIndicesInfo.queueFamilyIndices; // Optional
    }
    
//    global variable - this is our swapchain
    VkSwapchainKHR swapChainKHR;
//    temp rtn variable for swapchain creation
    VkResult swapChainCreateResult = vkCreateSwapchainKHR(device, &createSwapChainInfo, NULL, &swapChainKHR);
    if (swapChainCreateResult != VK_SUCCESS)
    {
        printf("failed to create swap chain!\n");
        return -1;
    }
    
    
    /**
     This block pertains to    createImageViews(); from tutorial
     */
//    local malloc variable that is freed at the end of this code block
    VkImage* swapChainImages = malloc(sizeof(VkImage) * 1);
    
//    These two functions should really be wrapped in if statements to make sure they are successful
    vkGetSwapchainImagesKHR(device, swapChainKHR, &imageCount, NULL);
    vkGetSwapchainImagesKHR(device, swapChainKHR, &imageCount, swapChainImages);
//    reoccurring variable for tracking swapchain image count
    int swapChainImagesCount = (int) sizeof(swapChainImages) / (int) swapChainImages[0];
//    Global malloc variable that is freed at the end of program
    VkImageView* swapChainImageViews = malloc(sizeof(VkImageView) * swapChainImagesCount);
    
    
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        VkImageViewCreateInfo createImageViewInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = presentsAnFormatsInfo.availableFormats.format,
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
    free(swapChainImages);
    
    
    
    

//    7 :: Graphics pipeline
//    After a long arguous process, we are finally at the graphics pipe line!

//    this whole section can be abstracted into functions and return a single variable
//    Functions needed: (1) parseShaders; (2) compileShaders; (3) createShaderProgram
    
//    7.1 : Shaders (Vertex Shader first)::
//    To Do: Abstract this fundtion
    char vertPath[] =
                    "#version 450\n"
                    "#extension GL_ARB_separate_shader_objects : enable\n"
                    "layout(location = 0) out vec3 fragColor;\n"
                    "vec2 positions[3] = vec2[](vec2(0.0, -0.5),vec2(0.5, 0.5),vec2(-0.5, 0.5));\n"
                    "vec3 colors[3] = vec3[](vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));\n"
                    "void main(){gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);fragColor = colors[gl_VertexIndex];}";
    
    
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
    
//    This is the final outcome of this code block
//    Furthermore these variables are used for the graphics pipeline
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    
//    Now that we have our frag shader we free the compiled results, and free the compiler itself
    shaderc_result_release(compilerVertResult);
    shaderc_result_release(compilerFragResult);
    shaderc_compiler_release(compiler);

    

//    7.2 : Fixing functions
//    This is the next major step
//    In Vulkan you have to be explicit about everything, from viewport size to color blending function.
    
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
    
    
// 7.3 : Creating the graphics pipeline
//    Global variable this is our pipline layout which will be submitted for drawing.
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
    

//    7.4 : Creating the rendering pass
//    Next major stage
    
//    This is an exact variable... Can be cut and instead etend availableFormats
//    struct type
    VkFormat VkColourFormat = presentsAnFormatsInfo.availableFormats.format;
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
    
//    Global variable that is needed for cleanup at the end of program
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
        return -1;
    }
    
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
        return -1;
    }


//    8 : setting up the command buffer and frame buffers plue the render
//    pass command pool
    
//    Global variable that is malloc with the determined size of the swapchain image count.
//    Needed for cleanup at the end.
    VkFramebuffer* swapChainFramebuffers = malloc(sizeof(VkFramebuffer) * swapChainImagesCount);
    
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        VkImageView attachments[] =
        {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1
        };


        if (vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            printf("failed to create framebuffer!\n");
            free(swapChainFramebuffers);
            return -1;
        }
    }
    
//    Lets create the commandpool now..
//    What the command pool exactly is.. to find out
    
//    Global variable that is needed also for cleanup at the end of program
    VkCommandPool commandPool;
    
    VkCommandPoolCreateInfo poolInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndicesInfo.presentFamily,
        .flags = 0 // Optional
    };
    
    if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        printf("failed to create command pool!\n");
        free(swapChainFramebuffers);
        return -1;
    }

    
//    Creating the command buffer
//    Global variable that is malloc with the determined size of the swapchain image count.
//    Needed for cleanup at the end.
    VkCommandBuffer* commandBuffers = malloc(sizeof(VkCommandBuffer) * swapChainImagesCount);
    
    VkCommandBufferAllocateInfo allocInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) sizeof(commandBuffers) 
    };
    

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) != VK_SUCCESS) {
        printf("failed to allocate command buffers!\n");
        
        free(commandBuffers);
        free(swapChainFramebuffers);
        return -1;
    }
    
//    Starting the command buffer recording
    int commandBufferSize = (int) sizeof(commandBuffers) / sizeof(commandBuffers[0]);
    
//    All values in this code block are temporary expect the variables that are being changed.
    for (int i = 0; i < commandBufferSize; i++)
    {
        VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0, // Optional
            .pInheritanceInfo = NULL // Optional
        };
          

       if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
           printf("failed to begin recording command buffer!\n");
           
           free(commandBuffers);
           free(swapChainFramebuffers);
           return -1;
       }
        
        VkRenderPassBeginInfo renderPassInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = swapChainFramebuffers[i],
            .renderArea.offset = {0, 0},
            .renderArea.extent = extent
        };
        
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        
        vkCmdEndRenderPass(commandBuffers[i]);
        
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            printf("failed to record command buffer!\n");
            
            free(commandBuffers);
            free(swapChainFramebuffers);
            return -1;
        }
       
    }
    


//  9 : rasterisation and Presentation of the triangle that we are drawing
    const int MAX_FRAMES_IN_FLIGHT = 2;
    size_t currentFrame = 0;
    
    VkSemaphore* imageAvailableSemaphore = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    VkSemaphore* renderFinishedSemaphore = malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    VkFence* inFlightFences = malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);
    VkFence* imagesInFlight = malloc(sizeof(VkFence) * swapChainImagesCount);
//    We have to iterate of imageInFlight to set it up with NULL in each array position
    for (int i = 0; i < swapChainImagesCount; i++)
    {
        imagesInFlight[i] = VK_NULL_HANDLE;
    }
        
    VkSemaphoreCreateInfo semaphoreInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    
    VkFenceCreateInfo fenceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
      
    
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS ) {

            printf("failed to create synchronization objects for a frame!\n");
            
            free(commandBuffers);
            free(swapChainFramebuffers);
            free(imageAvailableSemaphore);
            free(renderFinishedSemaphore);
            free(inFlightFences);
            free(imagesInFlight);
            return -1;
        }
    }
    
    


//    Main loop
//        This is the game loops
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
//        We tell our program to wait for the fence that we set up in section 9.
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapChainKHR, UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
    
//        We have to make sure that these two values are equal for it'll crash the progarm
        if (currentFrame == imageIndex)
        {
            if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
            {
                vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
            }
        }
        
//      Mark the image as now being in use by this frame
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];
        
        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .commandBufferCount = 1,
            .signalSemaphoreCount = 1
            
        };
        
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore[currentFrame]};
        submitInfo.pSignalSemaphores = signalSemaphores;
        
        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            printf("failed to submit draw command buffer!\n");
//            return -1;
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
//        vkQueueWaitIdle(graphicsQueue);
        
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    
    if (vkDeviceWaitIdle(device) == VK_SUCCESS)
    {
//            Program clean up
//            Will need to figure out how to abstract this effectively
//            for (int i = 0; i < swapChainImagesCount; i++)
//            {
//                vkDestroyFence(device, imagesInFlight[i], NULL);
//            }
            free(imagesInFlight);
            
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroySemaphore(device, renderFinishedSemaphore[i], NULL);
                vkDestroySemaphore(device, imageAvailableSemaphore[i], NULL);
                vkDestroyFence(device, inFlightFences[i], NULL);
            }
            free(imageAvailableSemaphore);
            free(renderFinishedSemaphore);
            free(inFlightFences);
            
             

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
            
        //    Second pipeline
        //    vkDestroyPipelineLayout(device, renderPipelineLayout, NULL);
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
