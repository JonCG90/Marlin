//
//  instance.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include "instance.hpp"

#include <vulkan/vulkan_metal.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <map>
#include <set>

namespace marlin
{

#define VK_EXT_METAL_SURFACE_EXTENSION_NAME "VK_EXT_metal_surface"

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

static int getDevicScore( VkPhysicalDevice i_device )
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties( i_device, &deviceProperties );
    
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(i_device, &deviceFeatures);
    
    int score = 0;
    
    if ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
    {
        score += 100;
    }
    
    return score;
}

struct QueueFamilyIndices
{
    std::optional< uint32_t > graphicsFamily;
    std::optional< uint32_t > presentFamily;
};

static void getQueueFamilies( VkPhysicalDevice i_device, VkSurfaceKHR i_surface, QueueFamilyIndices &o_familyIndices )
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( i_device, &queueFamilyCount, nullptr );
    
    std::vector< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties( i_device, &queueFamilyCount, queueFamilies.data() );
    
    for ( uint32_t i = 0; i < queueFamilyCount; i++ )
    {
        const VkQueueFamilyProperties &queueFamily = queueFamilies[ i ];
        
        if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
        {
            o_familyIndices.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( i_device, i, i_surface, &presentSupport );
        
        if ( presentSupport )
        {
            o_familyIndices.presentFamily = i;
        }
    }
}

static const std::vector< const char* > s_deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static bool hasRequiredExtensions( VkPhysicalDevice i_device )
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties( i_device, nullptr, &extensionCount, nullptr );

    std::vector< VkExtensionProperties > availableExtensions( extensionCount );
    vkEnumerateDeviceExtensionProperties( i_device, nullptr, &extensionCount, availableExtensions.data() );
    
    std::set< std::string > requiredExtensions( s_deviceExtensions.begin(), s_deviceExtensions.end() );
    
    for ( const VkExtensionProperties &extension : availableExtensions )
    {
        requiredExtensions.erase( extension.extensionName );
    }
    
    return requiredExtensions.empty();
}

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector< VkSurfaceFormatKHR > formats;
    std::vector< VkPresentModeKHR > presentModes;
};

VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &i_availableFormats )
{
    for ( const VkSurfaceFormatKHR &surfaceFormat : i_availableFormats )
    {
        if ( surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return surfaceFormat;
        }
    }
    
    return i_availableFormats.front();
}

