//
//  commandBuffer.hpp
//  Marlin
//
//  Created by Jonathan Graham on 2/24/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_COMMANDBUFFER_HPP
#define MARLIN_COMMANDBUFFER_HPP

#include <marlin/vulkan/commands.hpp>
#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/vkObject.hpp>

namespace marlin
{

class CommandBufferRecord
{
public:
    
    CommandBufferRecord() = delete;
    explicit CommandBufferRecord( VkCommandBuffer i_commandBuffer, VkCommandBufferUsageFlags i_flags );
    ~CommandBufferRecord();
    
private:
    
    VkCommandBuffer m_commandBuffer;
};

class CommandBuffer : public VkObjectT< VkCommandBuffer >
{
public:
    
    explicit CommandBuffer( VkCommandBuffer i_commandBuffer );
    void addCommand( CommandPtr i_command );
    void record( VkCommandBufferUsageFlags i_flags );
    void reset();
    CommandBufferRecordPtr scopedRecord( VkCommandBufferUsageFlags i_flags );
    
private:
    
    CommandPtrs m_commands;
};

} // namespace marlin

#endif /* MARLIN_COMMANDBUFFER_HPP */
