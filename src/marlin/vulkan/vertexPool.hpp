//
//  vertexPool.hpp
//  Marlin
//
//  Created by Jonathan Graham on 12/11/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_VERTEXPOOL_HPP
#define MARLIN_VERTEXPOOL_HPP

#include <marlin/vulkan/defs.hpp>

#include <marlin/vulkan/buffer.hpp>

#include <thirdparty/OffsetAllocator/offsetAllocator.hpp>

namespace marlin
{

struct VertexPoolHandle
{
    size_t index;
    OffsetAllocator::Allocation allocation;
    BufferTPtr< std::byte > buffer;
};

class VertexPool
{
public:

    VertexPool( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice );
    ~VertexPool() = default;
    
    VertexPoolHandle allocate( uint32_t i_size );
    void deallocate( const VertexPoolHandle &i_handle );

private:
    
    struct PoolEntry
    {
        OffsetAllocator::Allocator allocator;
        BufferTPtr< std::byte > buffer;
    };
    
    VertexPoolHandle allocate( size_t i_index, uint32_t i_size );
    
    DevicePtr m_device;
    PhysicalDevicePtr m_physicalDevice;
    
    std::vector< PoolEntry > m_entries;
};

} // namespace marlin

#endif /* MARLIN_VERTEXPOOL_HPP */
