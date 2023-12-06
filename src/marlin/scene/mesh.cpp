//
//  mesh.cpp
//  Marlin
//
//  Created by Jonathan Graham on 12/3/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include "mesh.hpp"

namespace marlin
{

Mesh::Mesh()
{
}

void Mesh::setVertices( const std::vector< Vec3f > &i_vertices )
{
    m_vertices = i_vertices;
}

void Mesh::setNormals( const std::vector< Vec3f > &i_normals )
{
    m_normals = i_normals;
}

void Mesh::setColors( const std::vector< Vec3f > &i_colors )
{
    m_colors = i_colors;
}

void Mesh::setUVs( const std::vector< Vec2f > &i_uvs )
{
    m_uvs = i_uvs;
}

void Mesh::setIndices( const std::vector< uint32_t > &i_indices )
{
    m_indices = i_indices;
}

} // namespace marlin
