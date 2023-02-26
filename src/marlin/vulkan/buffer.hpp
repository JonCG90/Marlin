//
//  buffer.hpp
//  Marlin
//
//  Created by Jonathan Graham on 2/21/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_BUFFER_HPP
#define MARLIN_BUFFER_HPP

#include "defs.hpp"
#include "vkObject.hpp"

namespace marlin
{

class Buffer : public VkObjectT< VkBuffer >
{
public:
    
    static BufferPtr create( DevicePtr i_device );
    
    Buffer( VkDevice i_device );
    
    Buffer() = default;
    ~Buffer() override;
    
    void destroy();
    
private:
    
    VkDevice m_device;
    VkDeviceMemory m_memory;
};

} // namespace marlin

#endif /* MARLIN_BUFFER_HPP */
