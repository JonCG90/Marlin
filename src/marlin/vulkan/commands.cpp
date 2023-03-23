//
//  CommandFactory.cpp
//  Marlin
//
//  Created by Jonathan Graham on 3/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/commands.hpp>

#include <marlin/vulkan/commandBuffer.hpp>
#include <marlin/vulkan/pipeline.hpp>


namespace marlin
{

Command::Command( std::function< void ( CommandBufferPtr ) > i_recordFunc )
: m_recordFunc( i_recordFunc )
{
}

void Command::record( CommandBufferPtr i_commandBuffer )
{
    m_recordFunc( i_commandBuffer );
}

CommandPtr CommandFactory::beginRenderPass( VkRenderPass i_renderPass, VkFramebuffer i_frameBuffer, const VkExtent2D &i_extent )
{
    auto func = [ i_renderPass, i_frameBuffer, i_extent ]( CommandBufferPtr i_commandBuffer ) {
        
        VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        
        VkRenderPassBeginInfo renderPassInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = i_renderPass,
            .framebuffer = i_frameBuffer,
            .renderArea.offset = { 0, 0 },
            .renderArea.extent = i_extent,
            .clearValueCount = 1,
            .pClearValues = &clearColor,
        };
        
        vkCmdBeginRenderPass( i_commandBuffer->getObject(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
    };
    
    return std::make_unique< Command >( func );
}

CommandPtr CommandFactory::endRenderPass()
{
    auto func = []( CommandBufferPtr i_commandBuffer ) {
        vkCmdEndRenderPass( i_commandBuffer->getObject() );
    };
    
    return std::make_unique< Command >( func );
}

CommandPtr CommandFactory::bindPipeline( GraphicsPipelinePtr i_pipeline )
{
    auto func = [ i_pipeline ]( CommandBufferPtr i_commandBuffer ) {
        vkCmdBindPipeline( i_commandBuffer->getObject(), VK_PIPELINE_BIND_POINT_GRAPHICS, i_pipeline->getObject() );
    };
    
    return std::make_unique< Command >( func );
}

CommandPtr CommandFactory::setViewport( const Vec2f i_position, const Vec2f i_size )
{
    auto func = [ i_position, i_size ]( CommandBufferPtr i_commandBuffer ) {
        
        VkViewport viewport {
            .x = i_position.x,
            .y = i_position.y,
            .width = i_size.x,
            .height = i_size.y,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        vkCmdSetViewport( i_commandBuffer->getObject(), 0, 1, &viewport );
    };
    
    return std::make_unique< Command >( func );
}

CommandPtr CommandFactory::setScissor( const Vec2i i_offset, const Vec2u i_extent )
{
    auto func = [ i_offset, i_extent ]( CommandBufferPtr i_commandBuffer ) {
        
        VkRect2D scissor {
            .offset = { i_offset.x, i_offset.y },
            .extent = { i_extent.x, i_extent.y },
        };
        vkCmdSetScissor( i_commandBuffer->getObject(), 0, 1, &scissor );
    };
    
    return std::make_unique< Command >( func );
}

} // marlin
