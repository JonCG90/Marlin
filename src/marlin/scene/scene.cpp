//
//  scene.cpp
//  Marlin
//
//  Created by Jonathan Graham on 12/5/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include "scene.hpp"

#include <iostream>

namespace marlin
{

SceneObject::SceneObject()
{
    static uint64_t idCounter = 0;
    m_id = idCounter++;
}

uint64_t SceneObject::getId() const
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

GeometryPtr Geometry::create()
{
    return std::make_shared< Geometry >();
}

Geometry::Geometry()
: SceneObject()
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
}

void Scene::addObject( SceneObjectPtr object )
{
    m_objects[ object->getId() ] = object;
}

void Scene::removeObject( SceneObjectPtr object )
{
    m_objects.erase( object->getId() );
}

} // namespace marlin
