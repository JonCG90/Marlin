//
//  instance.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include "instance.hpp"

#include <iostream>
#include <set>

namespace marlin
{

static std::set< std::string > getAvailableInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
    
    std::vector< VkExtensionProperties > extensions( extensionCount );
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
    
    std::set< std::string > extensionNames;
    for ( const VkExtensionProperties &extension : extensions )
    {
        extensionNames.insert( extension.extensionName );
    }
    
    return extensionNames;
}

static bool hasRequiredInstanceExtension( const char* i_extension )
{
    static std::set< std::string > extensions = getAvailableInstanceExtensions();
    
    if ( !extensions.count( i_extension ) )
    {
        std::cerr << "Warning: Extension '" << i_extension << "' not found." << std::endl;
        return false;
    }
    
    return true;
}

static void getSupportedValidationLayers( const std::vector< const char* > &i_validationLayers, std::vector< const char* > &o_validationLayers )
{    
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

    std::vector< VkLayerProperties > availableLayers( layerCount );
    vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );
    
    o_validationLayers.clear();
    
    for ( const char* validationLayer : i_validationLayers )
    {
        const auto it = std::find_if( availableLayers.begin(), availableLayers.end(), [&]( const VkLayerProperties &layer ) {
            return std::strcmp( layer.layerName, validationLayer ) == 0;
        });
        
        if ( it != availableLayers.end() )
        {
            o_validationLayers.push_back( validationLayer );
        }
        else
        {
            std::cerr << "Error: Unable to find layer '" << validationLayer << "'" << std::endl;
        }
    }
}

// Callback for debug output on validation layers
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT i_severity, VkDebugUtilsMessageTypeFlagsEXT i_type, const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData, void* i_userData )
{
    if ( i_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
    {
        std::cerr << "Validation layer: " << i_callbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT( VkInstance i_instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger ) {
    
    // This is an extension function so isn't automatically loaded
    const auto func = reinterpret_cast< PFN_vkCreateDebugUtilsMessengerEXT >( vkGetInstanceProcAddr( i_instance, "vkCreateDebugUtilsMessengerEXT" ) );
    if ( func != nullptr )
    {
        return func( i_instance, pCreateInfo, pAllocator, pDebugMessenger );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT( VkInstance i_instance, VkDebugUtilsMessengerEXT i_debugMessenger, const VkAllocationCallbacks* i_pAllocator )
{
    // This is an extension function so isn't automatically loaded
    const auto func = reinterpret_cast< PFN_vkDestroyDebugUtilsMessengerEXT >( vkGetInstanceProcAddr( i_instance, "vkDestroyDebugUtilsMessengerEXT" ) );
    if ( func != nullptr )
    {
        func( i_instance, i_debugMessenger, i_pAllocator );
    }
}

static void populateDebugInfo( VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo )
{
    debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData = nullptr;
}

static void setupDebugging( VkInstance i_instance, VkDebugUtilsMessengerEXT &i_debugMessenger )
{
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    populateDebugInfo( debugCreateInfo );
    
    VkResult r = createDebugUtilsMessengerEXT( i_instance, &debugCreateInfo, nullptr, &i_debugMessenger );
    if ( r != VK_SUCCESS )
    {
        std::cerr << "Error: Failed to set up debug messenger." << std::endl;
    }
}

MlnInstance & MlnInstance::getInstance()
{
    static MlnInstance instance;
    return instance;
}

MlnInstance::MlnInstance()
    : m_enableValidation( false )
{
#ifdef DEBUG
    m_enableValidation = true;
#else
    m_enableValidation = false;
#endif
}

void MlnInstance::init()
{
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInstanceInfo {};
    createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInstanceInfo.pApplicationInfo = &appInfo;
    
    std::vector< const char* > extensions;
    getExtensions( extensions );
    createInstanceInfo.enabledExtensionCount = static_cast< uint32_t >( extensions.size() );
    createInstanceInfo.ppEnabledExtensionNames = extensions.data();
    
    std::vector< const char* > validationLayers;
    getValidationLayers( validationLayers );
    createInstanceInfo.enabledLayerCount = static_cast< uint32_t >( validationLayers.size() );
    createInstanceInfo.ppEnabledLayerNames = validationLayers.data();
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    if ( m_enableValidation )
    {
        populateDebugInfo( debugCreateInfo );
        createInstanceInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInstanceInfo.enabledLayerCount = 0;
    }
        
    if ( vkCreateInstance( &createInstanceInfo, nullptr, &m_vkInstance ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to create Vulkan instance." );
    }
    
    if ( m_enableValidation )
    {
        setupDebugging( m_vkInstance, m_debugMessenger );
    }
}

void MlnInstance::deinit()
{
    if ( m_enableValidation )
    {
        destroyDebugUtilsMessengerEXT( m_vkInstance, m_debugMessenger, nullptr );
    }
    
    vkDestroyInstance( m_vkInstance, nullptr );
}

void MlnInstance::getValidationLayers( std::vector< const char* > &o_validationLayers )
{
    std::vector< const char* > validationLayers;

    if ( m_enableValidation )
    {
        validationLayers.push_back( "VK_LAYER_KHRONOS_validation" );
    }
    
    o_validationLayers.clear();
    getSupportedValidationLayers( validationLayers, o_validationLayers );
}

void MlnInstance::getExtensions( std::vector< const char* > &o_extensions )
{
    std::vector< const char* > extensions;

    if ( m_enableValidation )
    {
        extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }
    
    for ( const char* extension : extensions )
    {
        if ( hasRequiredInstanceExtension( extension ) )
        {
            o_extensions.push_back( extension );
        }
    }
}

} // namespace marlin
