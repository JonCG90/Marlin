//
//  surface.cpp
//  Marlin
//
//  Created by Jonathan Graham on 10/24/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/surface.hpp>

#include <vulkan/vulkan_metal.h>

#include <stdexcept>

namespace marlin
{

SurfacePtr Surface::create( VkInstance i_instance, void* i_layer )
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
    
    return std::make_shared< Surface >( vkSurface, i_instance );
}

Surface::Surface( VkSurfaceKHR i_surface, VkInstance i_instance )
: VkObjectT<VkSurfaceKHR>( i_surface )
, m_instance( i_instance )
{}

Surface::~Surface()
{
    if ( m_object != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Surface object not released." << std::endl;
    }
}

void Surface::destroy()
{
    vkDestroySurfaceKHR( m_instance, m_object, nullptr );
    m_object = VK_NULL_HANDLE;
}

} // namespace marlin

