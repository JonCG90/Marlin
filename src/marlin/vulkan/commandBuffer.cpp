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

CommandBuffer::CommandBuffer( VkCommandBuffer i_commandBuffer )
: VkObjectT< VkCommandBuffer >( i_commandBuffer )
{
}

} // namespace marlin
