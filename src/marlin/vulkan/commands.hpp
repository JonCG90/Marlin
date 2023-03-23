//
//  CommandFactory.hpp
//  Marlin
//
//  Created by Jonathan Graham on 3/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_COMMANDS_HPP
#define MARLIN_COMMANDS_HPP

#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/../defs.hpp>

#include <vulkan/vulkan.h>

namespace marlin
{

class Command
{
public:
        
    Command() = delete;
    Command( std::function< void ( CommandBufferPtr ) > i_recordFunc );

    virtual ~Command() = default;
    
    void record( CommandBufferPtr i_commandBuffer );
private:
    
    std::function< void ( CommandBufferPtr ) > m_recordFunc;
};

class CommandFactory
{
public:
    
    static CommandPtr beginRenderPass( VkRenderPass i_renderPass, VkFramebuffer i_frameBuffer, const VkExtent2D &i_extent );
    static CommandPtr endRenderPass();
    static CommandPtr bindPipeline( GraphicsPipelinePtr i_pipeline );
    static CommandPtr setViewport( const Vec2f i_position, const Vec2f i_size );
    static CommandPtr setScissor( const Vec2i i_offset, const Vec2u i_extent );

};

} // namespace marlin

#endif /* MARLIN_COMMANDS_HPP */
