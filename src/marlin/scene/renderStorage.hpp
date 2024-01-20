//
//  renderStorage.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/01/24.
//  Copyright © 2024 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_RENDERSTORAGE_HPP
#define MARLIN_RENDERSTORAGE_HPP

#include <marlin/scene/mesh.hpp>
#include <marlin/scene/scene.hpp>
#include <marlin/vulkan/bufferPool.hpp>

#include <unordered_map>

namespace marlin
{

using VertexPoolHandle = BufferPoolHandleT< std::byte >;
using IndexPoolHandle = BufferPoolHandleT< uint32_t >;

struct MeshStorage
{
    VertexPoolHandle vertexHandle;
    IndexPoolHandle indexHandle;
    uint32_t vertexCount;
    uint32_t indexCount;
};

struct MeshLODs
{
    std::map< uint8_t, MeshStorage > meshLODs;
};

class RenderStorage
{
public:

    RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice );
    ~RenderStorage() = default;
    
    VertexPoolHandle allocateVertexBuffer( uint32_t i_size );
    void deallocateVertexBuffer( const VertexPoolHandle &i_handle );
    
    IndexPoolHandle allocateIndexBuffer( uint32_t i_size );
    void deallocateIndexBuffer( const IndexPoolHandle &i_handle );
    
    void updateLOD( ObjectId i_id, uint32_t i_lodIndex, const Mesh &i_mesh );
    const MeshLODs* getLODs( ObjectId i_id ) const;
    
    std::vector< ObjectId > getGeometryIds() const;
    
    BufferTPtr< VkDrawIndexedIndirectCommand > getIndirectBuffer() const;

private:
    
    BufferPoolT< std::byte > m_vertexPool;
    BufferPoolT< uint32_t > m_indexPool;
    
    BufferTPtr< VkDrawIndexedIndirectCommand > m_indirectBuffer;
    
    std::unordered_map< ObjectId, MeshLODs > m_meshStorage;
};


} // namespace marlin

#endif /* MARLIN_RENDERSTORAGE_HPP */
