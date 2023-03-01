//
//  buffer.hpp
//  Marlin
//
//  Created by Jonathan Graham on 2/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_BUFFER_HPP
#define MARLIN_BUFFER_HPP

#include "defs.hpp"
#include "vkObject.hpp"

namespace marlin
{

template < class T >
class BufferT : public VkObjectT< VkBuffer >
{
public:
    
    static std::shared_ptr< BufferT< T > > create( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, const std::vector< T > &i_data, VkBufferUsageFlagBits i_usage );
    
    BufferT( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice, const std::vector< T > &i_data, VkBufferUsageFlagBits i_usage );
    
    BufferT() = default;
    ~BufferT() override;
    
    void destroy();
    
private:
        
    DevicePtr m_device;
    VkDeviceMemory m_memory;
};

} // namespace marlin

#include "buffer.tpp"

#endif /* MARLIN_BUFFER_HPP */