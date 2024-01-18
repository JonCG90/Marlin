//
//  mesh.hpp
//  Marlin
//
//  Created by Jonathan Graham on 12/3/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_MESH_HPP
#define MARLIN_MESH_HPP

#include <marlin/vulkan/../defs.hpp>

#include <vector>

namespace marlin
{

class Mesh
{
public:
        
    Mesh();
    ~Mesh() = default;

    void setVertices( const std::vector< Vec3f > &i_vertices );
    const std::vector< Vec3f > & getVertices() const;
    
    void setNormals( const std::vector< Vec3f > &i_normals );
    const std::vector< Vec3f > & getNormals() const;

    void setColors( const std::vector< Vec3f > &i_colors );
    const std::vector< Vec3f > & getColors() const;

    void setUVs( const std::vector< Vec2f > &i_uvs );
    const std::vector< Vec2f > & getUVs() const;

    void setIndices( const std::vector< uint32_t > &i_indices );
    const std::vector< uint32_t > & getIndices() const;
    
protected:
    
    std::vector< Vec3f > m_vertices;
    std::vector< Vec3f > m_normals;
    std::vector< Vec3f > m_colors;
    std::vector< Vec2f > m_uvs;
    std::vector< uint32_t > m_indices;
    
};


} // namespace marlin

#endif /* MARLIN_MESH_HPP */
