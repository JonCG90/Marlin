//
//  instance.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/instance.hpp>

#include <marlin/scene/renderStorage.hpp>

#include <marlin/vulkan/buffer.hpp>
#include <marlin/vulkan/commandBuffer.hpp>
#include <marlin/vulkan/commands.hpp>
#include <marlin/vulkan/descriptor/descriptorCache.hpp>
#include <marlin/vulkan/device.hpp>
#include <marlin/vulkan/physicalDevice.hpp>
#include <marlin/vulkan/surface.hpp>
#include <marlin/vulkan/swapChain.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma clang diagnostic pop
#define GLM_FORCE_RADIANS


#include <stb/stb_image.h>

#include <chrono>

#include <vulkan/vulkan_metal.h>

#include <fstream>
#include <set>
#include <sstream>

namespace marlin
{

// TODO FIX
const int MAX_FRAMES_IN_FLIGHT = 2;

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
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
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
    appInfo.apiVersion = VK_MAKE_VERSION( 1, 3, 0 );
    
    VkInstanceCreateInfo createInstanceInfo {};
    createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInstanceInfo.pApplicationInfo = &appInfo;
    createInstanceInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    
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

SurfacePtr Instance::getSurface()
{
    return m_surface;
}

PhysicalDevicePtr Instance::getPhysicalDevice()
{
    return m_physicalDevice;
}

DevicePtr Instance::getDevice()
{
    return m_device;
}
    
VkQueue Instance::getGraphicsQueue()
{
    return m_graphicsQueue;
}

VkQueue Instance::getPresentQueue()
{
    return m_presentQueue;
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
    else if ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
    {
        score += 50;
    }
    
    return score;
}

struct QueueFamilyIndices
{
    std::optional< QueueFamily > graphicsFamily;
    std::optional< QueueFamily > presentFamily;
    std::optional< QueueFamily > computeFamily;
    std::optional< QueueFamily > transferFamily;
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
        
        if ( family.hasTransfer() )
        {
            o_familyIndices.transferFamily = family;
        }
        
        if ( family.hasCompute() )
        {
            o_familyIndices.computeFamily = family;
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
    
    if ( !familyIndices.graphicsFamily.has_value() ||
         !familyIndices.presentFamily.has_value()  ||
         !familyIndices.transferFamily.has_value() ||
         !familyIndices.computeFamily.has_value() )
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
    
    m_renderStorage = new RenderStorage( m_device, m_physicalDevice );
    
    // Create the swap chain
    createSwapChain();
    createImageViews();
    
    createRenderPass();
    
    // Create our pipeline
    createDescriptorSetLayout();
    createGraphicsPipeline();
    
    createFramebuffers();
    
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    
    createSyncObjects();
}

void MlnInstance::deinit()
{
    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        vkDestroySemaphore( m_device->getObject(), m_imageAvailableSemaphores[ i ], nullptr );
        vkDestroySemaphore( m_device->getObject(), m_renderFinishedSemaphores[ i ], nullptr );
        vkDestroyFence( m_device->getObject(), m_inFlightFences[ i ], nullptr );
    }

    for ( auto framebuffer : m_swapChainFramebuffers )
    {
        vkDestroyFramebuffer( m_device->getObject(), framebuffer, nullptr );
    }
    
    vkDestroyDescriptorPool( m_device->getObject(), m_descriptorPool, nullptr );
    vkDestroyDescriptorSetLayout( m_device->getObject(), m_descriptorSetLayout, nullptr );
    m_pipeline->destroy();
    vkDestroyRenderPass( m_device->getObject(), m_renderPass, nullptr );
    
    for ( BufferTPtr< UniformBufferObject > buffer : m_uniformBuffers )
    {
        buffer->destroy();
    }
    m_uniformBuffers.clear();

    vkDestroyDescriptorSetLayout(m_device->getObject(), m_descriptorSetLayout, nullptr);

    for ( VkImageView imageView : m_swapChainImageViews )
    {
        vkDestroyImageView( m_device->getObject(), imageView, nullptr );
    }
    
    m_swapChain->destroy();
    
    delete m_renderStorage;
    
    m_device->destroy();

    if ( m_enableValidation )
    {
        destroyDebugUtilsMessengerEXT( m_vkInstance, m_debugMessenger, nullptr );
    }
    
