//
//  marlin.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include "marlin.hpp"

#include <marlin/scene/scene.hpp>
#include <marlin/vulkan/instance.hpp>

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

namespace marlin
{

void init( void* i_layer )
{
    marlin::MlnInstance::getInstance().init( i_layer );
}

void render( ScenePtr i_scene )
{
    marlin::MlnInstance::getInstance().drawFrame( i_scene );
}

void deinit()
{
    marlin::MlnInstance::getInstance().deinit();
}

}
