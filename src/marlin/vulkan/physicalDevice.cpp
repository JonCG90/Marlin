//
//  physicalDevice.cpp
//  Marlin
//
//  Created by Jonathan Graham on 10/19/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include "physicalDevice.hpp"

namespace marlin
{

PhysicalDevices PhysicalDevice::getPhysicalDevices( VkInstance i_instance )
{
    VkInstance instance = i_instance;
    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

    PhysicalDevices devices;
    if ( deviceCount == 0 )
    {
        return devices;
    }
    
    std::vector< VkPhysicalDevice > vkDevices( deviceCount );
    vkEnumeratePhysicalDevices( instance, &deviceCount, vkDevices.data() );
    
    devices.reserve( deviceCount );
    for ( VkPhysicalDevice vkDevice : vkDevices )
    {
        devices.emplace_back( vkDevice );
    }
    
    return devices;
}

PhysicalDevice::PhysicalDevice( VkPhysicalDevice i_device )
: VkObjectT< VkPhysicalDevice >( i_device )
{
    
}

void PhysicalDevice::getQueueFamilyProperties( std::vector< VkQueueFamilyProperties > &properties )
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( m_object, &count, nullptr );
    
    properties.resize( count );
    vkGetPhysicalDeviceQueueFamilyProperties( m_object, &count, properties.data() );
}

void PhysicalDevice::getExtensions( std::vector< VkExtensionProperties > &extensions )
{
    uint32_t count;
    vkEnumerateDeviceExtensionProperties( m_object, nullptr, &count, nullptr );

    extensions.resize( count );
    vkEnumerateDeviceExtensionProperties( m_object, nullptr, &count, extensions.data() );
}

} // namespace marlin
