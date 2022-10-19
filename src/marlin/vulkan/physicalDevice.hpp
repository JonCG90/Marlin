//
//  physicalDevice.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/19/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_PHYSICALDEVICE_HPP
#define MARLIN_PHYSICALDEVICE_HPP

#include "vkObject.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace marlin
{

class PhysicalDevice;

using PhysicalDevices = std::vector< PhysicalDevice >;

class PhysicalDevice : public VkObjectT< VkPhysicalDevice >
{
public:
    
    static PhysicalDevices getPhysicalDevices( VkInstance i_instance );
    
    PhysicalDevice() = default;
    explicit PhysicalDevice( VkPhysicalDevice i_device );

    void getQueueFamilyProperties( std::vector< VkQueueFamilyProperties > &properties );
    void getExtensions( std::vector< VkExtensionProperties > &extensions );
};

} // namespace marlin

#endif /* physicalDevice_hpp */
