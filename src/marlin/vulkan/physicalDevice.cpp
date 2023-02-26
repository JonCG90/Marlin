//
//  physicalDevice.cpp
//  Marlin
//
//  Created by Jonathan Graham on 10/19/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include "physicalDevice.hpp"
#include "surface.hpp"

namespace marlin
{

QueueFamily::QueueFamily( VkPhysicalDevice i_physicalDevice, const VkQueueFamilyProperties &i_properties, uint32_t i_index )
: m_physicalDevice( i_physicalDevice )
, m_properties( i_properties )
, m_index( i_index )
{
}

bool QueueFamily::hasGraphics() const
{
    return m_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
}

bool QueueFamily::hasCompute() const
{
    return m_properties.queueFlags & VK_QUEUE_COMPUTE_BIT;
}

bool QueueFamily::hasTransfer() const
{
    return m_properties.queueFlags & VK_QUEUE_TRANSFER_BIT;
}

bool QueueFamily::isSurfaceSupported( const SurfacePtr &i_surface ) const
{
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR( m_physicalDevice, m_index, i_surface->getObject(), &supported );
    
    return supported;
}

uint32_t QueueFamily::getIndex() const
{
    return m_index;
}

uint32_t QueueFamily::count() const
{
    return m_properties.queueCount;
}

PhysicalDevicePtrs PhysicalDevice::getPhysicalDevices( VkInstance i_instance )
{
    VkInstance instance = i_instance;
    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

    PhysicalDevicePtrs devices;
    if ( deviceCount == 0 )
    {
        return devices;
    }
    
    std::vector< VkPhysicalDevice > vkDevices( deviceCount );
    vkEnumeratePhysicalDevices( instance, &deviceCount, vkDevices.data() );
    
    devices.reserve( deviceCount );
    for ( VkPhysicalDevice vkDevice : vkDevices )
    {
        PhysicalDevicePtr device = std::make_shared< PhysicalDevice >( vkDevice );
        devices.push_back( device );
    }
    
    return devices;
}

PhysicalDevice::PhysicalDevice( VkPhysicalDevice i_device )
: VkObjectT< VkPhysicalDevice >( i_device )
{
}

VkPhysicalDeviceProperties PhysicalDevice::getProperties() const
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties( m_object, &properties );
    
    return properties;
}

VkPhysicalDeviceFeatures PhysicalDevice::getFeatures() const
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures( m_object, &features );
    
    return features;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::getMemoryProperties() const
{
    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties( m_object, &properties );
    
    return properties;
}

void PhysicalDevice::getQueueFamilies( QueueFamilies &o_queueFamilies ) const
{
    std::vector< VkQueueFamilyProperties > properties;
    
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( m_object, &count, nullptr );
    
    properties.resize( count );
    vkGetPhysicalDeviceQueueFamilyProperties( m_object, &count, properties.data() );
    
    o_queueFamilies.clear();
    o_queueFamilies.reserve( count );
    
    for ( uint32_t i = 0; i < count; i++ )
    {
        const VkQueueFamilyProperties &property = properties[ i ];
        o_queueFamilies.emplace_back( m_object, property, i );
    }
}

void PhysicalDevice::getExtensions( std::vector< VkExtensionProperties > &extensions ) const
{
    uint32_t count;
    vkEnumerateDeviceExtensionProperties( m_object, nullptr, &count, nullptr );

    extensions.resize( count );
    vkEnumerateDeviceExtensionProperties( m_object, nullptr, &count, extensions.data() );
}

SwapChainSupportDetails PhysicalDevice::getSwapChainSupportDetails( SurfacePtr i_surface ) const
{
    VkSurfaceKHR surface = i_surface->getObject();
    SwapChainSupportDetails details;
    
    // Query capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_object, surface, &details.capabilities );
    
    // Query formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR( m_object, surface, &formatCount, nullptr );

    if ( formatCount != 0 )
    {
        details.formats.resize( formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( m_object, surface, &formatCount, details.formats.data() );
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR( m_object, surface, &presentModeCount, nullptr );

    if ( presentModeCount != 0 )
    {
        details.presentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( m_object, surface, &presentModeCount, details.presentModes.data() );
    }
    
    return details;
}

} // namespace marlin