    m_surface->destroy();
    vkDestroyInstance( m_vkInstance, nullptr );
}

void MlnInstance::drawFrame( ScenePtr i_scene )
{
    // Make sure the device entities are up to date
    i_scene->update();
    
    vkWaitForFences( m_device->getObject(), 1, &m_inFlightFences[ m_currentFrame ], VK_TRUE, UINT64_MAX );
    vkResetFences( m_device->getObject(), 1, &m_inFlightFences[ m_currentFrame ] );

    uint32_t imageIndex;
    m_swapChain->acquireImage( m_imageAvailableSemaphores[ m_currentFrame ], VK_NULL_HANDLE, imageIndex );
    
    updateUniformBuffer( 0 );
    
    CommandBufferPtr commandBuffer = m_device->getCommandBuffer( QueueTypeGraphics, m_currentFrame );
    commandBuffer->reset();

    recordCommandBuffer( commandBuffer, imageIndex );
    
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[ m_currentFrame ] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    submitInfo.commandBufferCount = 1;
    
    VkCommandBuffer vkCommandBuffer = commandBuffer->getObject();
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    
    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[ m_currentFrame ] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if ( vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, m_inFlightFences[ m_currentFrame ] ) != VK_SUCCESS )
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
    
    m_currentFrame = ( m_currentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
}

void MlnInstance::updateUniformBuffer( uint32_t currentImage )
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration< float, std::chrono::seconds::period >( currentTime - startTime ).count();
    
    VkExtent2D extend = m_swapChain->getExtent();
    
    UniformBufferObject ubo {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection = glm::perspective(glm::radians(45.0f), extend.width / (float) extend.height, 0.1f, 10.0f);
    ubo.projection[1][1] *= -1;
    
    memcpy( m_uniformBuffersMapped[ currentImage ], &ubo, sizeof( ubo ) );
}

RenderStorage & MlnInstance::getRenderStorage()
{
    return *m_renderStorage;
}

void MlnInstance::createLogicalDevice()
{    
    QueueCreateCounts queuesCounts {
        { QueueTypeGraphics, 1 },
        { QueueTypePresent,  1 },
        { QueueTypeTransfer, 1 },
    };
    QueueCreateCounts bufferCounts {
        { QueueTypeGraphics, MAX_FRAMES_IN_FLIGHT },
        { QueueTypePresent,  MAX_FRAMES_IN_FLIGHT },
        { QueueTypeTransfer, MAX_FRAMES_IN_FLIGHT },
    };
    
    m_device = Device::create( m_physicalDevice, m_surface, queuesCounts, bufferCounts );
    m_descriptorCache = std::make_unique< DescriptorCache >( m_device );
    
    // Get our device queues
    m_graphicsQueue = m_device->getQueue( QueueTypeGraphics, 0 );
    m_presentQueue = m_device->getQueue( QueueTypePresent, 0 );
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
}

void MlnInstance::createImageViews()
{
    std::vector< VkImage > images = m_swapChain->getImages();
    
    size_t imageCount = images.size();
    m_swapChainImageViews.resize( imageCount );

    for ( size_t i = 0; i < imageCount; i++ )
    {
        VkImage image = images[ i ];
        
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

void MlnInstance::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr, // Optional
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &uboLayoutBinding,
    };

    if ( vkCreateDescriptorSetLayout( m_device->getObject(), &layoutInfo, nullptr, &m_descriptorSetLayout ) != VK_SUCCESS)
    {
        throw std::runtime_error("Erro: Failed to create descriptor set layout.");
    }
}

void MlnInstance::createGraphicsPipeline()
{
    m_pipeline = GraphicsPipeline::create( m_device, m_renderPass, m_swapChain->getExtent(), m_descriptorCache );
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

uint32_t findMemoryType2( PhysicalDevicePtr i_device, uint32_t i_typeFilter, VkMemoryPropertyFlags i_properties )
{
    VkPhysicalDeviceMemoryProperties memProperties = i_device->getMemoryProperties();
    
    for ( uint32_t i = 0; i < memProperties.memoryTypeCount; i++ )
    {
        if ( ( i_typeFilter & ( 1 << i ) ) && ( memProperties.memoryTypes[i].propertyFlags & i_properties ) == i_properties )
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void MlnInstance::createUniformBuffers()
{
    UniformBufferObject ubo;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        
        BufferTPtr< UniformBufferObject > buffer = BufferT< UniformBufferObject >::create( m_device, m_physicalDevice, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, BufferMode::Local, ubo );
        
        m_uniformBuffersMapped.push_back( buffer->mapMemory() );
        m_uniformBuffers.push_back( buffer );
    }
}

void MlnInstance::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = static_cast< uint32_t >( MAX_FRAMES_IN_FLIGHT )
    };
    
    VkDescriptorPoolCreateInfo poolInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
        .maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
    };
    
