//
//  descriptorCache.hpp
//  Marlin
//
//  Created by Jonathan Graham on 3/28/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_DESCRIPTORCACHE_HPP
#define MARLIN_DESCRIPTORCACHE_HPP

#include <marlin/vulkan/defs.hpp>

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace marlin
{

class DescriptorCache
{
public:
    
    DescriptorCache( DevicePtr i_device );
    const std::vector< VkDescriptorSetLayout > & getLayouts( const std::string &i_path );
    
private:
    
    DevicePtr m_device;
    
    std::unordered_map< std::string, std::vector< VkDescriptorSetLayout > > m_layouts;
};

} // namespace marlin

#endif /* MARLIN_DESCRIPTORCACHE_HPP */
