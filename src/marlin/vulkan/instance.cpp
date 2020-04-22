//
//  instance.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include "instance.hpp"

#include <iostream>
#include <vector>

namespace marlin
{

static ExtensionMap getAvailableExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
    
    Extensions extensions( extensionCount );
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
    
    ExtensionMap extensionMap;
    for ( const VkExtensionProperties &extension : extensions )
    {
        extensionMap[ extension.extensionName ] = extension;
    }
    
    return extensionMap;
}

MlnInstance & MlnInstance::getInstance()
{
    static MlnInstance instance;
    return instance;
}

void MlnInstance::init()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
        
    if ( vkCreateInstance( &createInfo, nullptr, &m_vkInstance ) != VK_SUCCESS ) {
        throw std::runtime_error( "Error: Failed to create Vulkan instance." );
    }
}

void MlnInstance::deinit()
{
    vkDestroyInstance( m_vkInstance, nullptr );
}

} // namespace marlin
