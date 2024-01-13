//
//  bufferPool.cpp
//  Marlin
//
//  Created by Jonathan Graham on 12/11/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/bufferPool.hpp>

namespace marlin
{

const static size_t kPoolSize = 2048;

BufferPool::BufferPool( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, PoolUsage i_usage )
: m_device( i_device )
, m_physicalDevice( i_physicalDevice )
, m_vkUsage( VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM )
{
    if ( i_usage == PoolUsage::Vertex )
    {
        m_vkUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    else if ( i_usage == PoolUsage::Index )
    {
        m_vkUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
}

BufferPoolHandle BufferPool::allocate( uint32_t i_size )
{
    BufferPoolHandle handle;
    for ( uint32_t i = 0; i < m_entries.size(); i++ )
    {
        handle = allocate( i, i_size );
        if ( handle.allocation.offset != OffsetAllocator::Allocation::NO_SPACE )
        {
            break;
        }
    }
    
    if ( handle.allocation.offset == OffsetAllocator::Allocation::NO_SPACE )
    {
        BufferTPtr< std::byte > buffer = BufferT< std::byte >::create( m_device, m_physicalDevice, m_vkUsage, BufferMode::Local, nullptr, kPoolSize );
        m_entries.emplace_back( kPoolSize, buffer );
        handle = allocate( m_entries.size() - 1, i_size );
    }
    
    return handle;
}

void BufferPool::deallocate( const BufferPoolHandle &i_handle )
{
    m_entries[ i_handle.index ].allocator.free( i_handle.allocation );
}

BufferPoolHandle BufferPool::allocate( size_t i_index, uint32_t i_size )
{
    OffsetAllocator::Allocation allocation = m_entries[ i_index ].allocator.allocate( i_size );

    BufferPoolHandle handle;
    handle.allocation = allocation;
    handle.index = i_index;
    handle.buffer = m_entries[ i_index ].buffer;
    
    return handle;
}

} // namespace marlin
