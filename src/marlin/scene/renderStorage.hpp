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

class RenderStorage
{
public:

    RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice );
    ~RenderStorage() = default;
    
    BufferPoolHandle allocateVertexBuffer( uint32_t i_size );
    void deallocateVertexBuffer( const BufferPoolHandle &i_handle );
    
    BufferPoolHandle allocateIndexBuffer( uint32_t i_size );
    void deallocateIndexBuffer( const BufferPoolHandle &i_handle );

private:
    
    BufferPool m_vertexPool;
    BufferPool m_indexPool;

//    BufferTPtr< std::byte > m_vertexBuffer;
//    BufferTPtr< uint32_t > m_indexBuffer;
    
};


} // namespace marlin

#endif /* MARLIN_RENDERSTORAGE_HPP */
