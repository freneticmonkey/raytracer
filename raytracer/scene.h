//
//  scene.h
//  SDL2Test
//
//  Created by Scott Porter on 22/09/13.
//

#ifndef SDL2Test_scene_h
#define SDL2Test_scene_h

#include <vector>
#include <memory>
#include "types.h"
#include "geometry.h"

using namespace glm;

class Object;
class Material;

//typedef std::shared_ptr<vec3> PixelPtr;
typedef std::vector<vec3> PixelLine;
typedef std::shared_ptr<PixelLine> PixelLinePtr;
typedef std::vector<PixelLinePtr> Pixels;

typedef std::shared_ptr<Pixels> PixelsPtr;

typedef std::shared_ptr<Object> ObjectPtr;
typedef std::vector<ObjectPtr> Objects;

template<typename Container>
void deleteContainer(Container &c)
{
    while(!c.empty())
    {
        delete c.back(), c.pop_back();
    }
}

struct Intersection
{
    ObjectPtr object;
    float  * time;
    
    Intersection(ObjectPtr iObject, float * t)
    {
        object = iObject;
        time = t;
    }
    ~Intersection()
    {
        if ( time != NULL )
        {
            delete time;
            time = NULL;
        }
    }
};

typedef std::vector<Intersection *> Intersections;
typedef std::shared_ptr<Ray> RayPtr;

class Scene
{
public:
    Scene(int renderStart, int renderEnd);
    ~Scene();
    
    void moveTo(vec3 p);
    void lookAt(vec3 p);
    void addObject(ObjectPtr newObject);
    void addLight(vec3 point);
    void setupRender(int width, int height);
	
	//void renderArea(int start, int end, PixelsPtr result);
	void renderArea();
    
	// Single threaded function - renders all pixels
    PixelsPtr getPixels();
    
    vec3 rayColour(Ray ray);
    Lights getVisibleLights(vec3 point);
    
    static const int MAX_RECURSION_DEPTH = 3;

#ifdef _WIN32
	static const float EPSILON;
#endif
#ifdef __APPLE__
    constexpr static const float EPSILON = 0.00001f;
#endif
    
private:
    bool lightIsVisible(vec3 lightPos, vec3 point);
    Intersection * firstIntersection(Intersections ints);
    
    Objects m_objects;
    Lights  m_lightPoints;
    vec3    m_position;
    vec3    m_lookingAt;
    float   m_fieldOfView;
    int     m_recursionDepth;

	int configHeight;
	int configWidth;
	float halfHeight;
	float halfWidth;
	float pixelWidth;
	float pixelHeight;

	int m_renderStart;
	int m_renderEnd;

	vec3 vpRight;
	vec3 vpUp;
	RayPtr eye;
    
    PixelsPtr  m_pixels;
    
};

#endif
