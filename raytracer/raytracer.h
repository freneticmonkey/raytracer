//
//  raytracer.h
//  SDL2Test
//
//  Created by Scott Porter on 21/09/13.
//

#ifndef SDL2Test_raytracer_h
#define SDL2Test_raytracer_h

#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "geometry.h"
#include "scene.h"

typedef std::shared_ptr<std::thread> ThreadPtr;
typedef std::shared_ptr<Scene> ScenePtr;

class Raytracer
{
public:
    Raytracer();
    ~Raytracer() {};

	void moveX(float unit)
    {
        camPos.x += unit;
        m_scene->moveTo(camPos);
    }
    void moveY(float unit)
    {
        camPos.y += unit;
        m_scene->moveTo(camPos);
    }
    void moveZ(float unit)
    {
        camPos.z += unit;
        m_scene->moveTo(camPos);
    }

	void setupScene(int width, int height);

	// Grabbed from StackOverflow.
    void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
    {
        Uint8 * pixel = (Uint8*)surface->pixels;
        pixel += (y * surface->pitch) + (x * sizeof(Uint32));
        *((Uint32*)pixel) = color;
    }
    
    void PutPixel32(SDL_Surface * surface, int x, int y, Uint32 color)
    {
        if( SDL_MUSTLOCK(surface) )
            SDL_LockSurface(surface);
        PutPixel32_nolock(surface, x, y, color);
        if( SDL_MUSTLOCK(surface) )
            SDL_UnlockSurface(surface);
    }
    
    void run();
    
    void WriteToSurface(SDL_Surface * surface, int width, int height);
private:
    PixelsPtr m_pixels;
	vec3 camPos;
    ScenePtr m_scene;
    int m_sectionSize;
    unsigned int m_threads;
    
};



#endif
