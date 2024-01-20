//
//  buffer.tpp
//  Marlin
//
//  Created by Jonathan Graham on 2/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/commandBuffer.hpp>
#include <marlin/vulkan/device.hpp>
#include <marlin/vulkan/physicalDevice.hpp>

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

    throw std::runtime_error( "Error: Failed to find suitable memory type." );
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
        throw std::runtime_error( "Error: Failed to  buffer" );
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
        throw std::runtime_error( "Error: Failed to allocate buffer memory" );
    }

    vkBindBufferMemory( i_device->getObject(), o_buffer, o_bufferMemory, 0 );
}

inline void copyBuffer( DevicePtr i_device, VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_srcOffset, VkDeviceSize i_dstOffset, VkDeviceSize i_size )
{
    CommandBufferPtr commandBuffer = i_device->getCommandBuffer( QueueTypeTransfer, 0 );
    commandBuffer->reset();
    
    {
        CommandBufferRecordPtr scopedRecord = commandBuffer->scopedRecord( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
                
        VkBufferCopy copyRegion {
            .srcOffset = i_srcOffset,
            .dstOffset = i_dstOffset,
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
BufferTPtr< T > BufferT< T >::create( DevicePtr i_device,
                                      PhysicalDevicePtr i_physicalDevice,
                                      VkBufferUsageFlags i_usage,
                                      BufferMode i_mode,
                                      const std::vector< T > &i_data )
{
    return std::make_shared< BufferT< T > >( i_device, i_physicalDevice, i_usage, i_mode, i_data );
}

template < class T >
BufferTPtr< T > BufferT< T >::create( DevicePtr i_device,
                                      PhysicalDevicePtr i_physicalDevice,
                                      VkBufferUsageFlags i_usage,
                                      BufferMode i_mode,
                                      T i_data )
{
    const std::vector< T > data { i_data };
    return std::make_shared< BufferT< T > >( i_device, i_physicalDevice, i_usage, i_mode, data );
}

template < class T >
BufferTPtr< T > BufferT< T >::create( DevicePtr i_device,
                                      PhysicalDevicePtr i_physicalDevice,
                                      VkBufferUsageFlags i_usage,
                                      BufferMode i_mode,
                                      const T* i_data,
                                      size_t i_count )
{
    return std::make_shared< BufferT< T > >( i_device, i_physicalDevice, i_usage, i_mode, i_data, i_count );
}

template < class T >
BufferT< T >::BufferT( DevicePtr i_device,
                       PhysicalDevicePtr i_physicalDevice,
                       VkBufferUsageFlags i_usage,
                       BufferMode i_mode,
                       const std::vector< T > &i_data )
: BufferT< T >( i_device, i_physicalDevice, i_usage, i_mode, i_data.data(), i_data.size() * sizeof( T ) )
{
}

template < class T >
BufferT< T >::BufferT( DevicePtr i_device,
                       PhysicalDevicePtr i_physicalDevice,
                       VkBufferUsageFlags i_usage,
                       BufferMode i_mode,
                       const T* i_data,
                       size_t i_count )
: m_device( i_device )
, m_physicalDevice( i_physicalDevice )
, m_mode( i_mode )
, m_count( i_count )
{
    const VkDeviceSize size = i_count * sizeof( T );
    const VkPhysicalDeviceMemoryProperties deviceMemoryProperties = i_physicalDevice->getMemoryProperties();

    switch ( i_mode ) {
        case BufferMode::Local:
            {
                createBuffer( i_device, size, deviceMemoryProperties, i_usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_object, m_memory );
             
                if ( i_data != nullptr )
                {
                    void* dest;
                    vkMapMemory( i_device->getObject(), m_memory, 0, size, 0, &dest );
                    memcpy( dest, i_data, static_cast< size_t >( size ) );
                    vkUnmapMemory( i_device->getObject(), m_memory );
                }
            }
            break;
        case BufferMode::Device:
            {
                createBuffer( i_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_DST_BIT | i_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_object, m_memory );
                
                if ( i_data != nullptr )
                {
                    VkBuffer stagingBuffer;
                    VkDeviceMemory stagingBufferMemory;

                    createBuffer( i_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

                    void* dest;
                    vkMapMemory( i_device->getObject(), stagingBufferMemory, 0, size, 0, &dest );
                    memcpy( dest, i_data, static_cast< size_t >( size ) );
                    vkUnmapMemory( i_device->getObject(), stagingBufferMemory );

                    copyBuffer( m_device, stagingBuffer, m_object, 0, 0, size );

                    vkDestroyBuffer( i_device->getObject(), stagingBuffer, nullptr );
                    vkFreeMemory( i_device->getObject(), stagingBufferMemory, nullptr );
                }
            }
            break;
            
        default:
            break;
    }
    
    m_size = size;
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
void* BufferT< T >::mapMemory()
{
    if ( m_mode != BufferMode::Local )
    {
        throw std::runtime_error( "Mapping non local buffers is not supported." );
    }
    
    void *data;
    vkMapMemory( m_device->getObject(), m_memory, 0, m_size, 0, &data );
    return data;
}

template < class T >
void BufferT< T >::unmapMemory()
{
    if ( m_mode != BufferMode::Local )
    {
        throw std::runtime_error( "Unmapping non local buffers is not supported." );
    }
    
    vkUnmapMemory( m_device->getObject(), m_memory );
}

template < class T >
void BufferT< T >::updateData( const T* i_data, size_t i_offset, size_t i_count )
{
    if ( i_count == 0 )
    {
        return;
    }

    if ( ( i_offset + i_count ) * sizeof( T ) > m_size )
    {
        throw std::runtime_error( "Trying to update out of bounds memory." );
    }
    
    VkDevice device = m_device->getObject();
    
    VkDeviceSize offset = i_offset * sizeof( T );
    VkDeviceSize size = i_count * sizeof( T );

    switch ( m_mode ) {
        case BufferMode::Local:
            {
                void *dest;
                vkMapMemory( device, m_memory, offset, size, 0, &dest );
                memcpy( dest, i_data, static_cast< size_t >( size ) );
                vkUnmapMemory( device, m_memory );
            }
            break;
        case BufferMode::Device:
            {
                VkBuffer stagingBuffer;
                VkDeviceMemory stagingBufferMemory;

                const VkPhysicalDeviceMemoryProperties deviceMemoryProperties = m_physicalDevice->getMemoryProperties();
                createBuffer( m_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

                void* dest;
                vkMapMemory( device, stagingBufferMemory, 0, size, 0, &dest );
                memcpy( dest, i_data, static_cast< size_t >( size ) );
                vkUnmapMemory( device, stagingBufferMemory );

                copyBuffer( m_device, stagingBuffer, m_object, 0, offset, size );

                vkDestroyBuffer( device, stagingBuffer, nullptr );
                vkFreeMemory( device, stagingBufferMemory, nullptr );
            }
            break;
            
        default:
            break;
    }
}

template < class T >
void BufferT< T >::updateData( const std::vector< T > &i_data, size_t i_offset )
{
    updateData( i_data.data(), i_offset, i_data.size() );
}

template < class T >
size_t BufferT< T >::getCount() const
{
    return m_count;
}

template < class T >
void BufferT< T >::destroy()
{
    vkDestroyBuffer( m_device->getObject(), m_object, nullptr );
    vkFreeMemory( m_device->getObject(), m_memory, nullptr );
    
    m_object = VK_NULL_HANDLE;
}

} // namespace marlin
