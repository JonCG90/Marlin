//
//  marlin.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include "marlin.hpp"

namespace marlin
{

void MlnContext::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    VkResult result = vkCreateInstance( &createInfo, nullptr, &m_instance );
    
    std::cout << result << std::endl;
}

void init()
{
    
}

void render()
{
    
}

void deinit()
{
    
}

}
