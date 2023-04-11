//
//  descriptorAlloc.cpp
//  Marlin
//
//  Created by Jonathan Graham on 3/30/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/descriptor/descriptorAlloc.hpp>

#include <marlin/vulkan/device.hpp>

namespace marlin
{

VkDescriptorPool allocPool( VkDevice i_device )
{
    static std::vector< VkDescriptorPoolSize > s_poolSizes {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 }
    };
    
    VkDescriptorPoolCreateInfo poolInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = static_cast< uint32_t >( s_poolSizes.size() ),
        .pPoolSizes = s_poolSizes.data(),
        .maxSets = 1000,
    };
    
    VkDescriptorPool pool;
    if ( vkCreateDescriptorPool( i_device, &poolInfo, nullptr, &pool ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to create descriptor pool." );
    }
    
    return pool;
}

VkResult allocDescriptorSet( VkDescriptorPool i_pool, VkDevice i_device, const std::vector< VkDescriptorSetLayout > i_layouts, std::vector< VkDescriptorSet > o_descriptorSets)
{
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = i_pool,
        .descriptorSetCount = static_cast< uint32_t >( i_layouts.size() ),
        .pSetLayouts = i_layouts.data(),
    };
    
    o_descriptorSets.resize( i_layouts.size() );
    return vkAllocateDescriptorSets( i_device, &allocInfo, o_descriptorSets.data() );
}

DescriptorAlloc::DescriptorAlloc( DevicePtr i_device )
: m_device( i_device )
, m_currentIndex( 0 )
{
}

DescriptorAlloc::~DescriptorAlloc()
{
    if ( m_pools.size() > 0 )
    {
        std::cerr << "Warning: Descriptor pool objects not released." << std::endl;
    }
}

void DescriptorAlloc::getDescriptorSets( const std::vector< VkDescriptorSetLayout > i_layouts, std::vector< VkDescriptorSet > o_descriptorSets )
{
    VkDescriptorPool pool = getCurrentPool();
    VkResult result = allocDescriptorSet( pool, m_device->getObject(), i_layouts, o_descriptorSets );
    if ( result == VK_SUCCESS )
    {
        return;
    }
    
    if ( result == VK_ERROR_FRAGMENTED_POOL || result == VK_ERROR_OUT_OF_POOL_MEMORY )
    {
        m_currentIndex++;
        pool = getCurrentPool();
    }
    
    if ( allocDescriptorSet( pool, m_device->getObject(), i_layouts, o_descriptorSets ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to allocate descriptor sets." );
    }
}

void DescriptorAlloc::reset()
{
    for ( VkDescriptorPool pool : m_pools )
    {
        vkResetDescriptorPool( m_device->getObject(), pool, 0 );
    }
    
    m_currentIndex = 0;
}

void DescriptorAlloc::destroy()
{
    for ( VkDescriptorPool pool : m_pools )
    {
        vkDestroyDescriptorPool( m_device->getObject(), pool, nullptr );
    }
}
VkDescriptorPool DescriptorAlloc::getCurrentPool()
{
    if ( m_currentIndex >= m_pools.size() )
    {
        VkDescriptorPool pool = allocPool( m_device->getObject() );
        m_pools.push_back( pool );
    }
    
    return m_pools[ m_currentIndex ];
}

} // namespace marlin
