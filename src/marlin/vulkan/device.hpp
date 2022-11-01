//
//  device.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/31/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_DEVICE_HPP
#define MARLIN_DEVICE_HPP

#include "defs.hpp"
#include "vkObject.hpp"

namespace marlin
{

class Device : public VkObjectT< VkDevice >
{
public:
    
    static DevicePtr create( PhysicalDevicePtr i_device, const QueueFamilies &i_families );
    
    Device() = default;
    explicit Device( VkDevice i_device );
        
    ~Device() override;
    
    VkQueue getQueue( const QueueFamily &i_family, uint32_t i_index ) const;
    
    void destroy();
};

} // namespace marlin

#endif /* device_hpp */
