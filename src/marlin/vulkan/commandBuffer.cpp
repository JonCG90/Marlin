//
//  commandBuffer.cpp
//  Marlin
//
//  Created by Jonathan Graham on 2/24/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/commandBuffer.hpp>

namespace marlin
{

CommandBufferRecord::CommandBufferRecord( VkCommandBuffer i_commandBuffer, VkCommandBufferUsageFlags i_flags )
: m_commandBuffer( i_commandBuffer )
{
    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = i_flags
    };
    
    vkBeginCommandBuffer( i_commandBuffer, &beginInfo );
}

CommandBufferRecord::~CommandBufferRecord()
{
    vkEndCommandBuffer( m_commandBuffer );
}

CommandBuffer::CommandBuffer( VkCommandBuffer i_commandBuffer )
: VkObjectT< VkCommandBuffer >( i_commandBuffer )
{
}
void CommandBuffer::addCommand( CommandPtr i_command )
{
    m_commands.emplace_back( std::move( i_command ) );
}

void CommandBuffer::reset()
{
    vkResetCommandBuffer( m_object, 0 );
    m_commands.clear();
}

void CommandBuffer::record( VkCommandBufferUsageFlags i_flags )
{
    CommandBufferRecordPtr record = scopedRecord( i_flags );
    
    for ( CommandPtr &command : m_commands )
    {
        command->record( getObject() );
    }
}

CommandBufferRecordPtr CommandBuffer::scopedRecord( VkCommandBufferUsageFlags i_flags )
{
    return std::make_shared< CommandBufferRecord >( m_object, i_flags );
}

} // namespace marlin
