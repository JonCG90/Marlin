//
//  pipeline.hpp
//  Marlin
//
//  Created by Jonathan Graham on 3/15/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_PIPELINE_HPP
#define MARLIN_PIPELINE_HPP

#include <marlin/vulkan/../defs.hpp>
#include <marlin/vulkan/defs.hpp>
#include <marlin/vulkan/vkObject.hpp>

#include <array>

namespace marlin
{

struct UniformBufferObject {
    Mat4f model;
    Mat4f view;
    Mat4f projection;
};

struct Vertex
{
    Vec3 pos;
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
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class Pipeline : public VkObjectT< VkPipeline >
{
public:
        
    Pipeline() = default;
    virtual ~Pipeline();
    Pipeline( VkPipeline i_pipeline, VkPipelineLayout i_layout, DevicePtr i_device );
        
    VkPipelineLayout getLayout() const;
    
protected:
    
    VkPipelineLayout m_layout;
    DevicePtr m_device;
};

class GraphicsPipeline : public Pipeline
{
public:
    
    static GraphicsPipelinePtr create( DevicePtr i_device, VkRenderPass i_renderPass, const VkExtent2D &i_extent, DescriptorCachePtr &io_descriptorCache );
    
    GraphicsPipeline() = default;
    GraphicsPipeline( VkPipeline i_pipeline, VkPipelineLayout i_layout, DevicePtr i_device );
    ~GraphicsPipeline() override = default;
    
    void destroy();

private:
    
};

} // namespace marlin

#endif /* MARLIN_PIPELINE_HPP */
