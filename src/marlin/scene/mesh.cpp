//
//  mesh.cpp
//  Marlin
//
//  Created by Jonathan Graham on 12/3/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/scene/mesh.hpp>

namespace marlin
{

Mesh::Mesh()
{
}

void Mesh::setVertices( const std::vector< Vec3f > &i_vertices )
{
    m_vertices = i_vertices;
}

const std::vector< Vec3f > & Mesh::getVertices() const
{
    return m_vertices;
}

void Mesh::setNormals( const std::vector< Vec3f > &i_normals )
{
    m_normals = i_normals;
}

const std::vector< Vec3f > & Mesh::getNormals() const
{
    return m_normals;
}

void Mesh::setColors( const std::vector< Vec3f > &i_colors )
{
    m_colors = i_colors;
}

const std::vector< Vec3f > & Mesh::getColors() const
{
    return m_colors;
}

void Mesh::setUVs( const std::vector< Vec2f > &i_uvs )
{
    m_uvs = i_uvs;
}

const std::vector< Vec2f > & Mesh::getUVs() const
{
    return m_uvs;
}

void Mesh::setIndices( const std::vector< uint32_t > &i_indices )
{
    m_indices = i_indices;
}

const std::vector< uint32_t > & Mesh::getIndices() const
{
    return m_indices;
}

} // namespace marlin
