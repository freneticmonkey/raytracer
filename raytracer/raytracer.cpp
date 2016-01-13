//
//  raytracer.cpp
//  SDL2Test
//
//  Created by Scott Porter on 21/09/13.
//

#include <iostream>
#include <thread>
#include <functional>


#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "raytracer.h"

Raytracer::Raytracer(SDL_Window * window, int width, int height)
{
    camPos = vec3(0.0f, 2.5f, -5.0f);
    m_scene = ScenePtr(new Scene());
    
	m_threads = std::thread::hardware_concurrency();
    
    m_renderActive = false;
    m_useJobs = true;
    
    m_window = window;
    
    
    m_surface = SDL_GetWindowSurface(m_window);
    m_width = width;
    m_height = height;
    
    m_time = 0.0f;
    
    m_onStart = std::chrono::system_clock::now();
    
    JobSystem::Setup();
    
    setupScene();
    
};


Raytracer::~Raytracer()
{
    if ( m_jobData.size() > 0 )
    {
        for ( auto renderJob : m_jobData )
        {
            delete renderJob;
        }
    }
    
    JobSystem::Shutdown();
}

void Raytracer::setupScene()
{
    // Configure the base scene
    m_sectionSize = m_height / m_threads;
    
	m_scene->addLight(vec3(0.0f, -10.0f, -10.0f));
//	m_scene->addLight(vec3(0.0f, -10.f, 10.0f));
	m_scene->lookAt(vec3(0.0f, 4.0f, 0.0f));

	ObjectPtr firstSphere( new Sphere(vec3(-2.5f, 3.0f, 0.0f), 1.0f));
	firstSphere->material()->setColour(vec3(1.0f, 0.0f, 0.0f))->setTurbulance(0.5)->setBump(10.0);
	m_scene->addObject( firstSphere );
	
	//scene->addObject( ObjectPtr( new Sphere(vec3(1.25f, 3.0f, 0.0f), 1.0f, new Material(vec3(0.0f, 1.0f, 0.0f)))) );

	m_scene->addObject( ObjectPtr( new Sphere(vec3(2.5f, 3.0f, 0.0f),
											1.0f, 
											new CheckerboardMaterial(vec3(0.0f, 0.0f, 1.0f), 
																	 vec3(1.0f, 0.5f, 0.0f),
																	 3.0f,
																	 0.2f,
																	 0.7f
																	 )
											) ));
	ObjectPtr newSphere( new Sphere(vec3(0.0f, 3.0f, 0.0f), 1.0f ) );
	newSphere->material()->setColour( vec3(0.0f, 1.0f, 0.0f) )
								   ->setOtherColour( vec3(0.0f) )
								   ->setMarble(0.5);
								   //->setTurbulance(0.5);
	m_scene->addObject( newSphere );

	//scene->addObject( ObjectPtr( new HalfSpace(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), new Material(vec3(1.0f, 0.0f, 0.0f)) )) );
	m_scene->addObject( ObjectPtr( new HalfSpace(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), new CheckerboardMaterial())) );
	
	// Walls
	//scene->addObject( ObjectPtr( new HalfSpace(vec3(-4.0f, 2.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), new Material(vec3(1.0f, 0.0f, 0.0f)) )) );
	//scene->addObject( ObjectPtr( new HalfSpace(vec3(4.0f, 2.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), new Material(vec3(0.0f, 1.0f, 0.0f)) )) );

    m_scene->setupRender(m_width, m_height);
    
    // Setup the jobs for the renderer
    int jobLines = 1;
    int start = 0;
    
    bool needInit = m_jobData.size() == 0;
    
    if ( needInit )
    {
        m_root = JobSystem::CreateJob(m_rootData.Bind( threadRenderJob ) );
        
        for (int i = 0; i < m_height; i++ )
        {
            RayTracerData* renderJob = new RayTracerData(start, start + 1, m_scene);
            start += jobLines;
            
            m_jobData.push_back(renderJob);
            
            m_renderJobs.push_back( JobSystem::CreateJobAsChild(m_root, renderJob->Bind(threadRenderJob)) );
        }
    }
}

void Raytracer::threadRenderJob(JobSystem::Job* job, RayTracerData *data)
{
    data->m_scene->renderArea(data->m_start, data->m_end);
}

void Raytracer::run()
{
    // Render
    if ( !m_renderActive )
    {
        m_time = std::chrono::duration <float, std::milli> (std::chrono::system_clock::now() - m_onStart).count() / 1000.0f;
        
        camPos.x = 10 * cosf(m_time);
        camPos.z = 10 * sinf(m_time);
        m_scene->moveTo(camPos);
        
        m_renderStart = std::chrono::steady_clock::now();
        
        m_renderActive = true;
        
        if ( m_useJobs )
        {
            JobSystem::StartFrame();
            m_root->unfinishedJobs = m_renderJobs.size() + 1;
            
            for ( auto job : m_renderJobs )
            {
                // Increment job and root
                job->unfinishedJobs = 1;
                JobSystem::Run(job);
            }
            JobSystem::Run(m_root);
        }
        else
        {
            std::vector<ThreadPtr> threadList;
            
            // Kick off rendering
            for ( int i = 0; i < m_threads; i++ )
            {
                // Configure the thread render region
                int from = i * m_sectionSize;
                int to = from + m_sectionSize;
                ThreadPtr renderThread = ThreadPtr(new std::thread(std::bind(&Scene::renderArea, m_scene, from, to)) );
                threadList.push_back( renderThread );
            }
            
            // Join the render threads
            for ( auto &thread : threadList )
            {
                thread->join();
            }
            
            m_renderActive = false;
        }
    }
}

void Raytracer::Pump()
{
    if ( m_useJobs )
    {
        m_renderActive = m_root->unfinishedJobs > 0;
        
        if ( !m_renderActive )
        {
            auto end = std::chrono::steady_clock::now();
            auto diff = end - m_renderStart;
            
            std::cout << "Total Render Time: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
            WriteToSurface();
            
            JobSystem::EndFrame();
            
            run();

        }
    }
}

void Raytracer::WriteToSurface()
{
    PixelsPtr pixels = m_scene->getPixels();
    
    if ( pixels->size() == m_height )
	{

		for ( int y = 0; y < m_height; y++ )
		{
			PixelLinePtr line = pixels->at(y);

			if ( y == 0 )
			{
				if ( line->size() != m_width )
				{
					std::cout << "ERROR: Invalid number of x pixels: " << line->size() << std::endl;
					std::cout << "ERROR: Stopping...\n";
				}
			}
            
			for ( int x = 0; x < m_width; x++ )
			{
				vec3 pixel = line->at(x);

				Uint8 r = std::min(255.0f, (pixel.x * 255));
				Uint8 g = std::min(255.0f, (pixel.y * 255));
				Uint8 b = std::min(255.0f, (pixel.z * 255));
				PutPixel32(m_surface, x, y, SDL_MapRGB(m_surface->format, r, g, b) );
			}
		}
	}
	else
	{
		std::cout << "ERROR: Invalid number of y pixels: " << m_pixels->size() << std::endl;
	}
    
    SDL_UpdateWindowSurface(m_window);
}