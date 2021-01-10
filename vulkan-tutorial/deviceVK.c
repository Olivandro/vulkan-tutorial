//
//  deviceVK.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 10/01/2021.
//

#include "deviceVK.h"


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
