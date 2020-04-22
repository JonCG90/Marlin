//
//  instance.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_INSTANCE_HPP
#define MARLIN_INSTANCE_HPP

#include <vulkan/vulkan.h>

#include <map>

namespace marlin
{

using Extensions = std::vector< VkExtensionProperties >;
using ExtensionMap = std::map< std::string, VkExtensionProperties >;

class MlnInstance
{
public:
    MlnInstance() = default;
    ~MlnInstance() = default;
    
    static MlnInstance & getInstance();
    
    void init();
    void deinit();
    
    MlnInstance( MlnInstance const &i_instance ) = delete;
    void operator=( MlnInstance const &i_instance )  = delete;
    
private:
        
    VkInstance m_vkInstance;
};

} // namespace marlin

#endif /* MARLIN_INSTANCE_HPP */
