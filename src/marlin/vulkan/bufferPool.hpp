//
//  bufferPool.hpp
//  Marlin
//
//  Created by Jonathan Graham on 12/11/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_BUFFERPOOL_HPP
#define MARLIN_BUFFERPOOL_HPP

#include <marlin/vulkan/defs.hpp>

#include <marlin/vulkan/buffer.hpp>

#include <thirdparty/OffsetAllocator/offsetAllocator.hpp>

namespace marlin
{

enum class PoolUsage
{
    Vertex,
    Index,
};

struct BufferPoolHandle
{
    size_t index = 0;
    OffsetAllocator::Allocation allocation;
    BufferTPtr< std::byte > buffer;
};

class BufferPool
{
public:

    BufferPool( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, PoolUsage i_usage );
    ~BufferPool() = default;
    
    BufferPoolHandle allocate( uint32_t i_size );
    void deallocate( const BufferPoolHandle &i_handle );

private:
    
    struct PoolEntry
    {       
        PoolEntry( OffsetAllocator::Allocator i_allocator, BufferTPtr< std::byte > i_buffer )
        : allocator( std::move( i_allocator ) )
        , buffer( i_buffer )
        {}
        
        OffsetAllocator::Allocator allocator;
        BufferTPtr< std::byte > buffer;
    };
    
    BufferPoolHandle allocate( size_t i_index, uint32_t i_size );
    
    DevicePtr m_device;
    PhysicalDevicePtr m_physicalDevice;
    VkBufferUsageFlagBits m_vkUsage;
    
    std::vector< PoolEntry > m_entries;
};

} // namespace marlin

#endif /* MARLIN_BUFFERPOOL_HPP */
