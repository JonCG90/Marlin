//
//  device.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/31/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_DEVICE_HPP
#define MARLIN_DEVICE_HPP

#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/vkObject.hpp>

#include <map>

namespace marlin
{

enum QueueType : int
{
    QueueTypeCompute  = 0x1000,
    QueueTypeGraphics = 0x2000,
    QueueTypePresent  = 0x4000,
    QueueTypeTransfer = 0x8000,
};

using QueueCreateCounts = std::map< QueueType, uint32_t >;
using BufferCreateCounts = std::map< QueueType, uint32_t >;
using QueueToFamily = std::map< QueueType, QueueFamily >;
using QueueToCommandPool = std::map< QueueType, VkCommandPool >;
using QueueToCommandBuffers = std::map< QueueType, std::vector< CommandBufferPtr > >;

class Device : public VkObjectT< VkDevice >
{
public:
    
    static DevicePtr create( PhysicalDevicePtr i_device, const SurfacePtr, const QueueCreateCounts &i_queueCounts, const BufferCreateCounts &i_bufferCounts );
    
    Device() = default;
    Device( VkDevice i_device, const QueueToFamily &i_supportedQueues, const BufferCreateCounts &i_bufferCounts );
        
    ~Device() override;
    
    VkQueue getQueue( QueueType i_type, uint32_t i_index ) const;
    
    CommandBufferPtr getCommandBuffer( QueueType i_type, uint32_t i_index );

    void destroy();
    
private:

    VkCommandPool getCommandPool( QueueType i_type );

    QueueToFamily m_supportedQueues;
    BufferCreateCounts m_bufferCounts;
    
    QueueToCommandPool m_commandPools;
    QueueToCommandBuffers m_commandBuffers;
};

} // namespace marlin

#endif /* device_hpp */
