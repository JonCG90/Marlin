//
//  descriptorAlloc.hpp
//  Marlin
//
//  Created by Jonathan Graham on 3/30/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_DESCRIPTORALLOC_HPP
#define MARLIN_DESCRIPTORALLOC_HPP

#include <marlin/vulkan/defs.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace marlin
{

using VkDescriptorPools = std::vector< VkDescriptorPool >;

class DescriptorAlloc
{
public:

    DescriptorAlloc( DevicePtr i_device );
    ~DescriptorAlloc();

    void getDescriptorSets( const std::vector< VkDescriptorSetLayout > i_layouts, std::vector< VkDescriptorSet > o_descriptorSets );
    
    void reset();
    void destroy();

private:
    
    VkDescriptorPool getCurrentPool();

    DevicePtr m_device;
    VkDescriptorPools m_pools;
    size_t m_currentIndex;

};

} // namespace marlin

#endif /* MARLIN_DESCRIPTORALLOC_HPP */
