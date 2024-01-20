//
//  renderStorage.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/01/24.
//  Copyright © 2024 Jonathan Graham. All rights reserved.
//

#include <marlin/scene/renderStorage.hpp>

namespace marlin
{

RenderStorage::RenderStorage( DevicePtr i_device, PhysicalDevicePtr i_physicalDevice )
: m_vertexPool( i_device, i_physicalDevice, PoolUsage::Vertex, 2048 * 3 )
, m_indexPool( i_device, i_physicalDevice, PoolUsage::Index, 2048 )
{
}

VertexPoolHandle RenderStorage::allocateVertexBuffer( uint32_t i_size )
{
    return m_vertexPool.allocate( i_size );
}

void RenderStorage::deallocateVertexBuffer( const VertexPoolHandle &i_handle )
{
    m_vertexPool.deallocate( i_handle );
}

IndexPoolHandle RenderStorage::allocateIndexBuffer( uint32_t i_size )
{
    return m_indexPool.allocate( i_size );
}

void RenderStorage::deallocateIndexBuffer( const IndexPoolHandle &i_handle )
{
    m_indexPool.deallocate( i_handle );
}

void RenderStorage::updateLOD( ObjectId i_id, uint32_t i_lodIndex, const Mesh &i_mesh )
{
    MeshLODs &meshLODs = m_meshStorage[ i_id ];
    MeshStorage &meshLOD = meshLODs.meshLODs[ i_lodIndex ];
    
    VertexPoolHandle &vertexHandle = meshLOD.vertexHandle;
    if ( vertexHandle.isValid() )
    {
        deallocateVertexBuffer( vertexHandle );
    }
    
    struct Vertex
    {
        Vec3 pos;
        Vec3 color;
    };
    
    // Temp transform vertices
    std::vector< Vertex > vertices;
    
    const std::vector< Vec3f > &meshVertices = i_mesh.getVertices();
    const std::vector< Vec3f > &meshColors = i_mesh.getColors();
    
    for ( size_t i = 0; i < meshVertices.size(); i++ )
    {
        Vec3f meshVertex = meshVertices[ i ];
        Vec3f meshColor = meshColors[ i ];
        
        Vertex vertex;
        vertex.pos = meshVertex;
        vertex.color = meshColor;
        
        vertices.push_back( vertex );
    }

    std::byte* bytes = reinterpret_cast< std::byte* >( vertices.data() );
    uint32_t vertexBufferSize = static_cast< uint32_t >( vertices.size() * sizeof( Vertex ) );
    vertexHandle = allocateVertexBuffer( vertexBufferSize );
    BufferTPtr< std::byte > vertexBuffer = vertexHandle.buffer;
    vertexBuffer->updateData( bytes, vertexHandle.allocation.offset, vertexBufferSize );
    
    meshLOD.vertexCount = static_cast< uint32_t>( vertices.size() );
    
    IndexPoolHandle &indexHandle = meshLOD.indexHandle;
    if ( indexHandle.isValid() )
    {
        deallocateIndexBuffer( indexHandle );
    }

    const std::vector< uint32_t > &indices = i_mesh.getIndices();
    uint32_t indexBufferSize = static_cast< uint32_t >( indices.size() );
        
    indexHandle = allocateIndexBuffer( indexBufferSize );
    BufferTPtr< uint32_t > indexBuffer = indexHandle.buffer;
    indexBuffer->updateData( indices.data(), indexHandle.allocation.offset, indexBufferSize );
    
    meshLOD.indexCount = static_cast< uint32_t>( indices.size() );
}

const MeshLODs* RenderStorage::getLODs( ObjectId i_id ) const
{
    auto itr = m_meshStorage.find( i_id );
    if ( itr != m_meshStorage.end() )
    {
        return &itr->second;
    }
    return nullptr;
}

std::vector< ObjectId > RenderStorage::getGeometryIds() const
{
    std::vector< ObjectId > objectIds;
    for ( const auto &pair : m_meshStorage )
    {
        objectIds.push_back( pair.first );
    }
    
    return objectIds;
}


} // namespace marlin
