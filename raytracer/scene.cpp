//
//  scene.cpp
//  SDL2Test
//
//  Created by Scott Porter on 22/09/13.
//

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <iostream>

#include "scene.h"

#ifdef _WIN32
const float Scene::EPSILON = 0.00001f;
#endif

Scene::Scene()
{
    m_position = vec3(0.0f, 1.8f, 10.0f);
    m_lookingAt = vec3(0.0f);
    m_fieldOfView = 45.0f;
    m_recursionDepth = 0;

	m_pixels = PixelsPtr(new Pixels());
}

Scene::~Scene()
{
}

void Scene::moveTo(vec3 p)
{
    m_position = p;
	printf("New Pos: x: %3.2f y: %3.2f z: %3.2f\n", m_position.x, m_position.y, m_position.z );
	fflush(stdout);
    
    eye = RayPtr(new Ray(m_position, m_lookingAt - m_position));
    vec3 cvec = cross(eye->vector(), vec3(0.0f, 1.0f, 0.0f));
    vpRight = normalize(cvec);
    vpUp = normalize(cross(vpRight, eye->vector()));
}

void Scene::lookAt(vec3 p)
{
    m_lookingAt = p;
}

void Scene::addObject(ObjectPtr newObject)
{
    m_objects.push_back(newObject);
}

void Scene::addLight(vec3 point)
{
    m_lightPoints.push_back(point);
}

void Scene::setupRender(int width, int height)
{
	configWidth = width;
	configHeight = height;
    float fovRadians = M_PI * (m_fieldOfView / 2.0f) / 180.0f;
    halfWidth = tan(fovRadians);
    halfHeight = 0.75f * halfWidth;
    float camWidth = halfWidth * 2.0f;
    float camHeight = halfHeight * 2.0f;
    pixelWidth = camWidth / (width - 1);
    pixelHeight = camHeight / (height - 1);
    
    // Initialise Pixel Storage
    
    // Resize outer vertical
    m_pixels->resize(configHeight);
    Pixels::iterator p = m_pixels->begin();
    
    // Fill inner horizontal
    for ( int i = 0; i < configHeight; i++ )
    {
        PixelLinePtr xpixels = PixelLinePtr(new PixelLine());
        xpixels->resize(configWidth);
        *p = xpixels;
        p++;
    }
}

void Scene::renderArea(int renderStart, int renderEnd)
{
    m_renderStart = renderStart;
    m_renderEnd = renderEnd;
    int recursionDepth = 0;
    for (int y = m_renderStart; y < m_renderEnd; y++)
    {
		PixelLine::iterator px = (*m_pixels)[y]->begin();
        
        vec3 ycomp = vpUp * (y * pixelHeight - halfHeight);

        for (int x = 0; x < configWidth; x++ )
        {
            vec3 xcomp = vpRight * (x * pixelWidth - halfWidth);
            Ray ray(eye->point(), eye->vector() + xcomp + ycomp);
            recursionDepth = 0;
            vec3 colour = rayColour(ray, recursionDepth);
            *px = colour;
			px++;
        }
    }
}

PixelsPtr Scene::getPixels() 
{ 
	//renderArea();//0, configHeight, m_pixels);
	return m_pixels;
}

vec3 Scene::rayColour(const Ray& ray, int& recursionDepth)
{
    if ( recursionDepth > Scene::MAX_RECURSION_DEPTH || recursionDepth < 0)
    {
        return vec3(0.0f);
    }
    
    recursionDepth += 1;
    
    Intersections ints;
    
    for ( auto &object : m_objects )
    {
        ints.push_back( new Intersection(object, object->intersectionTime(ray)) );
    }
    
    Intersection * intersection = firstIntersection(ints);
    
    vec3 returnColour(0.0f);
    
    if ( intersection != NULL )
    {
        //std::cout << "Hit: " << intersection->object->getName() << std::endl;
        vec3 point = ray.pointAtTime(intersection->time);
        
        //returnColour = intersection->object->material()->colourAt(point);
        
        returnColour = intersection->object->material()->colourAt(ray,
                                                                 point,
                                                                 intersection->object->normalAt(point),
                                                                 this,
                                                                 recursionDepth
                                                                 );    }
    deleteContainer(ints);
    
    recursionDepth -= 1;
    return returnColour;
}

Lights Scene::getVisibleLights(vec3 point)
{
    Lights visibleLights;
    
    for ( auto &light : m_lightPoints )
    {
        if ( lightIsVisible(light, point) )
        {
            visibleLights.push_back(light);
        }
    }
    
    return visibleLights;
}

bool Scene::lightIsVisible(vec3 lightPos, vec3 point)
{
    bool result = true;
    for ( auto &object : m_objects )
    {
        float * time = object->intersectionTime(Ray(point, lightPos - point));
        
        if ( (time != NULL) && ( *time > Scene::EPSILON ) )
        {
            result = false;
        }
        delete time;
        
        if (!result)
        {
            break;
        }
    }
    return result;
}

Intersection * Scene::firstIntersection(Intersections ints)
{
    Intersection * result = NULL;
    float * candidateT;
    
    for (auto i : ints )
    {
        candidateT = i->time;
        
        if ( ( candidateT != NULL ) && ( *candidateT > -Scene::EPSILON) )
        {
            if ( ( result == NULL ) || ( *candidateT < *result->time) )
            {
                result = i;
            }
        }
    }
    return result;
}