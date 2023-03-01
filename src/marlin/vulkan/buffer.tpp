//
//  buffer.tpp
//  Marlin
//
//  Created by Jonathan Graham on 2/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include "commandBuffer.hpp"
#include "device.hpp"
#include "physicalDevice.hpp"

namespace marlin
{

inline uint32_t findMemoryType( const VkPhysicalDeviceMemoryProperties &i_deviceMemoryProperties,
                                uint32_t i_typeFilter,
                                const VkMemoryPropertyFlags &i_properties )
{
    for ( uint32_t i = 0; i < i_deviceMemoryProperties.memoryTypeCount; i++ )
    {
        if ( ( i_typeFilter & ( 1 << i ) ) && ( i_deviceMemoryProperties.memoryTypes[i].propertyFlags & i_properties ) == i_properties )
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

inline void createBuffer( DevicePtr i_device,
                          VkDeviceSize i_size,
                          const VkPhysicalDeviceMemoryProperties &i_deviceMemoryProperties,
                          VkBufferUsageFlags i_usage,
                          VkMemoryPropertyFlags i_properties,
                          VkBuffer &o_buffer,
                          VkDeviceMemory &o_bufferMemory )
{
    VkBufferCreateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = i_size,
        .usage = i_usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .flags = 0,
    };

    if ( vkCreateBuffer( i_device->getObject(), &bufferInfo, nullptr, &o_buffer ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( i_device->getObject(), o_buffer, &memRequirements );

    VkMemoryAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType( i_deviceMemoryProperties, memRequirements.memoryTypeBits, i_properties )
    };

    if ( vkAllocateMemory( i_device->getObject(), &allocInfo, nullptr, &o_bufferMemory ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory( i_device->getObject(), o_buffer, o_bufferMemory, 0 );
}

inline void copyBuffer( DevicePtr i_device, VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size )
{
    CommandBufferPtr commandBuffer = i_device->getCommandBuffer( QueueTypeTransfer );
    commandBuffer->reset();
    
    {
        CommandBufferRecordPtr scopedRecord = commandBuffer->scopedRecord( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
                
        VkBufferCopy copyRegion {
            .srcOffset = 0, // Optional
            .dstOffset = 0, // Optional
            .size = i_size
        };

        vkCmdCopyBuffer( commandBuffer->getObject(), i_srcBuffer, i_dstBuffer, 1, &copyRegion );
    }
    
    VkCommandBuffer vkCommandBuffer = commandBuffer->getObject();
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer
    };

    VkQueue queue = i_device->getQueue( QueueTypeTransfer, 0 );
    vkQueueSubmit( queue, 1, &submitInfo, VK_NULL_HANDLE );
    vkQueueWaitIdle( queue );
}

template < class T >
std::shared_ptr< BufferT< T > > BufferT< T >::create( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, const std::vector< T > &i_data, VkBufferUsageFlagBits i_usage )
{
    return std::make_shared< BufferT< T > >( i_device, i_physicalDevice, i_data, i_usage );
}

template < class T >
BufferT< T >::BufferT( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, const std::vector< T > &i_data, VkBufferUsageFlagBits i_usage )
: m_device( i_device )
{
    const VkDeviceSize size = sizeof( T ) * i_data.size();
    const VkPhysicalDeviceMemoryProperties deviceMemoryProperties = i_physicalDevice->getMemoryProperties();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer( i_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

    void* data;
    vkMapMemory( i_device->getObject(), stagingBufferMemory, 0, size, 0, &data );
    memcpy( data, i_data.data(), static_cast< size_t >( size ) );
    vkUnmapMemory( i_device->getObject(), stagingBufferMemory );

    createBuffer( i_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_DST_BIT | i_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_object, m_memory );

    copyBuffer( m_device, stagingBuffer, m_object, size );

    vkDestroyBuffer( i_device->getObject(), stagingBuffer, nullptr );
    vkFreeMemory( i_device->getObject(), stagingBufferMemory, nullptr );
}

template < class T >
BufferT< T >::~BufferT()
{
    if ( m_object != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Buffer object not released." << std::endl;
    }
    
    if ( m_memory != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Buffer memory not released." << std::endl;
    }
}

template < class T >
void BufferT< T >::destroy()
{
    vkDestroyBuffer( m_device->getObject(), m_object, nullptr );
    vkFreeMemory( m_device->getObject(), m_memory, nullptr );
    
    m_object = VK_NULL_HANDLE;
}

} // namespace marlin