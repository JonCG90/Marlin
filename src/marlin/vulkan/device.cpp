//
//  device.cpp
//  Marlin
//
//  Created by Jonathan Graham on 10/31/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/device.hpp>

#include <marlin/vulkan/physicalDevice.hpp>
#include <marlin/vulkan/commandBuffer.hpp>

namespace marlin
{

static const std::vector< const char* > s_deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

DevicePtr Device::create( PhysicalDevicePtr i_device, const SurfacePtr i_surface, const QueueCreateCounts &i_queuesCounts, const BufferCreateCounts &i_bufferCounts )
{
    QueueFamilies deviceFamilies;
    i_device->getQueueFamilies( deviceFamilies );

    QueueToFamily queueFamilies;

    for ( const QueueFamily &family : deviceFamilies )
    {
        if ( family.hasGraphics() && ( i_queuesCounts.find( QueueTypeGraphics ) != i_queuesCounts.end() ) )
        {
            queueFamilies[ QueueTypeGraphics ] = family;
        }

        if ( family.hasTransfer() && ( i_queuesCounts.find( QueueTypeTransfer ) != i_queuesCounts.end() ) )
        {
            queueFamilies[ QueueTypeTransfer ] = family;
        }

        if ( family.hasCompute() && ( i_queuesCounts.find( QueueTypeCompute ) != i_queuesCounts.end() ) )
        {
            queueFamilies[ QueueTypeCompute ] = family;
        }

        if ( family.isSurfaceSupported( i_surface ) && ( i_queuesCounts.find( QueueTypePresent ) != i_queuesCounts.end() ) )
        {
            queueFamilies[ QueueTypePresent ] = family;
        }
    }

    std::map< uint32_t, std::vector< QueueType > > uniqueIndices;
    for ( const auto &pair : queueFamilies )
    {
        uniqueIndices[ pair.second.getIndex() ].push_back( pair.first );
    }

    float queuePriority = 1.0f;
    std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;

    for ( const auto &pair : uniqueIndices )
    {
        uint32_t familyIndex = pair.first;
        uint32_t queueCount = 1;
        for ( QueueType type : pair.second )
        {
            queueCount = std::max( queueCount, i_queuesCounts.at( type ) );
        }

        VkDeviceQueueCreateInfo queueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = familyIndex,
            .queueCount = queueCount,
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
    
    return std::make_shared< Device >( vkDevice, queueFamilies, i_bufferCounts );
}

Device::Device( VkDevice i_device, const QueueToFamily &i_supportedQueues, const BufferCreateCounts &i_bufferCounts )
: VkObjectT<VkDevice>( i_device )
, m_supportedQueues( i_supportedQueues )
, m_bufferCounts( i_bufferCounts )
{}

Device::~Device()
{
    if ( m_object != VK_NULL_HANDLE || !m_commandPools.empty() )
    {
        std::cerr << "Warning: Device object not released." << std::endl;
    }
}

VkQueue Device::getQueue( QueueType i_type, uint32_t i_index ) const
{
    THROW_INVALID( "Invalid Device" );
    
    const auto it = m_supportedQueues.find( i_type );
    if ( it == m_supportedQueues.end() )
    {
        throw std::runtime_error( "Requesting queue that was not created." );
    }
    
    const QueueFamily &family = it->second;
    if ( i_index >= family.count() )
    {
        throw std::runtime_error( "Queue index out of range." );
    }
    
    VkQueue queue;
    vkGetDeviceQueue( m_object, family.getIndex(), i_index, &queue );
    return queue;
}

CommandBufferPtr Device::getCommandBuffer( QueueType i_type, uint32_t i_index )
{
    THROW_INVALID( "Invalid Device" );
    
    if ( m_supportedQueues.find( i_type ) == m_supportedQueues.end() )
    {
        throw std::runtime_error( "Unsupported queue type." );
    }
    
    if ( i_index >= m_bufferCounts[ i_type ] )
    {
        throw std::runtime_error( "Trying to access command buffer outside of specified range." );
    }
    
    // Find existing
    const auto it = m_commandBuffers.find( i_type );
    if ( it != m_commandBuffers.end() && !it->second.empty() )
    {
        return it->second[ i_index ];
    }
    
    // Create one
    VkCommandPool commandPool = getCommandPool( i_type );
    std::vector< VkCommandBuffer > commandBuffers( m_bufferCounts[ i_type ] );
    
    VkCommandBufferAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = m_bufferCounts[ i_type ],
    };
    
    if ( vkAllocateCommandBuffers( m_object, &allocInfo, commandBuffers.data() ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to allocate command buffers!" );
    }
    
    std::vector< CommandBufferPtr > &queueCommandBuffer = m_commandBuffers[ i_type ];
    for ( VkCommandBuffer vkCommandBuffer : commandBuffers )
    {
        CommandBufferPtr commandBuffer = std::make_shared< CommandBuffer >( vkCommandBuffer );
        queueCommandBuffer.push_back( commandBuffer );
    }
    
    return queueCommandBuffer[ i_index ];
}

void Device::destroy()
{
    for ( const auto &pair : m_commandPools )
    {
        vkDestroyCommandPool( m_object, pair.second, nullptr );
    }
    
    m_commandBuffers.clear();
    m_commandPools.clear();

    vkDestroyDevice( m_object, nullptr );
    m_object = VK_NULL_HANDLE;
}

VkCommandPool Device::getCommandPool( QueueType i_type )
{
    // Find existing
    const auto it = m_commandPools.find( i_type );
    if ( it != m_commandPools.end() )
    {
        return it->second;
    }
    
    // Create new
    VkCommandPoolCreateInfo poolInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_supportedQueues.at( i_type ).getIndex(),
    };

    VkCommandPool commandPool;
    if ( vkCreateCommandPool( m_object, &poolInfo, nullptr, &commandPool ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to create command pool!" );
    }
    
    m_commandPools[ i_type ] = commandPool;
    
    return commandPool;
}

} // namespace marlin
