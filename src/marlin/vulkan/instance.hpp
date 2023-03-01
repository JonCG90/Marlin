//
//  instance.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/22/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_INSTANCE_HPP
#define MARLIN_INSTANCE_HPP

#include "defs.hpp"
#include "vkObject.hpp"
#include "../defs.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace marlin
{

struct Vertex
{
    Vec2 pos;
    Vec3 color;
    
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription {
            .binding = 0,
            .stride = sizeof( Vertex ),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        return bindingDescription;
    }
    
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};
        
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

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

    BufferTPtr< Vertex > m_vertexBuffer;
    BufferTPtr< uint32_t > m_indexBuffer;

    SwapChainPtr m_swapChain;
    std::vector< VkImageView > m_swapChainImageViews;
    
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
    void copyBuffer( VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size );
    void createBuffer( VkDeviceSize i_size, VkBufferUsageFlags i_usage, VkMemoryPropertyFlags i_properties, VkBuffer &o_buffer, VkDeviceMemory &o_bufferMemory );
    void createVertexBuffer();
    void createIndexBuffer();
    void recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex );
    void createSyncObjects();
};

} // namespace marlin

#endif /* MARLIN_INSTANCE_HPP */
