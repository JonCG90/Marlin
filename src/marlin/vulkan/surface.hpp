//
//  surface.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/24/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_SURFACE_HPP
#define MARLIN_SURFACE_HPP

#include "vkObject.hpp"

namespace marlin
{

class Surface : public VkObjectT< VkSurfaceKHR >
{
public:
    
    static Surface create( VkInstance i_instance, void* i_layer );
    
    Surface() = default;
    explicit Surface( VkSurfaceKHR i_surface );
};

} // namespace marlin

#endif /* MARLIN_SURFACE_HPP */
