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
    
    void init( void* i_layer );
    void deinit();
    
    MlnInstance( MlnInstance const &i_instance ) = delete;
    void operator=( MlnInstance const &i_instance )  = delete;
    
private:
        
    VkInstance m_vkInstance;
    VkSurfaceKHR m_vkSurface;

    VkPhysicalDevice m_vkPhysicalDevice;
    VkDevice m_vkDevice;
    
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    
    VkSwapchainKHR m_swapChain;
    std::vector< VkImage > m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    
    bool m_enableValidation;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    
    void setupDebugMessenger();
    
private:
    
    void getValidationLayers( std::vector< const char* > &o_validationLayers );
    void getExtensions( std::vector< const char* > &o_extensions );
    void createLogicalDevice();
    void createSwapChain();

};

} // namespace marlin

#endif /* MARLIN_INSTANCE_HPP */
