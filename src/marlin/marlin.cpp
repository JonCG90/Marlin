//
//  marlin.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include "marlin.hpp"

#include <marlin/vulkan/instance.hpp>

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

namespace marlin
{

void init()
{
    marlin::MlnInstance::getInstance().init();
}

void render()
{
    
}

void deinit()
{
    marlin::MlnInstance::getInstance().deinit();
}

}
