//
//  ray.h
//  SDL2Test
//
//  Created by Scott Porter on 23/09/13.
//

#ifndef SDL2Test_ray_h
#define SDL2Test_ray_h

#include "glm/glm.hpp"

using namespace glm;

class Ray
{
public:
    Ray(vec3 point, vec3 vector)
    {
        m_point = point;
        m_vector = normalize(vector);
    };
    ~Ray() {};
    
    vec3 point() { return m_point; }
    vec3 vector() { return m_vector; }
    
    vec3 pointAtTime(float * t)
    {
        return m_point + m_vector * (*t);
    }
    
private:
    vec3 m_point;
    vec3 m_vector;
};


#endif
