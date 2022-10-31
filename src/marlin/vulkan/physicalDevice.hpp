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
class Surface;
class QueueFamily;

using QueueFamilies = std::vector< QueueFamily >;

class QueueFamily
{
public:
    
    QueueFamily( VkPhysicalDevice i_physicalDevice, const VkQueueFamilyProperties &i_properties, uint32_t i_index );
    ~QueueFamily() = default;
    
    bool hasGraphics() const;
    bool hasCompute() const;
    bool isSurfaceSupported( const Surface &i_surface ) const;
    
    uint32_t getIndex() const;
    uint32_t count() const;
    
private:
    
    VkPhysicalDevice m_physicalDevice;
    VkQueueFamilyProperties m_properties;
    uint32_t m_index;
};

using PhysicalDevices = std::vector< PhysicalDevice >;

class PhysicalDevice : public VkObjectT< VkPhysicalDevice >
{
public:
    
    static PhysicalDevices getPhysicalDevices( VkInstance i_instance );
    
    PhysicalDevice() = default;
    explicit PhysicalDevice( VkPhysicalDevice i_device );

    VkPhysicalDeviceProperties getProperties() const;
    VkPhysicalDeviceFeatures getFeatures() const;
    
    void getQueueFamilies( QueueFamilies &o_queueFamilies ) const;
    void getExtensions( std::vector< VkExtensionProperties > &extensions ) const;
};

} // namespace marlin

#endif /* physicalDevice_hpp */
