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
#include <marlin/scene/renderStorage.hpp>

#include <array>
#include <unordered_map>

namespace marlin
{

static const uint32_t s_maxLODs = 5;

class Scene;
using ScenePtr = std::shared_ptr< Scene >;

class SceneObject;
using SceneObjectPtr = std::shared_ptr< SceneObject >;

using ObjectId = uint64_t;

class SceneObject
{
    friend class Scene;
    
public:
    
    explicit SceneObject( ScenePtr i_scene );
    ~SceneObject() = default;
    
    ObjectId getId() const;
    
    void setMatrix( const Mat4d &i_matrix );
    Mat4d getMatrix() const;
    
protected:
    
    std::weak_ptr< Scene > m_parentScene;
    
    virtual void update( RenderStorage &i_renderStorage ) = 0;
    void setDirty();

private:
        
    ObjectId m_id;
    Mat4d m_matrix;
};

class Geometry;
using GeometryPtr = std::shared_ptr< Geometry >;

class Geometry : public SceneObject
{
public:
    
    static GeometryPtr create( ScenePtr i_scene );
    
    explicit Geometry( ScenePtr i_scene );
    ~Geometry() = default;
    
    void setLOD( const Mesh &mesh, uint32_t lodIndex );
    
protected:
    
    void update( RenderStorage &i_renderStorage ) override;
    
private:
    
    // LOD array of mesh and dirty states
    std::array< std::pair< Mesh, bool >, s_maxLODs > m_lods;
};

class Scene
{
public:
    
    static ScenePtr create();
    
    void addObject( SceneObjectPtr object );
    void removeObject( SceneObjectPtr object );
    void update();
    void setDirty( ObjectId id );
    
private:
        
    std::unordered_map< ObjectId, SceneObjectPtr > m_objects;
    std::vector< ObjectId > m_dirtyList;
    RenderStorage m_renderStorage;
};

} // namespace marlin

#endif /* MARLIN_SCENE_HPP */