VkPresentModeKHR chooseSwapPresentMode( const std::vector< VkPresentModeKHR >& i_availablePresentModes )
{
    for ( const auto &presentMode : i_availablePresentModes )
    {
        if ( presentMode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR &i_capabilities )
{
    // Some window managers set the current extent to max
    if ( i_capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max() )
    {
        return i_capabilities.currentExtent;
    }
 
    static VkExtent2D s_defaultExtent = { 600, 800 };
    const VkExtent2D &minExtent = i_capabilities.minImageExtent;
    const VkExtent2D &maxExtent = i_capabilities.maxImageExtent;

    uint32_t width = std::clamp( s_defaultExtent.width, minExtent.width, maxExtent.width );
    uint32_t height = std::clamp( s_defaultExtent.height, minExtent.height, maxExtent.height );
    
    return { width, height };
}

SwapChainSupportDetails querySwapChainSupport( VkPhysicalDevice i_device, VkSurfaceKHR i_surface )
{
    SwapChainSupportDetails details;
    
    // Query capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( i_device, i_surface, &details.capabilities );
    
    // Query formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR( i_device, i_surface, &formatCount, nullptr );

    if ( formatCount != 0 )
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR( i_device, i_surface, &formatCount, details.formats.data() );
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR( i_device, i_surface, &presentModeCount, nullptr );

    if ( presentModeCount != 0 )
    {
        details.presentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( i_device, i_surface, &presentModeCount, details.presentModes.data() );
    }
    
    return details;
}

static bool isDeviceSuitable( VkPhysicalDevice i_device, VkSurfaceKHR i_surface )
{
    if ( !hasRequiredExtensions( i_device ) )
    {
        return false;
    }
    
    QueueFamilyIndices familyIndices;
    getQueueFamilies( i_device, i_surface, familyIndices );
    
    if ( !familyIndices.graphicsFamily.has_value() || !familyIndices.presentFamily.has_value() )
    {
        return false;
    }
    
    SwapChainSupportDetails swapChainDetails = querySwapChainSupport( i_device, i_surface );
    if ( swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty() )
    {
        return false;
    }
    
    return true;
}

static std::vector< VkPhysicalDevice > getSuitableDevices( VkInstance i_instance, VkSurfaceKHR i_surface )
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( i_instance, &deviceCount, nullptr );
    
    if ( deviceCount == 0 )
    {
        throw std::runtime_error( "Error: Unable to find GPUs with Vulkan support." );
    }
    
    std::vector< VkPhysicalDevice > devices( deviceCount );
    vkEnumeratePhysicalDevices( i_instance, &deviceCount, devices.data() );
    
    std::vector< VkPhysicalDevice > suitableDevices;
    suitableDevices.reserve( deviceCount );
    
    for ( const VkPhysicalDevice &device : devices )
    {
        if ( isDeviceSuitable( device, i_surface ) )
        {
            suitableDevices.push_back( device );
        }
    }
    
    return suitableDevices;
}

static VkPhysicalDevice pickPhysicalDevice( VkInstance i_instance, VkSurfaceKHR i_surface )
{
    std::vector< VkPhysicalDevice > devices = getSuitableDevices( i_instance, i_surface );
    
    std::multimap< int, VkPhysicalDevice > deviceScores;
    for ( const VkPhysicalDevice &device : devices )
    {
        int score = getDevicScore( device );
        if ( score > 0 )
        {
            deviceScores.insert( { score, device } );
        }
    }
    
    if ( deviceScores.empty() )
    {
        throw std::runtime_error( "Error: Unable to find suitable GPU." );
    }
        
    return deviceScores.rbegin()->second;
}

MlnInstance & MlnInstance::getInstance()
{
    static MlnInstance instance;
    return instance;
}

MlnInstance::MlnInstance()
    : m_enableValidation( false )
    , m_vkInstance( VK_NULL_HANDLE )
    , m_vkPhysicalDevice( VK_NULL_HANDLE )
    , m_vkDevice( VK_NULL_HANDLE )
    , m_debugMessenger( VK_NULL_HANDLE )
{
#ifdef DEBUG
    m_enableValidation = true;
#else
    m_enableValidation = false;
#endif
}

void MlnInstance::init( void* i_layer )
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
    
    VkMetalSurfaceCreateInfoEXT createSurfaceInfo {
        .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .pLayer = i_layer
    };

    if ( vkCreateMetalSurfaceEXT( m_vkInstance, &createSurfaceInfo, nullptr, &m_vkSurface )  != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to create Vulkan instance." );
    }
    
    // Choose our physical device
    m_vkPhysicalDevice = pickPhysicalDevice( m_vkInstance, m_vkSurface );
    
    // Create logical device
    createLogicalDevice();
    
    // Create the swap chain
    createSwapChain();
    createImageViews();
    
    createGraphicsPipeline();
}

