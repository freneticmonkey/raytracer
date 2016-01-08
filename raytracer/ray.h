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
    
    const vec3& point() const { return m_point; }
    const vec3& vector() const { return m_vector; }
    
    vec3 pointAtTime(float* t) const
    {
        return m_point + m_vector * (*t);
    }
    
private:
    vec3 m_point;
    vec3 m_vector;
};


#endif
