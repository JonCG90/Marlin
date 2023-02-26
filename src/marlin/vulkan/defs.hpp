//
//  defs.hpp
//  Marlin
//
//  Created by Jonathan Graham on 11/1/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLINE_DEFS_HPP
#define MARLINE_DEFS_HPP

#include <iostream>
#include <vector>

namespace marlin
{

class Buffer;
using BufferPtr = std::shared_ptr< Buffer >;

class CommandBuffer;
using CommandBufferPtr = std::shared_ptr< CommandBuffer >;

class Device;
using DevicePtr = std::shared_ptr< Device >;

class PhysicalDevice;
using PhysicalDevicePtr = std::shared_ptr< PhysicalDevice >;
using PhysicalDevicePtrs = std::vector< PhysicalDevicePtr >;

class Surface;
using SurfacePtr = std::shared_ptr< Surface >;

class SwapChain;
using SwapChainPtr = std::shared_ptr< SwapChain >;

class QueueFamily;
using QueueFamilies = std::vector< QueueFamily >;

} // namespace marlin

#endif /* defs_hpp */
