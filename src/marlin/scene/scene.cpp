//
//  scene.cpp
//  Marlin
//
//  Created by Jonathan Graham on 12/5/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include "scene.hpp"

#include <iostream>
#include <set>

namespace marlin
{

SceneObject::SceneObject( ScenePtr i_scene )
: m_parentScene( i_scene )
{
    static std::atomic< ObjectId > idCounter = 0;
    m_id = idCounter++;
}

ObjectId SceneObject::getId() const
{
    return m_id;
}

void SceneObject::setMatrix( const Mat4d &i_matrix )
{
    m_matrix = i_matrix;
}

Mat4d SceneObject::getMatrix() const
{
    return m_matrix;
}

void SceneObject::setDirty()
{
    ScenePtr scene = m_parentScene.lock();
    scene->setDirty( m_id );
}

GeometryPtr Geometry::create( ScenePtr i_scene )
{
    return std::make_shared< Geometry >( i_scene );
}

Geometry::Geometry( ScenePtr i_scene )
: SceneObject( i_scene )
{
    // Set all LODs to dirty
    for ( auto &pair : m_lods )
    {
        pair.second = true;
    }
}

void Geometry::setLOD( const Mesh &mesh, uint32_t lodIndex )
{
    if ( lodIndex >= s_maxLODs )
    {
        std::cerr << "Warning: LOD index greater than max supported indices. Ignoring." << std::endl;
    }
    
    m_lods[ lodIndex ] = { mesh, true };
    
    // Mark ourselves as dirty
    setDirty();
}

void Geometry::update( RenderStorage &i_renderStorage )
{
    for ( auto &pair : m_lods )
    {
        // Skip if not dirty
        if ( !pair.second )
        {
            continue;
        }
        
        // Update
        Mesh &mesh = pair.first;
        
//        std::byte* bytes = reinterpret_cast< std::byte* >( vertices.data() );
//        m_vertexBuffer = BufferT< std::byte >::create( m_device, m_physicalDevice, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, BufferMode::Local, bytes, vertices.size() * sizeof( Vertex ) );
//        
//        m_indexBuffer = BufferT< uint32_t >::create( m_device, m_physicalDevice, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, BufferMode::Device, indices );
        
        pair.second = false;
    }
}

ScenePtr Scene::create()
{
    return std::make_shared< Scene >();
}

void Scene::addObject( SceneObjectPtr object )
{
    ObjectId objectId = object->getId();
    m_objects[ objectId ] = object;
    m_dirtyList.push_back( objectId );
}

void Scene::removeObject( SceneObjectPtr object )
{
    m_objects.erase( object->getId() );
}

void Scene::update()
{
    std::set< ObjectId > processed;
    for ( ObjectId id : m_dirtyList )
    {
        // Check that our object hasn't been deleted
        const auto itr = m_objects.find( id );
        if ( itr == m_objects.end() )
        {
            continue;
        }
        
        // Check that our object hasn't already been processed
        const auto pair = processed.insert( id );
        if ( !pair.second )
        {
            continue;
        }
        
        SceneObjectPtr object = itr->second;
        object->update( m_renderStorage );
    }
    
    m_dirtyList.clear();
}

void Scene::setDirty( ObjectId id )
{
    m_dirtyList.push_back( id );
}

} // namespace marlin
