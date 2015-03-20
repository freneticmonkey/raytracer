//
//  scene.cpp
//  SDL2Test
//
//  Created by Scott Porter on 22/09/13.
//

#include <algorithm>
#include <SDL.h>
#include <math.h>
#include <iostream>

#include "scene.h"

#define _USE_MATH_DEFINES

#ifdef _WIN32
const float Scene::EPSILON = 0.00001f;
#endif

Scene::Scene(int renderStart, int renderEnd)
{
    m_position = vec3(0.0f, 1.8f, 10.0f);
    m_lookingAt = vec3(0.0f);
    m_fieldOfView = 45.0f;
    m_recursionDepth = 0;

	m_renderStart = renderStart;
	m_renderEnd = renderEnd;
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
    
    eye = RayPtr(new Ray(m_position, m_lookingAt - m_position));
    vec3 cvec = cross(eye->vector(), vec3(0.0f, 1.0f, 0.0f));
    vpRight = normalize(cvec);
    vpUp = normalize(cross(vpRight, eye->vector()));
    
}

//void Scene::renderArea(int start, int end, PixelsPtr result)
void Scene::renderArea()
{
	float previousfraction = 0.0f;
	//Pixels ypixels;

	// Resize the vector to the section size
	m_pixels->resize(m_renderEnd - m_renderStart);
	Pixels::iterator p = m_pixels->begin();
    for (int y = m_renderStart; y < m_renderEnd; y++)
    {
        float currentfraction = (float)(y - m_renderStart) / (m_renderEnd - m_renderStart);
        if ( currentfraction - previousfraction > 0.05f)
        {
			//printf("Render: %d : %d: Complete: %2.2f\n", m_renderStart, m_renderEnd, (currentfraction * 100) );
			//fflush(stdout);
            //std::cout << (currentfraction * 100) << "% complete" << std::endl;
            previousfraction = currentfraction;
        }
        
        PixelLinePtr xpixels = PixelLinePtr(new PixelLine());
		xpixels->resize(configWidth);
		PixelLine::iterator px = xpixels->begin();

        for (int x = 0; x < configWidth; x++ )
        {
            vec3 colour;
            
            vec3 xcomp = vpRight * (x * pixelWidth - halfWidth);
            vec3 ycomp = vpUp  * (y * pixelHeight - halfHeight);
            Ray ray = Ray(eye->point(), eye->vector() + xcomp + ycomp);
            colour = rayColour(ray);
            //xpixels->push_back(colour);
			*px = colour;
			px++;
        }
        //result->assign( = xpixels;
		*p = xpixels;
		p++;
    }
}

PixelsPtr Scene::getPixels() 
{ 
	//renderArea();//0, configHeight, m_pixels);
	return m_pixels;
}

vec3 Scene::rayColour(Ray ray)
{
    if ( m_recursionDepth > Scene::MAX_RECURSION_DEPTH )
    {
        return vec3(0.0f);
    }
    
    
    m_recursionDepth += 1;
    
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
                                                                 this
                                                                 );
        
    }
    deleteContainer(ints);
    
    m_recursionDepth -= 1;
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