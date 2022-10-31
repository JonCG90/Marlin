//
//  vkObject.hpp
//  Marlin
//
//  Created by Jonathan Graham on 10/19/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_VKOBJECT
#define MARLIN_VKOBJECT

namespace marlin
{

template < class T >
class VkObjectT
{
public:
    
    explicit VkObjectT( const T &object );
    
    VkObjectT();
    virtual ~VkObjectT() = default;
    
    bool isValid() const;
    T getObject() const;
    
protected:
    
    T m_object;
};

} // namespace marlin

#include "vkObject.tpp"

#endif /* MARLIN_VKOBJECT */
