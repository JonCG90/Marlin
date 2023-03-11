//
//  surface.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/24/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_SURFACE_HPP
#define MARLIN_SURFACE_HPP

#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/vkObject.hpp>

#include <iostream>

namespace marlin
{

class Surface : public VkObjectT< VkSurfaceKHR >
{
public:
    
    static SurfacePtr create( VkInstance i_instance, void* i_layer );
    
    Surface() = default;
    Surface( VkSurfaceKHR i_surface, VkInstance i_instance );
    
    ~Surface() override;
    
    void destroy();
    
private:
    
    VkInstance m_instance;
};

} // namespace marlin

#endif /* MARLIN_SURFACE_HPP */
