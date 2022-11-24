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
#include <sstream>

namespace marlin
{

#define VK_EXT_METAL_SURFACE_EXTENSION_NAME "VK_EXT_metal_surface"

// Callback for debug output on validation layers
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT i_severity, VkDebugUtilsMessageTypeFlagsEXT i_type, const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData, void* i_userData )
{
    if ( i_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
    {
        std::cerr << "Validation layer: " << i_callbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

static void populateDebugInfo( VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo )
{
    debugCreateInfo = VkDebugUtilsMessengerCreateInfoEXT {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };
}

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

void getExtensions( bool i_enableValidation, std::vector< const char* > &o_extensions )
{
    std::vector< const char* > extensions = {
        VK_EXT_METAL_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    if ( i_enableValidation )
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
            std::stringstream ss;
            ss << "Error: Unable to find layer '" << validationLayer << "'" << std::endl;
            throw std::runtime_error( ss.str() );
        }
    }
}

void getValidationLayers( bool i_enableValidation, std::vector< const char* > &o_validationLayers )
{
    std::vector< const char* > validationLayers;

    if ( i_enableValidation )
    {
        validationLayers.push_back( "VK_LAYER_KHRONOS_validation" );
    }
    
    o_validationLayers.clear();
    getSupportedValidationLayers( validationLayers, o_validationLayers );
}

Instance Instance::create( bool i_enableValidation )
{
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Marlin Render";
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInstanceInfo {};
    createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInstanceInfo.pApplicationInfo = &appInfo;
    
    std::vector< const char* > extensions;
    getExtensions( i_enableValidation, extensions );
    createInstanceInfo.enabledExtensionCount = static_cast< uint32_t >( extensions.size() );
    createInstanceInfo.ppEnabledExtensionNames = extensions.data();
    
    std::vector< const char* > validationLayers;
    getValidationLayers( i_enableValidation, validationLayers );
    createInstanceInfo.enabledLayerCount = static_cast< uint32_t >( validationLayers.size() );
    createInstanceInfo.ppEnabledLayerNames = validationLayers.data();
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    if ( i_enableValidation )
    {
        populateDebugInfo( debugCreateInfo );
        createInstanceInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInstanceInfo.enabledLayerCount = 0;
    }
        
    VkInstance vkInstance;
    if ( vkCreateInstance( &createInstanceInfo, nullptr, &vkInstance ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to create Vulkan instance." );
    }
    
    return Instance( vkInstance );
}

Instance::Instance( VkInstance i_instance )
: VkObjectT< VkInstance >( i_instance )
{
}

//**
// Old
//**

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

static int getDeviceScore( const PhysicalDevicePtr &i_device )
{
    VkPhysicalDeviceProperties deviceProperties = i_device->getProperties();
    
    int score = 0;
    
    if ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
    {
        score += 100;
    }
    
    return score;
}

struct QueueFamilyIndices
{
    std::optional< QueueFamily > graphicsFamily;
    std::optional< QueueFamily > presentFamily;
};

static void getQueueFamilies( PhysicalDevicePtr i_device, SurfacePtr i_surface, QueueFamilyIndices &o_familyIndices )
{
    QueueFamilies queueFamilies;
    i_device->getQueueFamilies( queueFamilies );
    
    for ( QueueFamily family : queueFamilies )
    {
        if ( family.hasGraphics() )
        {
            o_familyIndices.graphicsFamily = family;
        }
        
        if ( family.isSurfaceSupported( i_surface ) )
        {
            o_familyIndices.presentFamily = family;
        }
    }
}

static const std::vector< const char* > s_deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static bool hasRequiredExtensions( PhysicalDevicePtr i_device )
{
    std::vector< VkExtensionProperties > extensions;
    i_device->getExtensions( extensions );
        
    std::set< std::string > requiredExtensions( s_deviceExtensions.begin(), s_deviceExtensions.end() );
    
    for ( const VkExtensionProperties &extension : extensions )
    {
        requiredExtensions.erase( extension.extensionName );
    }
    
    return requiredExtensions.empty();
}

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

static bool isDeviceSuitable( PhysicalDevicePtr i_device, SurfacePtr i_surface )
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
    
    SwapChainSupportDetails swapChainDetails = i_device->getSwapChainSupportDetails( i_surface );
    if ( swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty() )
    {
        return false;
    }
    
    return true;
}

static PhysicalDevicePtrs getSuitableDevices( VkInstance i_instance, SurfacePtr i_surface )
{
    PhysicalDevicePtrs physicalDevices = PhysicalDevice::getPhysicalDevices( i_instance );

    if ( physicalDevices.empty() )
    {
        throw std::runtime_error( "Error: Unable to find GPUs with Vulkan support." );
    }

    PhysicalDevicePtrs suitableDevices;
    
    for ( const PhysicalDevicePtr &device : physicalDevices )
    {
        if ( isDeviceSuitable( device, i_surface ) )
        {
            suitableDevices.push_back( device );
        }
    }
    
    return suitableDevices;
}

static PhysicalDevicePtr pickPhysicalDevice( VkInstance i_instance, SurfacePtr i_surface )
{
    PhysicalDevicePtrs devices = getSuitableDevices( i_instance, i_surface );
    
    std::multimap< int, PhysicalDevicePtr > deviceScores;
    for ( const PhysicalDevicePtr &device : devices )
    {
        int score = getDeviceScore( device );
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
    Instance instance = Instance::create( true );
    m_vkInstance = instance.getObject();
    
    if ( m_enableValidation )
    {
        setupDebugging( m_vkInstance, m_debugMessenger );
    }
    
    m_surface = Surface::create( m_vkInstance, i_layer );
    
    // Choose our physical device
    m_physicalDevice = pickPhysicalDevice( m_vkInstance, m_surface );
    
    // Create logical device
    createLogicalDevice();
    
    // Create the swap chain
    createSwapChain();
    createImageViews();
    
    createRenderPass();
    
    // Create our pipeline
    createGraphicsPipeline();
    
    createFramebuffers();
    
    createCommandPool();
    createCommandBuffer();
    
    createSyncObjects();
}

void MlnInstance::deinit()
{
    vkDestroySemaphore( m_device->getObject(), m_imageAvailableSemaphore, nullptr );
    vkDestroySemaphore( m_device->getObject(), m_renderFinishedSemaphore, nullptr );
    vkDestroyFence( m_device->getObject(), m_inFlightFence, nullptr );
    
    vkDestroyCommandPool( m_device->getObject(), m_commandPool, nullptr );
    
    for ( auto framebuffer : m_swapChainFramebuffers )
    {
        vkDestroyFramebuffer( m_device->getObject(), framebuffer, nullptr );
    }
    
    vkDestroyPipeline( m_device->getObject(), m_graphicsPipeline, nullptr );
    vkDestroyPipelineLayout( m_device->getObject(), m_pipelineLayout, nullptr );
    vkDestroyRenderPass( m_device->getObject(), m_renderPass, nullptr );

    for ( VkImageView imageView : m_swapChainImageViews )
    {
        vkDestroyImageView( m_device->getObject(), imageView, nullptr );
    }
    
    m_swapChain->destroy();
    m_device->destroy();

    if ( m_enableValidation )
    {
        destroyDebugUtilsMessengerEXT( m_vkInstance, m_debugMessenger, nullptr );
    }
    
    m_surface->destroy();
    vkDestroyInstance( m_vkInstance, nullptr );
}

void MlnInstance::drawFrame()
{
    vkWaitForFences( m_device->getObject(), 1, &m_inFlightFence, VK_TRUE, UINT64_MAX );
    vkResetFences( m_device->getObject(), 1, &m_inFlightFence );

    uint32_t imageIndex;
    vkAcquireNextImageKHR( m_device->getObject(), m_swapChain->getObject(), UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex );
    
    vkResetCommandBuffer( m_commandBuffer, 0 );
    
    recordCommandBuffer( m_commandBuffer, imageIndex );
    
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    
    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if ( vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, m_inFlightFence ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Error: Failed to submit draw command buffer!" );
    }
    
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapChains[] = { m_swapChain->getObject() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR( m_presentQueue, &presentInfo );
}

void MlnInstance::createLogicalDevice()
{
    QueueFamilyIndices families;
    getQueueFamilies( m_physicalDevice, m_surface, families );
    
    m_device = Device::create( m_physicalDevice, { families.graphicsFamily.value(), families.presentFamily.value() } );
    
    // Get our device queues
    m_graphicsQueue = m_device->getQueue( families.graphicsFamily.value(), 0 );
    m_presentQueue = m_device->getQueue( families.presentFamily.value(), 0 );
}

void MlnInstance::createSwapChain()
{
        SwapChainSupportDetails swapChainSupport = m_physicalDevice->getSwapChainSupportDetails( m_surface );
    
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
    
        QueueFamilyIndices indices;
        getQueueFamilies( m_physicalDevice, m_surface, indices );
        
        uint32_t graphicIndex = indices.graphicsFamily.value().getIndex();
        uint32_t presentIndex = indices.presentFamily.value().getIndex();
    
        m_swapChain = SwapChain::create( m_device,
                                         m_surface,
                                         imageCount,
                                         extent,
                                         surfaceFormat.format,
                                         surfaceFormat.colorSpace,
                                         graphicIndex,
                                         presentIndex,
                                         presentMode,
                                         swapChainSupport.capabilities.currentTransform );

    m_swapChainImages = m_swapChain->getImages();
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
        imageViewCreateInfo.format = m_swapChain->getFormat();
        
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        
        if ( vkCreateImageView( m_device->getObject(), &imageViewCreateInfo, nullptr, &m_swapChainImageViews[ i ] ) != VK_SUCCESS )
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

void MlnInstance::createRenderPass()
{
    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = m_swapChain->getFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
        
    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    
    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    if ( vkCreateRenderPass( m_device->getObject(), &renderPassInfo, nullptr, &m_renderPass ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void MlnInstance::createGraphicsPipeline()
{
    std::vector< char > vertBytes;
    std::vector< char > fragBytes;

    readFile( "/Users/jongraham/Projects/Marlin/src/marlin/shaders/vert.spv", vertBytes );
    readFile( "/Users/jongraham/Projects/Marlin/src/marlin/shaders/frag.spv", fragBytes );
    
    VkShaderModule vertShaderModule = createShaderModule( vertBytes, m_device->getObject() );
    VkShaderModule fragShaderModule = createShaderModule( fragBytes, m_device->getObject() );
    
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
    viewport.width = static_cast< float >( m_swapChain->getExtent().width );
    viewport.height = static_cast< float >( m_swapChain->getExtent().height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    // Scissor
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapChain->getExtent();
    
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
    
    if ( vkCreatePipelineLayout( m_device->getObject(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create pipeline layout!" );
    }
    
    // Create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
    
    if ( vkCreateGraphicsPipelines( m_device->getObject(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to create graphics pipeline!" );
    }
    
    vkDestroyShaderModule( m_device->getObject(), vertShaderModule, nullptr );
    vkDestroyShaderModule( m_device->getObject(), fragShaderModule, nullptr );
}

void MlnInstance::createFramebuffers()
{
    const size_t numFramebuffers = m_swapChainImageViews.size();
    m_swapChainFramebuffers.resize( numFramebuffers );

    for (size_t i = 0; i < numFramebuffers; i++)
    {
        VkImageView attachments[] = {
            m_swapChainImageViews[ i ]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_swapChain->getExtent().width;
        framebufferInfo.height = m_swapChain->getExtent().height;
        framebufferInfo.layers = 1;

        if ( vkCreateFramebuffer( m_device->getObject(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[ i ] ) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void MlnInstance::createCommandPool()
{
    QueueFamilyIndices familyIndices;
    getQueueFamilies( m_physicalDevice, m_surface, familyIndices );

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = familyIndices.graphicsFamily.value().getIndex();
    
    if ( vkCreateCommandPool( m_device->getObject(), &poolInfo, nullptr, &m_commandPool ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to create command pool!" );
    }
}

void MlnInstance::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    
    if ( vkAllocateCommandBuffers( m_device->getObject(), &allocInfo, &m_commandBuffer ) != VK_SUCCESS ) {
        throw std::runtime_error( "failed to allocate command buffers!" );
    }
}

void MlnInstance::recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex )
{
    VkCommandBufferBeginInfo beginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };
    
    if ( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to begin recording command buffer!" );
    }
    
    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

    VkRenderPassBeginInfo renderPassInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_swapChainFramebuffers[ imageIndex ],
        .renderArea.offset = { 0, 0 },
        .renderArea.extent = m_swapChain->getExtent(),
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };
    
    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
    {
        vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline );

        VkViewport viewport {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast< float >( m_swapChain->getExtent().width ),
            .height = static_cast< float >( m_swapChain->getExtent().height ),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

        VkRect2D scissor {
            .offset = { 0, 0 },
            .extent = m_swapChain->getExtent(),
        };

        vkCmdSetScissor( commandBuffer, 0, 1, &scissor );
        
        vkCmdDraw( commandBuffer, 3, 1, 0, 0 );
    }
    vkCmdEndRenderPass( commandBuffer );
    
    if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

static void s_createSemaphore( const VkDevice &i_device, VkSemaphore &io_semaphor )
{
    VkSemaphoreCreateInfo semaphoreInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    
    if ( vkCreateSemaphore( i_device, &semaphoreInfo, nullptr, &io_semaphor ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create semaphores!" );
    }
}

static void s_createFence( const VkDevice &i_device, VkFence &io_fence )
{
    VkFenceCreateInfo fenceInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    
    if ( vkCreateFence( i_device, &fenceInfo, nullptr, &io_fence ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create fence!" );
    }
}

void MlnInstance::createSyncObjects()
{
    s_createSemaphore( m_device->getObject(), m_imageAvailableSemaphore );
    s_createSemaphore( m_device->getObject(), m_renderFinishedSemaphore );
    s_createFence( m_device->getObject(), m_inFlightFence );
}

} // namespace marlin
