//
//  instance.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_INSTANCE_HPP
#define MARLIN_INSTANCE_HPP

#include "device.hpp"
#include "physicalDevice.hpp"
#include "surface.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace marlin
{

class Instance : public VkObjectT< VkInstance >
{
public:
    
    static Instance create( bool i_enableValidation );
    
    Instance() = default;
    explicit Instance( VkInstance i_instance );
};

class MlnInstance
{
public:
    MlnInstance();
    ~MlnInstance() = default;
    
    static MlnInstance & getInstance();
    
    void init( void* i_layer );
    void deinit();
    
    void drawFrame();
    
    MlnInstance( MlnInstance const &i_instance ) = delete;
    void operator=( MlnInstance const &i_instance )  = delete;
    
private:
        
    VkInstance m_vkInstance;
    SurfacePtr m_surface;

    PhysicalDevicePtr m_physicalDevice;
    DevicePtr m_device;
        
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkCommandPool m_commandPool;
    VkCommandBuffer m_commandBuffer;

    VkSwapchainKHR m_swapChain;
    std::vector< VkImage > m_swapChainImages;
    std::vector< VkImageView > m_swapChainImageViews;
    
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    std::vector< VkFramebuffer > m_swapChainFramebuffers;
    
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_inFlightFence;

    bool m_enableValidation;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    
    void setupDebugMessenger();
    
private:
    
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffer();
    void recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex );
    void createSyncObjects();
};

} // namespace marlin

#endif /* MARLIN_INSTANCE_HPP */
