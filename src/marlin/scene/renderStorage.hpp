//
//  renderStorage.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/01/24.
//  Copyright © 2024 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_RENDERSTORAGE_HPP
#define MARLIN_RENDERSTORAGE_HPP

#include <marlin/vulkan/vertexPool.hpp>

namespace marlin
{

class RenderStorage
{
public:

    RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice );
    ~RenderStorage() = default;
    
private:
    
    VertexPool m_vertexPool;
    
//    BufferTPtr< std::byte > m_vertexBuffer;
//    BufferTPtr< uint32_t > m_indexBuffer;
    
};


} // namespace marlin

#endif /* MARLIN_RENDERSTORAGE_HPP */
