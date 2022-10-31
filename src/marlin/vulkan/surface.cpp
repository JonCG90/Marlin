//
//  surface.cpp
//  Marlin
//
//  Created by Jonathan Graham on 10/24/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include "surface.hpp"

#include <vulkan/vulkan_metal.h>

#include <stdexcept>

namespace marlin
{

Surface Surface::create( VkInstance i_instance, void* i_layer )
{
    VkMetalSurfaceCreateInfoEXT createSurfaceInfo {
        .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .pLayer = i_layer
    };

    VkSurfaceKHR vkSurface;
    if ( vkCreateMetalSurfaceEXT( i_instance, &createSurfaceInfo, nullptr, &vkSurface ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to create Vulkan instance." );
    }
    
    return Surface( vkSurface );
}

Surface::Surface( VkSurfaceKHR i_surface )
: VkObjectT<VkSurfaceKHR>( i_surface )
{}

} // namespace marlin

