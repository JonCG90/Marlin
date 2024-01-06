//
//  renderStorage.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/01/24.
//  Copyright © 2024 Jonathan Graham. All rights reserved.
//

#include <marlin/scene/renderStorage.hpp>

namespace marlin
{

RenderStorage::RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice )
: m_vertexPool( i_device, i_physicalDevice )
{
}


} // namespace marlin
