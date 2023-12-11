//
//  instance.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_INSTANCE_HPP
#define MARLIN_INSTANCE_HPP

#include <marlin/scene/scene.hpp>
#include <marlin/vulkan/../defs.hpp>
#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/pipeline.hpp>
#include <marlin/vulkan/vkObject.hpp>

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace marlin
{

class Instance : public VkObjectT< VkInstance >
{
public:
    
    static Instance create( bool i_enableValidation );
    
    Instance() = default;
    explicit Instance( VkInstance i_instance );
    
    SurfacePtr getSurface();

    PhysicalDevicePtr getPhysicalDevice();
    DevicePtr getDevice();
        
    VkQueue getGraphicsQueue();
    VkQueue getPresentQueue();
    VkCommandPool getCommandPool();
    
private:
    
    SurfacePtr m_surface;

    PhysicalDevicePtr m_physicalDevice;
    DevicePtr m_device;
        
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
};

class MlnInstance
{
public:
    MlnInstance();
    ~MlnInstance() = default;
    
    static MlnInstance & getInstance();
    
    void init( void* i_layer );
    void deinit();
    
    void drawFrame( const Scene &i_scene );
    void updateUniformBuffer(uint32_t currentImage);

    MlnInstance( MlnInstance const &i_instance ) = delete;
    void operator=( MlnInstance const &i_instance )  = delete;
    
private:
        
    VkInstance m_vkInstance;
    SurfacePtr m_surface;

    PhysicalDevicePtr m_physicalDevice;
    DevicePtr m_device;
        
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    
    DescriptorCachePtr m_descriptorCache;

    BufferTPtr< std::byte > m_vertexBuffer;
    BufferTPtr< uint32_t > m_indexBuffer;
    
    std::vector< BufferTPtr< UniformBufferObject > > m_uniformBuffers;
    std::vector<void*> m_uniformBuffersMapped;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    
    SwapChainPtr m_swapChain;
    std::vector< VkImageView > m_swapChainImageViews;
    
    VkRenderPass m_renderPass;
    VkDescriptorSetLayout m_descriptorSetLayout;
    GraphicsPipelinePtr m_pipeline;
    std::vector< VkFramebuffer > m_swapChainFramebuffers;
    
    std::vector< VkSemaphore > m_imageAvailableSemaphores;
    std::vector< VkSemaphore > m_renderFinishedSemaphores;
    std::vector< VkFence > m_inFlightFences;

    bool m_enableValidation;
    uint32_t m_currentFrame = 0;

    VkDebugUtilsMessengerEXT m_debugMessenger;
    
    void setupDebugMessenger();
    
private:
    
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    
    void createGraphicsPipeline();
    void createFramebuffers();
    void copyBuffer( VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size );
    void createBuffer( VkDeviceSize i_size, VkBufferUsageFlags i_usage, VkMemoryPropertyFlags i_properties, VkBuffer &o_buffer, VkDeviceMemory &o_bufferMemory );
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    
    void recordCommandBuffer( CommandBufferPtr commandBuffer, uint32_t imageIndex );
    void createSyncObjects();
};

} // namespace marlin

#endif /* MARLIN_INSTANCE_HPP */
