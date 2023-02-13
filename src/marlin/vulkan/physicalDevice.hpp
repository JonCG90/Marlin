//
//  physicalDevice.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/19/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_PHYSICALDEVICE_HPP
#define MARLIN_PHYSICALDEVICE_HPP

#include "defs.hpp"
#include "vkObject.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace marlin
{

class QueueFamily
{
public:
    
    QueueFamily( VkPhysicalDevice i_physicalDevice, const VkQueueFamilyProperties &i_properties, uint32_t i_index );
    ~QueueFamily() = default;
    
    bool hasGraphics() const;
    bool hasCompute() const;
    bool isSurfaceSupported( const SurfacePtr &i_surface ) const;
    
    uint32_t getIndex() const;
    uint32_t count() const;
    
private:
    
    VkPhysicalDevice m_physicalDevice;
    VkQueueFamilyProperties m_properties;
    uint32_t m_index;
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector< VkSurfaceFormatKHR > formats;
    std::vector< VkPresentModeKHR > presentModes;
};

class PhysicalDevice : public VkObjectT< VkPhysicalDevice >
{
public:
    
    static PhysicalDevicePtrs getPhysicalDevices( VkInstance i_instance );
    
    PhysicalDevice() = default;
    explicit PhysicalDevice( VkPhysicalDevice i_device );

    VkPhysicalDeviceProperties getProperties() const;
    VkPhysicalDeviceFeatures getFeatures() const;
    VkPhysicalDeviceMemoryProperties getMemoryProperties() const;
    
    void getQueueFamilies( QueueFamilies &o_queueFamilies ) const;
    void getExtensions( std::vector< VkExtensionProperties > &extensions ) const;
    SwapChainSupportDetails getSwapChainSupportDetails( SurfacePtr i_surface ) const;
};

} // namespace marlin

#endif /* physicalDevice_hpp */
