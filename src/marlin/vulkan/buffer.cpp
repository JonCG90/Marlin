//
//  buffer.cpp
//  Marlin
//
//  Created by Jonathan Graham on 2/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include "buffer.hpp"
#include "device.hpp"
#include "physicalDevice.hpp"

namespace marlin
{

uint32_t findMemoryType( const VkPhysicalDeviceMemoryProperties &i_deviceMemoryProperties,
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

void createBuffer( VkDevice i_device,
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
    
    if ( vkCreateBuffer( i_device, &bufferInfo, nullptr, &o_buffer ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create buffer!");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( i_device, o_buffer, &memRequirements );
    
    VkMemoryAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType( i_deviceMemoryProperties, memRequirements.memoryTypeBits, i_properties )
    };
    
    if ( vkAllocateMemory( i_device, &allocInfo, nullptr, &o_bufferMemory ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
    
    vkBindBufferMemory( i_device, o_buffer, o_bufferMemory, 0 );
}

void copyBuffer( VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size )
{
//    VkCommandBufferAllocateInfo allocInfo {
//        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
//        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
//        .commandPool = m_commandPool,
//        .commandBufferCount = 1
//    };
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers( m_device->getObject(), &allocInfo, &commandBuffer );
//
//    VkCommandBufferBeginInfo beginInfo {
//        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
//        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
//    };
//
//    vkBeginCommandBuffer( commandBuffer, &beginInfo );
//
//    VkBufferCopy copyRegion {
//        .srcOffset = 0, // Optional
//        .dstOffset = 0, // Optional
//        .size = i_size
//    };
//
//    vkCmdCopyBuffer( commandBuffer, i_srcBuffer, i_dstBuffer, 1, &copyRegion );
//
//    vkEndCommandBuffer( commandBuffer );
//
//    VkSubmitInfo submitInfo {
//        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
//        .commandBufferCount = 1,
//        .pCommandBuffers = &commandBuffer
//    };
//
//    vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
//    vkQueueWaitIdle( m_graphicsQueue );
//
//    vkFreeCommandBuffers( m_device->getObject(), m_commandPool, 1, &commandBuffer );
}

BufferPtr Buffer::create( DevicePtr i_device )
{
//    const std::vector<Vertex> vertices = {
//        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
//    };
//
//    const VkDeviceSize size = sizeof(Vertex) * vertices.size();
//    const VkPhysicalDeviceMemoryProperties deviceMemoryProperties = i_physicalDevice->getMemoryProperties();
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//
//    createBuffer( i_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );
//
//    void* data;
//    vkMapMemory( i_device->getObject(), stagingBufferMemory, 0, size, 0, &data );
//    memcpy( data, vertices.data(), static_cast< size_t >( size ) );
//    vkUnmapMemory( i_device->getObject(), stagingBufferMemory );
//
//    createBuffer( i_device, size, deviceMemoryProperties, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_object, m_memory );
//
//    copyBuffer( stagingBuffer, m_object, size );
//
//    vkDestroyBuffer( i_device->getObject(), stagingBuffer, nullptr );
//    vkFreeMemory( i_device->getObject(), stagingBufferMemory, nullptr );
    
    return nullptr;
}

Buffer::Buffer( VkDevice i_device )
: m_device( i_device )
{
}

Buffer::~Buffer()
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

void Buffer::destroy()
{
    vkDestroyBuffer( m_device, m_object, nullptr );
    vkFreeMemory( m_device, m_memory, nullptr );
    
    m_object = VK_NULL_HANDLE;
}

} // namespace marlin
