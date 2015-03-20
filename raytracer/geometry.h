//
//  geometry.h
//  SDL2Test
//
//  Created by Scott Porter on 22/09/13.
//

#ifndef SDL2Test_geometry_h
#define SDL2Test_geometry_h

#include "glm/glm.hpp"
#include "types.h"
#include "ray.h"
#include "material.h"
#include <string>

using namespace glm;

class Material;

class Object
{
public:
    Object(std::string name, Material * material = NULL);
    virtual ~Object();
    
    virtual float * intersectionTime(Ray r) = 0;
    virtual vec3 normalAt(vec3 point) = 0;
    
    std::string getName() { return m_name; }
    
    Material * material();
private:
    Material * m_material;
    std::string m_name;
};

class Sphere : public Object
{
    
public:
    Sphere(vec3 centre = vec3(0,0,0), float radius = 1.0f, Material * material = NULL);
    ~Sphere() {};
    
    vec3 centre() { return m_centre; }
    float radius()  { return m_radius; }
    
    float * intersectionTime(Ray ray);
    vec3 normalAt(vec3 point);
    
private:
    vec3    m_centre;
    float   m_radius;
};

class HalfSpace : public Object
{
public:
    HalfSpace(vec3 point, vec3 normal, Material * material = NULL);
    ~HalfSpace() {};
    
    vec3 point() { return m_point; }
    vec3 normal() { return m_normal; }
    
    float * intersectionTime(Ray ray);
    
    vec3 normalAt(vec3 point) { return m_normal; }
    
private:
    vec3 m_point;
    vec3 m_normal;
};

#endif