    if ( vkCreateDescriptorPool( m_device->getObject(), &poolInfo, nullptr, &m_descriptorPool ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to create descriptor pool!" );
    }
}

void MlnInstance::createDescriptorSets()
{
    std::vector< VkDescriptorSetLayout > layouts( MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout );
    
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
        .pSetLayouts = layouts.data(),
    };
    
    m_descriptorSets.resize( MAX_FRAMES_IN_FLIGHT );
    if ( vkAllocateDescriptorSets( m_device->getObject(), &allocInfo, m_descriptorSets.data() ) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    
    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        VkDescriptorBufferInfo bufferInfo {
            .buffer = m_uniformBuffers[i]->getObject(),
            .offset = 0,
            .range = sizeof( UniformBufferObject ),
        };
        
        VkWriteDescriptorSet descriptorWrite {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &bufferInfo,
            .pImageInfo = nullptr, // Optional
            .pTexelBufferView = nullptr, // Optional
        };

        vkUpdateDescriptorSets( m_device->getObject(), 1, &descriptorWrite, 0, nullptr );

    }
}

void MlnInstance::recordCommandBuffer( CommandBufferPtr commandBuffer, uint32_t imageIndex )
{
    const VkExtent2D &extent = m_swapChain->getExtent();
    
    CommandPtr beginPass = CommandFactory::beginRenderPass( m_renderPass, m_swapChainFramebuffers[ imageIndex ], extent );
    CommandPtr bind = CommandFactory::bindPipeline( m_pipeline );
    CommandPtr viewport = CommandFactory::setViewport( Vec2f( 0.0 ), Vec2f( extent.width, extent.height ) );
    CommandPtr scissor = CommandFactory::setScissor( Vec2i( 0 ), Vec2u( extent.width, extent.height ) );
    CommandPtr endPass = CommandFactory::endRenderPass();
    
    std::vector< CommandPtr > drawCommands;
    
    std::vector< ObjectId > geometryIds = m_renderStorage->getGeometryIds();
    for ( ObjectId geometryId : geometryIds )
    {
        const MeshLODs* lod = m_renderStorage->getLODs( geometryId );
        if ( lod == nullptr )
        {
            continue;
        }
        
        for ( const auto &pair : lod->meshLODs )
        {
            const MeshStorage &lodStorage = pair.second;
            
            auto func = [ this, &lodStorage ]( VkCommandBuffer i_commandBuffer ) {
                
                VkBuffer vertexBuffers[] = { lodStorage.vertexHandle.buffer->getObject() };
                VkDeviceSize offsets[] = { lodStorage.vertexHandle.allocation.offset };
                vkCmdBindVertexBuffers( i_commandBuffer, 0, 1, vertexBuffers, offsets );
                vkCmdBindIndexBuffer( i_commandBuffer, lodStorage.indexHandle.buffer->getObject(), 0, VK_INDEX_TYPE_UINT32 );

                vkCmdBindDescriptorSets( i_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getLayout(), 0, 1, &m_descriptorSets[ 0 ], 0, nullptr );
                
                uint32_t count =  static_cast< uint32_t >( lodStorage.indexHandle.buffer->getCount() );
                vkCmdDrawIndexed( i_commandBuffer, count, 1, 0, 0, 0 );
            };
            
            drawCommands.emplace_back( CommandFactory::commandFunction( func ) );
        }
    }

    commandBuffer->addCommand( std::move( beginPass ) );
    commandBuffer->addCommand( std::move( bind ) );
    commandBuffer->addCommand( std::move( viewport ) );
    commandBuffer->addCommand( std::move( scissor ) );
    
    for ( CommandPtr &drawCommand : drawCommands )
    {
        commandBuffer->addCommand( std::move( drawCommand ) );
    }
    
    commandBuffer->addCommand( std::move( endPass ) );

    commandBuffer->record( 0 );
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
    for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        VkSemaphore imageAvailableSemaphore;
        s_createSemaphore( m_device->getObject(), imageAvailableSemaphore );
        m_imageAvailableSemaphores.push_back( imageAvailableSemaphore );

        VkSemaphore renderFinishedSemaphore;
        s_createSemaphore( m_device->getObject(), renderFinishedSemaphore );
        m_renderFinishedSemaphores.push_back( renderFinishedSemaphore );
        
        VkFence inFlightFence;
        s_createFence( m_device->getObject(), inFlightFence );
        m_inFlightFences.push_back( inFlightFence );
    }
}

} // namespace marlin
