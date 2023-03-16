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

class ShaderModule : public VkObjectT< VkShaderModule >
{
public:
    
    static ShaderModulePtr create( const std::string &i_path, DevicePtr i_device );
        
    ShaderModule() = default;
    ShaderModule( const std::vector< char > i_bytes, DevicePtr i_device );

    virtual ~ShaderModule();
    
    std::string getEntryPoint() const;
    VkShaderStageFlagBits getStage() const;
    void getDescriptorSetLayouts( std::vector< DescriptorSetLayoutData > &o_layouts ) const;
    
    void destroy();
    
protected:
    
    DevicePtr m_device;
    SpvReflectShaderModule m_spvModule;
};

} // namespace marlin

#endif /* MARLIN_SHADER_HPP */
