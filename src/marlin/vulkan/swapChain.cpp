//
//  swapChain.cpp
//  Marlin
//
//  Created by Jonathan Graham on 11/23/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/swapChain.hpp>

#include <marlin/vulkan/device.hpp>
#include <marlin/vulkan/surface.hpp>

#include <vulkan/vulkan.h>

#include <iostream>

namespace marlin
{

SwapChainPtr SwapChain::create( DevicePtr i_device,
                                SurfacePtr i_surface,
                                uint32_t i_imageCount,
                                const VkExtent2D &i_extent,
                                VkFormat i_format,
                                VkColorSpaceKHR i_colorSpace,
                                uint32_t i_graphicIndex,
                                uint32_t i_presentIndex,
                                VkPresentModeKHR i_presentMode,
                                VkSurfaceTransformFlagBitsKHR i_preTransform )
{                
    return std::make_shared< SwapChain >( i_device->getObject(),
                                          i_surface->getObject(),
                                          i_imageCount,
                                          i_extent,
                                          i_format,
                                          i_colorSpace,
                                          i_graphicIndex,
                                          i_presentIndex,
                                          i_presentMode,
                                          i_preTransform );
}

SwapChain::SwapChain( VkDevice i_device,
                      VkSurfaceKHR i_surface,
                      uint32_t i_imageCount,
                      const VkExtent2D &i_extent,
                      VkFormat i_format,
                      VkColorSpaceKHR i_colorSpace,
                      uint32_t i_graphicIndex,
                      uint32_t i_presentIndex,
                      VkPresentModeKHR i_presentMode,
                      VkSurfaceTransformFlagBitsKHR i_preTransform )
: VkObjectT< VkSwapchainKHR >( VK_NULL_HANDLE )
, m_device( i_device )
, m_imageCount( i_imageCount )
, m_format( i_format )
, m_extent( i_extent )
{
    VkSwapchainCreateInfoKHR swapChainCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = i_surface,
        .minImageCount = i_imageCount,
        .imageFormat = i_format,
        .imageColorSpace = i_colorSpace,
        .imageExtent = i_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    };
    
    uint32_t queueFamilyIndices[] = { i_graphicIndex, i_presentIndex };

    if ( i_graphicIndex != i_presentIndex )
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
    
    swapChainCreateInfo.preTransform = i_preTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = i_presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if ( vkCreateSwapchainKHR( i_device, &swapChainCreateInfo, nullptr, &m_object ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create swap chain." );
    }
}

SwapChain::~SwapChain()
{
    if ( m_object != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Surface object not released." << std::endl;
    }
}

std::vector< VkImage > SwapChain::getImages()
{
    std::vector< VkImage > images;
    
    // Get images
    vkGetSwapchainImagesKHR( m_device, m_object, &m_imageCount, nullptr );
    images.resize( m_imageCount );

    vkGetSwapchainImagesKHR( m_device, m_object, &m_imageCount, images.data() );

    return images;
}

VkFormat SwapChain::getFormat() const
{
    return m_format;
}

const VkExtent2D & SwapChain::getExtent() const
{
    return m_extent;
}

void SwapChain::acquireImage( VkSemaphore i_semaphore, VkFence i_fence, uint32_t &o_imageIndex )
{
    vkAcquireNextImageKHR( m_device, m_object, UINT64_MAX, i_semaphore, VK_NULL_HANDLE, &o_imageIndex );
}

void SwapChain::destroy()
{
    vkDestroySwapchainKHR( m_device, m_object, nullptr );
    m_object = VK_NULL_HANDLE;
}

} // namespace marlin
