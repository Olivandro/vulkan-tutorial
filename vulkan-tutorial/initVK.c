//
//  initVK.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 21/12/2020.
//

#include "initVK.h"

//This function has to be expanded on... Currently its hard wired to use only a single layer
VkBool32 initValidationLayer(const char* validationLayers)
{
        VkBool32 foundvalidationLayer = 0;
        uint32_t layerCount = 0;
    //    Beginning to access and obtain count of validation layers available on system
    //    If you are using the LunarG SDK use vulkaninfo.app to verify your systems
    //    Vulkan implemenation.
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    
        const char*  instance_validation_layers = (const char*)validationLayers;
    
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
            printf("Number of layers avaiable: %u\n", layerCount);
            
    //        At this point we will iterator over our instance_layers struct and compare it
    //        with strcmp (from the string.h) to see if our validation layer request is
    //        available on out system (this was done by vkEnumerateInstanceLayerProperties)
            for (int i = 0; i < validationLayersLength; i++)
            {
                foundvalidationLayer = 0;
                for (int j = 0; j < layerCount; j++) {
//                    printf("System Validation layer: %s \n", instance_layers[j].layerName);
//                    If strcmp is == 0, i.e. if the strings are the same
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
            assert();
        }
    return foundvalidationLayer;
}


VkInstance createInstance(const char* validationLayers)
{
        uint32_t extensionCount = 0;
        VkInstance instance;
        VkApplicationInfo appInfo;
        VkInstanceCreateInfo createInfo;
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = NULL;
        
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
        {
            printf("failed to create Vulkan instance! Return code: %d\n", (int)result);
            assert();
        }
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
