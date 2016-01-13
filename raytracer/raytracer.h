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

#include "JobSystem.h"

typedef std::shared_ptr<std::thread> ThreadPtr;
typedef std::shared_ptr<Scene> ScenePtr;


class RayTracerData : public JobSystem::Data<RayTracerData>
{
public:
    int m_start;
    int m_end;
    ScenePtr m_scene;
    RayTracerData(int start = 0, int end = 0, ScenePtr scene = nullptr)
    {
        m_start = start;
        m_end = end;
        m_scene = scene;
    }
    virtual ~RayTracerData() {}
    
};

class Raytracer
{
public:
    Raytracer(SDL_Window * window, int width, int height);
    ~Raytracer();

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

	void setupScene();

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
    
    static void threadRenderJob(JobSystem::Job* job, RayTracerData *data);
    
    void run();
    
    void Pump();
    
    void WriteToSurface();
private:
    PixelsPtr       m_pixels;
	vec3            camPos;
    ScenePtr        m_scene;
    int             m_sectionSize;
    unsigned int    m_threads;
    
    int m_width;
    int m_height;
    
    RayTracerData   m_rootData;
    JobSystem::Job* m_root;
    
    std::vector<RayTracerData*>     m_jobData;
    std::vector<JobSystem::Job*>    m_renderJobs;
    
    bool m_renderActive;
    bool m_useJobs;
    
    SDL_Surface*    m_surface;
    SDL_Window*     m_window;
    
    std::chrono::steady_clock::time_point m_renderStart;
    
    float m_time;
    
    std::chrono::system_clock::time_point m_onStart;
    
};



#endif
