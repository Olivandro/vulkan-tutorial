//
//  main.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 29/09/2020.
//

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linmath.h"



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
        vkEnumerateInstanceLayerProperties(&layerCount, instance_layers);
        
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
                printf("Cannot find layer: %s\n", &instance_validation_layers[i]);
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
        printf("failed to create Vulkan instance! Error code: %d\n", (int)result);
    else
        printf("VkInstance successfully created!\n Return code: %d \n", (int)result);

    
//  Check for number of available extension
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    printf("Number of extensions supported: %d \n", extensionCount);
    
//  lets print out the names of the extensions available
    VkExtensionProperties* instance_extensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, instance_extensions);
//    for (int i = 0; i < extensionCount; i++) {
//        printf("Extension name: %s \n", instance_extensions[i].extensionName);
//    }
    
//    free the allocated memory for instance_extensions
    free(instance_extensions);

    
//    :Next major step
//    Setting up the physical devices
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (deviceCount > 0) {
        printf("GPU device found!\n");
        VkPhysicalDevice* physical_device = malloc(sizeof(VkPhysicalDevice) * deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physical_device);
        /* For tri demo we just grab the first physical device */
        physicalDevice = physical_device[0];
        free(physical_device);
    }
    
//    Queue Family... Whatever that means...!!
    // Logic to find queue family indices to populate struct with
    uint32_t graphicsFamilyIndices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
    if (queueFamilyCount >= 1)
    {
        printf("%d Family queue found, number of queues per family group: %zu \n", queueFamilyCount, queueFamilies->queueCount);
        for (int i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                    graphicsFamilyIndices = i;
                    printf("Graphics Family indices: %d \n", graphicsFamilyIndices);
                    printf("Queue Flags: %zu \n", queueFamilies[i].queueFlags);
                    break; /* Use break to prevent the */
            }
        }
        
    }
        
    VkDevice device;
    
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsFamilyIndices,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };
    
    VkPhysicalDeviceFeatures deviceFeatures;
//    Set all physical device features to false.
    memset(&deviceFeatures, 0, sizeof(deviceFeatures));
    
    VkDeviceCreateInfo deviceCreateinfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &queueCreateInfo,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = 0,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = &validationLayers
    };
    
    
    if (vkCreateDevice(physicalDevice, &deviceCreateinfo, NULL, &device) != VK_SUCCESS) {
        printf("failed to create logical device!\n");
    }

    
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, graphicsFamilyIndices, 0, &graphicsQueue);
    
    
    
//    Math library tests
    mat4x4 matrix;
    vec4 r;
    vec4 v;
    mat4x4_identity(matrix);

    mat4x4_mul_vec4(r, matrix, v);
    uint32_t veclen = vec4_len(r);
    printf("Vec4 variable r length: %u \n", veclen);


//    Main loop
    while(!glfwWindowShouldClose(window))
    {
            glfwPollEvents();
    }

    
//    Program clean up
    free(queueFamilies);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    
    return 0;
}
