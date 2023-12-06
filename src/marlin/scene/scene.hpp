//
//  scene.hpp
//  Marlin
//
//  Created by Jonathan Graham on 12/5/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_SCENE_HPP
#define MARLIN_SCENE_HPP

#include <marlin/vulkan/../defs.hpp>
#include <marlin/scene/mesh.hpp>

#include <array>
#include <unordered_map>

namespace marlin
{

static const uint32_t s_maxLODs = 5;

class SceneObject;
using SceneObjectPtr = std::shared_ptr< SceneObject >;

class SceneObject
{
public:
    
    SceneObject();
    ~SceneObject() = default;
    
    uint64_t getId() const;
    
    void setMatrix( const Mat4d &i_matrix );
    Mat4d getMatrix() const;
    
private:
    
    uint64_t m_id;
    Mat4d m_matrix;
};

class Geometry;
using GeometryPtr = std::shared_ptr< Geometry >;

class Geometry : public SceneObject
{
public:
    
    static GeometryPtr create();
    
    Geometry();
    ~Geometry() = default;
    
    void setLOD( const Mesh &mesh, uint32_t lodIndex );
    
private:
    
    std::array< std::pair< Mesh, bool >, s_maxLODs > m_lods;
};

class Scene
{
public:
    
    void addObject( SceneObjectPtr object );
    void removeObject( SceneObjectPtr object );

private:
    
    std::unordered_map< uint64_t, SceneObjectPtr > m_objects;
};

} // namespace marlin

#endif /* MARLIN_SCENE_HPP */
