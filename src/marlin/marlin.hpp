//
//  marlin.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_HPP
#define MARLIN_HPP

#include <marlin/defs.hpp>

namespace marlin
{

class Scene;

void init( void* i_layer );

void render( const Scene &i_scene );

void deinit();

} // namespace marlin

#endif /* marlin_hpp */
