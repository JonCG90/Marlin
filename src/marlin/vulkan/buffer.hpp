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
                                   VkBufferUsageFlagBits i_usage,
                                   BufferMode i_mode,
                                   const std::vector< T > &i_data );
    static BufferTPtr< T > create( DevicePtr i_device,
                                   PhysicalDevicePtr i_physicalDevice,
                                   VkBufferUsageFlagBits i_usage,
                                   BufferMode i_mode,
                                   T i_data );
    
    BufferT( DevicePtr i_device,
             PhysicalDevicePtr i_physicalDevice,
             VkBufferUsageFlagBits i_usage,
             BufferMode i_mode,
             const std::vector< T > &i_data );
    
    BufferT() = default;
    ~BufferT() override;
    
    void* mapMemory();
    void unmapMemory();

    size_t getCount() const;
    void destroy();
    
private:
       
    size_t m_count;
    VkDeviceSize m_size;
    DevicePtr m_device;
    VkDeviceMemory m_memory;
    BufferMode m_mode;
};

} // namespace marlin

#include "buffer.tpp"

#endif /* MARLIN_BUFFER_HPP */
