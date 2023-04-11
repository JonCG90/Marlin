//
//  descriptorCache.cpp
//  Marlin
//
//  Created by Jonathan Graham on 3/28/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/descriptor/descriptorCache.hpp>

#include <marlin/vulkan/device.hpp>
#include <marlin/vulkan/shader.hpp>

namespace marlin
{

DescriptorCache::DescriptorCache( DevicePtr i_device )
: m_device( i_device )
{
}

const std::vector< VkDescriptorSetLayout > & DescriptorCache::getLayouts( const std::string &i_path )
{
    const auto it = m_layouts.find( i_path );
    if ( it != m_layouts.end() )
    {
        return it->second;
    }
    
    ShaderStage stage( i_path );
    
    std::vector< DescriptorSetLayoutData > layoutsData;
    stage.getDescriptorSetLayouts( layoutsData );

    std::vector< VkDescriptorSetLayout > layouts( layoutsData.size() );
    for ( size_t i = 0; i < layoutsData.size(); i++ )
    {
        const DescriptorSetLayoutData &layoutData = layoutsData[ i ];
        VkDescriptorSetLayoutCreateInfo layoutInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast< uint32_t >( layoutData.bindings.size() ),
            .pBindings = layoutData.bindings.data(),
        };

        if ( vkCreateDescriptorSetLayout( m_device->getObject(), &layoutInfo, nullptr, &layouts[ i ] ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Erro: Failed to create descriptor set layout." );
        }
    }
    
    return m_layouts.emplace( i_path, layouts ).first->second;    
}

} // namespace marlin
