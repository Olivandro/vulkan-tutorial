//
//  geninc.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 08/12/2021.
//

#include "geninc.h"

bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (int i = 0; i < (int) sizeof(candidates) / sizeof(candidates[0]); i++) {
        VkFormat format = candidates[i];
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
    VkFormat format = findSupportedFormat(physicalDevice, candidates, VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    return format;
}
