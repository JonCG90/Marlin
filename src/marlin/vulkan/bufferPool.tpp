//
//  bufferPool.tpp
//  Marlin
//
//  Created by Jonathan Graham on 12/11/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/bufferPool.hpp>

namespace marlin
{

template < class T >
PoolEntryT< T >::PoolEntryT( OffsetAllocator::Allocator i_allocator, BufferTPtr< T > i_buffer )
: allocator( std::move( i_allocator ) )
, buffer( i_buffer )
{}

template < class T >
BufferPoolT< T >::BufferPoolT( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, PoolUsage i_usage, size_t i_size )
: m_device( i_device )
, m_physicalDevice( i_physicalDevice )
, m_vkUsage( VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM )
, m_poolSize( i_size )
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

template < class T >
BufferPoolHandleT< T > BufferPoolT< T >::allocate( uint32_t i_size )
{
    BufferPoolHandleT< T > handle;
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
        BufferTPtr< T > buffer = BufferT< T >::create( m_device, m_physicalDevice, m_vkUsage, BufferMode::Local, nullptr, m_poolSize );
        m_entries.emplace_back( m_poolSize, buffer );
        handle = allocate( m_entries.size() - 1, i_size );
    }
    
    return handle;
}

template < class T >
void BufferPoolT< T >::deallocate( const BufferPoolHandleT< T > &i_handle )
{
    m_entries[ i_handle.index ].allocator.free( i_handle.allocation );
}

template < class T >
BufferPoolHandleT< T > BufferPoolT< T >::allocate( size_t i_index, uint32_t i_size )
{
    OffsetAllocator::Allocation allocation = m_entries[ i_index ].allocator.allocate( i_size );

    BufferPoolHandleT< T > handle;
    handle.allocation = allocation;
    handle.index = i_index;
    handle.buffer = m_entries[ i_index ].buffer;
    
    return handle;
}

} // namespace marlin
