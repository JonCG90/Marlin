//
//  commandBuffer.hpp
//  Marlin
//
//  Created by Jonathan Graham on 2/24/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_COMMANDBUFFER_HPP
#define MARLIN_COMMANDBUFFER_HPP

#include "defs.hpp"
#include "vkObject.hpp"

namespace marlin
{

class CommandBuffer : public VkObjectT< VkCommandBuffer >
{
public:
    
    explicit CommandBuffer( VkCommandBuffer i_commandBuffer );
    
private:
    
};

} // namespace marlin

#endif /* MARLIN_COMMANDBUFFER_HPP */
