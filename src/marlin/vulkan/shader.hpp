//
//  shader.hpp
//  Marlin
//
//  Created by Jonathan Graham on 3/16/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_SHADER_HPP
#define MARLIN_SHADER_HPP

#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/vkObject.hpp>

#include <SPIRV-Reflect/spirv_reflect.h>

namespace marlin
{

struct DescriptorSetLayoutData
{
    uint32_t number;
    std::vector< VkDescriptorSetLayoutBinding > bindings;
};

class ShaderStage
{
public:
    
    
    ShaderStage( const std::string &i_path );

    ~ShaderStage();
    
    std::string getEntryPoint() const;
    VkShaderStageFlagBits getStage() const;
    void getDescriptorSetLayouts( std::vector< DescriptorSetLayoutData > &o_layouts ) const;
    const std::vector< char > & getBytes() const;
    
protected:
    
    SpvReflectShaderModule m_spvModule;
    std::vector< char > m_bytes;
};

} // namespace marlin

#endif /* MARLIN_SHADER_HPP */
