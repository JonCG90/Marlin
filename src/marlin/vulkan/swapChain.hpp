//
//  swapChain.hpp
//  Marlin
//
//  Created by Jonathan Graham on 11/23/22.
//  Copyright © 2022 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_SWAPCHAIN_HPP
#define MARLIN_SWAPCHAIN_HPP

#include "defs.hpp"
#include "vkObject.hpp"

namespace marlin
{

class SwapChain : public VkObjectT< VkSwapchainKHR >
{
public:
    
    static SwapChainPtr create( DevicePtr i_device,
                                SurfacePtr i_surface,
                                uint32_t i_imageCount,
                                const VkExtent2D &i_extent,
                                VkFormat i_format,
                                VkColorSpaceKHR i_colorSpace,
                                uint32_t i_graphicIndex,
                                uint32_t i_presentIndex,
                                VkPresentModeKHR i_presentMode,
                                VkSurfaceTransformFlagBitsKHR i_preTransform );
    
    SwapChain( VkDevice i_device,
               VkSurfaceKHR i_surface,
               uint32_t i_imageCount,
               const VkExtent2D &i_extent,
               VkFormat i_format,
               VkColorSpaceKHR i_colorSpace,
               uint32_t i_graphicIndex,
               uint32_t i_presentIndex,
               VkPresentModeKHR i_presentMode,
               VkSurfaceTransformFlagBitsKHR i_preTransform );
    
    SwapChain() = default;
    ~SwapChain() override;
    
    std::vector< VkImage > getImages();
    
    VkFormat getFormat() const;
    const VkExtent2D & getExtent() const;
    
    void destroy();
    
private:
    
    VkDevice m_device;
    uint32_t m_imageCount;
    VkFormat m_format;
    VkExtent2D m_extent;
};

} // namespace marlin

#endif /* MARLIN_SWAPCHAIN_HPP */
