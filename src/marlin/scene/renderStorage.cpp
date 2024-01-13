//
//  renderStorage.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/01/24.
//  Copyright © 2024 Jonathan Graham. All rights reserved.
//

#include <marlin/scene/renderStorage.hpp>

namespace marlin
{

RenderStorage::RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice )
: m_vertexPool( i_device, i_physicalDevice, PoolUsage::Vertex )
, m_indexPool( i_device, i_physicalDevice, PoolUsage::Index )
{
}

BufferPoolHandle RenderStorage::allocateVertexBuffer( uint32_t i_size )
{
    return m_vertexPool.allocate( i_size );
}

void RenderStorage::deallocateVertexBuffer( const BufferPoolHandle &i_handle )
{
    m_vertexPool.deallocate( i_handle );
}

BufferPoolHandle RenderStorage::allocateIndexBuffer( uint32_t i_size )
{
    return m_indexPool.allocate( i_size );
}

void RenderStorage::deallocateIndexBuffer( const BufferPoolHandle &i_handle )
{
    m_indexPool.deallocate( i_handle );
}

} // namespace marlin
