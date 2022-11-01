//
//  device.cpp
//  Marlin
//
//  Created by Jonathan Graham on 10/31/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include "device.hpp"

#include "physicalDevice.cpp"

#include <set>

namespace marlin
{

static const std::vector< const char* > s_deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

DevicePtr Device::create( PhysicalDevicePtr i_device, const QueueFamilies &i_families )
{
    std::set< uint32_t > familyIndices;
    for ( const QueueFamily &family : i_families )
    {
        familyIndices.insert( family.getIndex() );
    }
    
    float queuePriority = 1.0f;
    std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
    
    for ( uint32_t familyIndex : familyIndices )
    {
        VkDeviceQueueCreateInfo queueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = familyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        
        queueCreateInfos.push_back( queueCreateInfo );
    }
    
    VkPhysicalDeviceFeatures deviceFeatures {};

    VkDeviceCreateInfo deviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queueCreateInfos.data(),
        .queueCreateInfoCount = static_cast< uint32_t >( queueCreateInfos.size() ),
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = static_cast< uint32_t >( s_deviceExtensions.size() ),
        .ppEnabledExtensionNames = s_deviceExtensions.data()
    };

    
    VkDevice vkDevice;
    if ( vkCreateDevice( i_device->getObject(), &deviceCreateInfo, nullptr, &vkDevice ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create logical device." );
    }
    
    return std::make_shared< Device >( vkDevice );
}

Device::Device( VkDevice i_device )
: VkObjectT<VkDevice>( i_device )
{}

Device::~Device()
{
    if ( m_object != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Device object not released." << std::endl;
    }
}

VkQueue Device::getQueue( const QueueFamily &i_family, uint32_t i_index ) const
{
    if ( i_index >= i_family.count() )
    {
        throw std::runtime_error( "Queue index out of range." );
    }
    
    VkQueue queue;
    vkGetDeviceQueue( m_object, i_family.getIndex(), i_index, &queue );
    return queue;
}

void Device::destroy()
{
    vkDestroyDevice( m_object, nullptr );
    m_object = VK_NULL_HANDLE;
}

} // namespace marlin