void MlnInstance::deinit()
{
    vkDestroyPipelineLayout( m_vkDevice, m_pipelineLayout, nullptr );
    
    for ( VkImageView imageView : m_swapChainImageViews )
    {
        vkDestroyImageView( m_vkDevice, imageView, nullptr );
    }
    
    vkDestroySwapchainKHR( m_vkDevice, m_swapChain, nullptr );
    vkDestroyDevice( m_vkDevice, nullptr );
    
    if ( m_enableValidation )
    {
        destroyDebugUtilsMessengerEXT( m_vkInstance, m_debugMessenger, nullptr );
    }
    
    vkDestroySurfaceKHR( m_vkInstance, m_vkSurface, nullptr);
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
    std::vector< const char* > extensions = {
        VK_EXT_METAL_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

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

void MlnInstance::createLogicalDevice()
{
    QueueFamilyIndices indices;
    getQueueFamilies( m_vkPhysicalDevice, m_vkSurface, indices );

    std::set< uint32_t > queueFamilies {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };
    
    float queuePriority = 1.0f;
    std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
    
    for ( uint32_t queueFamily : queueFamilies )
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        
        queueCreateInfos.push_back( queueCreateInfo );
    }
    
    VkPhysicalDeviceFeatures deviceFeatures {};

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast< uint32_t >( queueCreateInfos.size() );
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast< uint32_t >( s_deviceExtensions.size() );
    createInfo.ppEnabledExtensionNames = s_deviceExtensions.data();
    
    if ( vkCreateDevice( m_vkPhysicalDevice, &createInfo, nullptr, &m_vkDevice ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create logical device." );
    }
    
    // Get our device queues
    vkGetDeviceQueue( m_vkDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue );
    vkGetDeviceQueue( m_vkDevice, indices.presentFamily.value(), 0, &m_presentQueue );
}

void MlnInstance::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport( m_vkPhysicalDevice, m_vkSurface );

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapChainSupport.formats );
    VkPresentModeKHR presentMode = chooseSwapPresentMode( swapChainSupport.presentModes );
    VkExtent2D extent = chooseSwapExtent( swapChainSupport.capabilities );
    
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    
    uint32_t maxImageCount = swapChainSupport.capabilities.maxImageCount;
    if ( maxImageCount > 0 && imageCount > maxImageCount )
    {
        // Clamp just in case
        imageCount = maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = m_vkSurface;
    
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndices indices;
    getQueueFamilies( m_vkPhysicalDevice, m_vkSurface, indices );
    
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if ( indices.graphicsFamily != indices.presentFamily )
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
        swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if ( vkCreateSwapchainKHR( m_vkDevice, &swapChainCreateInfo, nullptr, &m_swapChain ) != VK_SUCCESS )
    {
        throw std::runtime_error("Failed to create swap chain!");
    }
    
    // Ge the swap chain images
    vkGetSwapchainImagesKHR( m_vkDevice, m_swapChain, &imageCount, nullptr );
    m_swapChainImages.resize( imageCount );
    vkGetSwapchainImagesKHR( m_vkDevice, m_swapChain, &imageCount, m_swapChainImages.data() );
    
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

void MlnInstance::createImageViews()
{
    size_t imageCount = m_swapChainImages.size();
    m_swapChainImageViews.resize( imageCount );

    for ( size_t i = 0; i < imageCount; i++ )
    {
        VkImage image = m_swapChainImages[ i ];
        
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_swapChainImageFormat;
        
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        
        if ( vkCreateImageView( m_vkDevice, &imageViewCreateInfo, nullptr, &m_swapChainImageViews[ i ] ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to create image views!" );
        }
    }
}

static void readFile( const std::string &i_filename, std::vector< char > &o_bytes )
{
    std::ifstream file( i_filename, std::ios::ate | std::ios::binary );

    if ( !file.is_open() )
    {
        throw std::runtime_error( "Failed to open file!" );
    }
    
    const size_t fileSize = static_cast< size_t >( file.tellg() );
    
    file.seekg( 0 );
    
    o_bytes.resize( fileSize );
    file.read( o_bytes.data(), fileSize );
    
    file.close();
}

VkShaderModule createShaderModule( const std::vector< char > &i_code, VkDevice i_device )
{
    VkShaderModuleCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = i_code.size(),
        .pCode = reinterpret_cast< const uint32_t* >( i_code.data() )
    };
    
    VkShaderModule shaderModule;
    if ( vkCreateShaderModule( i_device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
    {
        throw std::runtime_error("Failed to create shader module!");
    }
    
    return shaderModule;
}

void MlnInstance::createGraphicsPipeline()
{
    std::vector< char > vertBytes;
    std::vector< char > fragBytes;

    readFile( "/Users/jongraham/Projects/Marlin/src/marlin/shaders/vert.spv", vertBytes );
    readFile( "/Users/jongraham/Projects/Marlin/src/marlin/shaders/frag.spv", fragBytes );
    
    VkShaderModule vertShaderModule = createShaderModule( vertBytes, m_vkDevice );
    VkShaderModule fragShaderModule = createShaderModule( fragBytes, m_vkDevice );
    
    VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };
    
    // Vertex format
    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    
    // Topology
    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Viewport
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast< float >( m_swapChainExtent.width );
    viewport.height = static_cast< float >( m_swapChainExtent.height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    // Scissor
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapChainExtent;
    
    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    
    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    
    // Blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[ 0 ] = 0.0f; // Optional
    colorBlending.blendConstants[ 1 ] = 0.0f; // Optional
    colorBlending.blendConstants[ 2 ] = 0.0f; // Optional
    colorBlending.blendConstants[ 3 ] = 0.0f; // Optional
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout( m_vkDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create pipeline layout!" );
    }
    
    vkDestroyShaderModule( m_vkDevice, vertShaderModule, nullptr );
    vkDestroyShaderModule( m_vkDevice, fragShaderModule, nullptr );
}

} // namespace marlin
