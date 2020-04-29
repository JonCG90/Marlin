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

#include <vector>

namespace marlin
{

class MlnInstance
{
public:
    MlnInstance();
    ~MlnInstance() = default;
    
    static MlnInstance & getInstance();
    
    void init();
    void deinit();
    
    MlnInstance( MlnInstance const &i_instance ) = delete;
    void operator=( MlnInstance const &i_instance )  = delete;
    
private:
        
    VkInstance m_vkInstance;
    VkPhysicalDevice m_vkPhysicalDevice;
    
    bool m_enableValidation;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    
    void setupDebugMessenger();
    
private:
    
    void getValidationLayers( std::vector< const char* > &o_validationLayers );
    void getExtensions( std::vector< const char* > &o_extensions );

};

} // namespace marlin

#endif /* MARLIN_INSTANCE_HPP */
