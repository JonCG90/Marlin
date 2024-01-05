//
//  vertexPool.cpp
//  Marlin
//
//  Created by Jonathan Graham on 12/11/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/vertexPool.hpp>

#include <marlin/vulkan/buffer.hpp>

namespace marlin
{

const static size_t kPoolSize = 2048;

VertexPool::VertexPool( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice )
: m_device( i_device )
, m_physicalDevice( i_physicalDevice )
{
}

VertexPoolHandle VertexPool::allocate( uint32_t i_size )
{
    VertexPoolHandle handle;
    for ( uint32_t i = 0; i < m_entries.size(); i++ )
    {
        handle = allocate( i, i_size );
        if ( handle.allocation.offset != OffsetAllocator::Allocation::NO_SPACE )
        {
            break;
        }
    }
    
    if ( handle.allocation.offset != OffsetAllocator::Allocation::NO_SPACE )
    {
        BufferTPtr< std::byte > buffer = BufferT< std::byte >::create( m_device, m_physicalDevice, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, BufferMode::Device, nullptr, kPoolSize );
        
//        m_entries.emplace_back( kPoolSize, buffer );
        handle = allocate( m_entries.size() - 1, i_size );
    }
    
    return handle;
}

void VertexPool::deallocate( const VertexPoolHandle &i_handle )
{
    m_entries[ i_handle.index ].allocator.free( i_handle.allocation );
}

VertexPoolHandle VertexPool::allocate( size_t i_index, uint32_t i_size )
{
    OffsetAllocator::Allocation allocation = m_entries[ i_index ].allocator.allocate( i_size );

    VertexPoolHandle handle;
    handle.allocation = allocation;
    handle.index = i_index;
    
    return handle;
}

} // namespace marlin
