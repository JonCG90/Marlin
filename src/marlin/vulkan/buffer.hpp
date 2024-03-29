//
//  buffer.hpp
//  Marlin
//
//  Created by Jonathan Graham on 2/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_BUFFER_HPP
#define MARLIN_BUFFER_HPP

#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/vkObject.hpp>

namespace marlin
{

enum class BufferMode
{
    Local,
    Device,
};

template < class T >
class BufferT : public VkObjectT< VkBuffer >
{
public:
    
    static BufferTPtr< T > create( DevicePtr i_device,
                                   PhysicalDevicePtr i_physicalDevice,
                                   VkBufferUsageFlags i_usage,
                                   BufferMode i_mode,
                                   const std::vector< T > &i_data );
    static BufferTPtr< T > create( DevicePtr i_device,
                                   PhysicalDevicePtr i_physicalDevice,
                                   VkBufferUsageFlags i_usage,
                                   BufferMode i_mode,
                                   T i_data );
    static BufferTPtr< T > create( DevicePtr i_device,
                                   PhysicalDevicePtr i_physicalDevice,
                                   VkBufferUsageFlags i_usage,
                                   BufferMode i_mode,
                                   const T* i_data,
                                   size_t i_size );
    
    BufferT( DevicePtr i_device,
             PhysicalDevicePtr i_physicalDevice,
             VkBufferUsageFlags i_usage,
             BufferMode i_mode,
             const std::vector< T > &i_data );
    
    BufferT( DevicePtr i_device,
             PhysicalDevicePtr i_physicalDevice,
             VkBufferUsageFlags i_usage,
             BufferMode i_mode,
             const T* i_data,
             size_t i_size );
    
    BufferT() = default;
    ~BufferT() override;
    
    void* mapMemory();
    void unmapMemory();
    void updateData( const T* i_data, size_t offset, size_t i_count );
    void updateData( const std::vector< T > &i_data, size_t i_offset );

    size_t getCount() const;
    void destroy();
    
private:
     
    size_t m_count;
    VkDeviceSize m_size;
    DevicePtr m_device;
    PhysicalDevicePtr m_physicalDevice;
    VkDeviceMemory m_memory;
    BufferMode m_mode;
};

} // namespace marlin

#include "buffer.tpp"

#endif /* MARLIN_BUFFER_HPP */
