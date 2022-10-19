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

} // namespace marlin
