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

template < class T >
struct BufferPoolHandleT
{
    size_t index = 0;
    OffsetAllocator::Allocation allocation;
    BufferTPtr< T > buffer;
};

template < class T >
struct PoolEntryT
{
    PoolEntryT( OffsetAllocator::Allocator i_allocator, BufferTPtr< T > i_buffer );
    
    OffsetAllocator::Allocator allocator;
    BufferTPtr< T > buffer;
};

template < class T >
class BufferPoolT
{
public:

    BufferPoolT( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, PoolUsage i_usage );
    ~BufferPoolT() = default;
    
    BufferPoolHandleT< T > allocate( uint32_t i_size );
    void deallocate( const BufferPoolHandleT< T > &i_handle );

private:
    
    BufferPoolHandleT< T > allocate( size_t i_index, uint32_t i_size );
    
    DevicePtr m_device;
    PhysicalDevicePtr m_physicalDevice;
    VkBufferUsageFlagBits m_vkUsage;
    
    std::vector< PoolEntryT< T > > m_entries;
};

} // namespace marlin

#include "bufferPool.tpp"

#endif /* MARLIN_BUFFERPOOL_HPP */
