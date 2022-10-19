//
//  vkObject.tpp
//  Marlin
//
//  Created by Jonathan Graham on 10/19/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include "vkObject.hpp"

#include <vulkan/vulkan.h>

namespace marlin
{

template < class T >
VkObjectT< T >::VkObjectT( const T &object )
: m_object( object )
{
}

template < class T >
VkObjectT< T >::VkObjectT()
: VkObjectT( VK_NULL_HANDLE )
{
}


template < class T >
T VkObjectT< T >::getObject() const
{
    return m_object;
}

} // namespace marlin
