//
//  geometry.cpp
//  SDL2Test
//
//  Created by Scott Porter on 22/09/13.
//
#include <algorithm>
#include "geometry.h"

Object::Object(std::string name, Material * material)
{
	m_name = name;
    if ( !material )
    {
        material = new Material();
    }
    m_material = material;
}

Object::~Object()
{
	if ( m_material )
		delete m_material;
}

Material * Object::material()
{
    return m_material;
}

Sphere::Sphere(vec3 centre, float radius, Material * material) : Object("Sphere", material)
{
    m_centre = centre;
    m_radius = radius;
}

float * Sphere::intersectionTime(const Ray& ray)
{
    vec3 cp = m_centre - ray.point();
    float v = dot(cp, ray.vector());
    float discriminant = (m_radius * m_radius) - (dot(cp, cp) - v*v);
    
    if ( discriminant < 0.0f )
    {
        return NULL;
    }
    else
    {
        return new float(v - sqrtf(discriminant));
    }
}

vec3 Sphere::normalAt(vec3 point)
{
    return normalize(point - m_centre);
}

HalfSpace::HalfSpace(vec3 point, vec3 normal, Material * material) : Object("Plane", material)
{
    m_point = point;
    m_normal = normal;
}

float * HalfSpace::intersectionTime(const Ray& ray)
{
    float v = dot( ray.vector(), m_normal );
    
    if ( v )
    {
        return new float(1 / v);
    }
    else
    {
        return NULL;
    }
}