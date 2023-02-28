//
//  commandBuffer.cpp
//  Marlin
//
//  Created by Jonathan Graham on 2/24/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include "commandBuffer.hpp"

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

void CommandBuffer::reset()
{
    vkResetCommandBuffer( m_object, 0 );
}

CommandBufferRecordPtr CommandBuffer::scopedRecord( VkCommandBufferUsageFlags i_flags )
{
    return std::make_shared< CommandBufferRecord >( m_object, i_flags );
}

} // namespace marlin
