//
//  renderStorage.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/01/24.
//  Copyright © 2024 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_RENDERSTORAGE_HPP
#define MARLIN_RENDERSTORAGE_HPP

#include <marlin/vulkan/bufferPool.hpp>

namespace marlin
{

using VertexPoolHandle = BufferPoolHandleT< std::byte >;
using IndexPoolHandle = BufferPoolHandleT< uint32_t >;

class RenderStorage
{
public:

    RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice );
    ~RenderStorage() = default;
    
    VertexPoolHandle allocateVertexBuffer( uint32_t i_size );
    void deallocateVertexBuffer( const VertexPoolHandle &i_handle );
    
    IndexPoolHandle allocateIndexBuffer( uint32_t i_size );
    void deallocateIndexBuffer( const IndexPoolHandle &i_handle );

private:
    
    BufferPoolT< std::byte > m_vertexPool;
    BufferPoolT< uint32_t > m_indexPool;
};


} // namespace marlin

#endif /* MARLIN_RENDERSTORAGE_HPP */
