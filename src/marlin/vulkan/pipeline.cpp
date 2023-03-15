//
//  pipeline.cpp
//  Marlin
//
//  Created by Jonathan Graham on 3/15/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/pipeline.hpp>

#include <marlin/vulkan/device.hpp>


#include <fstream>

namespace marlin
{

void readFile( const std::string &i_filename, std::vector< char > &o_bytes )
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

Pipeline::Pipeline( VkPipeline i_pipeline, VkPipelineLayout i_layout, DevicePtr i_device )
: VkObjectT< VkPipeline >( i_pipeline )
, m_layout( i_layout )
, m_device( i_device )
{
}

Pipeline::~Pipeline()
{
    if ( m_object != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Pipeline object not released." << std::endl;
    }
    
    if ( m_layout != VK_NULL_HANDLE )
    {
        std::cerr << "Warning: Pipeline layout object not released." << std::endl;
    }
}

VkPipelineLayout Pipeline::getLayout() const
{
    return m_layout;
}

GraphicsPipelinePtr GraphicsPipeline::create( DevicePtr i_device, VkRenderPass i_renderPass, const VkExtent2D &i_extent, VkDescriptorSetLayout i_descriptorSetLayout )
{
    std::vector< char > vertBytes;
    std::vector< char > fragBytes;

    readFile( "/Users/jongraham/Projects/Marlin/src/marlin/shaders/vert.spv", vertBytes );
    readFile( "/Users/jongraham/Projects/Marlin/src/marlin/shaders/frag.spv", fragBytes );
    
    VkShaderModule vertShaderModule = createShaderModule( vertBytes, i_device->getObject() );
    VkShaderModule fragShaderModule = createShaderModule( fragBytes, i_device->getObject() );
    
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
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast< uint32_t >( attributeDescriptions.size() );
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    // Topology
    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Viewport
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast< float >( i_extent.width );
    viewport.height = static_cast< float >( i_extent.height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    // Scissor
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = i_extent;
    
    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    std::vector< VkDynamicState > dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast< uint32_t >( dynamicStates.size() ),
        .pDynamicStates = dynamicStates.data()
    };
    
    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &i_descriptorSetLayout; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
    
    VkPipelineLayout pipelineLayout;
    if ( vkCreatePipelineLayout( i_device->getObject(), &pipelineLayoutInfo, nullptr, &pipelineLayout ) != VK_SUCCESS )
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
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = i_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
    
    VkPipeline pipeline;
    if ( vkCreateGraphicsPipelines( i_device->getObject(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to create graphics pipeline!" );
    }
    
    vkDestroyShaderModule( i_device->getObject(), vertShaderModule, nullptr );
    vkDestroyShaderModule( i_device->getObject(), fragShaderModule, nullptr );
    
    return std::make_shared< GraphicsPipeline >( pipeline, pipelineLayout, i_device );
}

GraphicsPipeline::GraphicsPipeline( VkPipeline i_pipeline, VkPipelineLayout i_layout, DevicePtr i_device )
: Pipeline( i_pipeline, i_layout, i_device )
{
}

void GraphicsPipeline::destroy()
{
    vkDestroyPipeline( m_device->getObject(), m_object, nullptr );
    vkDestroyPipelineLayout( m_device->getObject(), m_layout, nullptr );
}

} // namespace marlin
