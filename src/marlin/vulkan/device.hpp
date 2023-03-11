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
using QueueToFamily = std::map< QueueType, QueueFamily >;
using QueueToCommandPool = std::map< QueueType, VkCommandPool >;
using QueueToCommandBuffer = std::map< QueueType, CommandBufferPtr >;

class Device : public VkObjectT< VkDevice >
{
public:
    
    static DevicePtr create( PhysicalDevicePtr i_device, const SurfacePtr, const QueueCreateCounts &queueCounts );
    
    Device() = default;
    explicit Device( VkDevice i_device, const QueueToFamily &i_supportedQueues );
        
    ~Device() override;
    
    VkQueue getQueue( QueueType i_type, uint32_t i_index ) const;
    CommandBufferPtr getCommandBuffer( QueueType i_type );

    void destroy();
    
private:

    VkCommandPool getCommandPool( QueueType i_type );

    QueueToFamily m_supportedQueues;
    QueueToCommandPool m_commandPools;
    QueueToCommandBuffer m_commandBuffers;
};

} // namespace marlin

#endif /* device_hpp */
