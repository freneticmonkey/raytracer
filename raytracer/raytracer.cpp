//
//  raytracer.cpp
//  SDL2Test
//
//  Created by Scott Porter on 21/09/13.
//

#include <iostream>
#include <thread>

#include "raytracer.h"

Raytracer::Raytracer()
{
    camPos = vec3(0.0f, 2.5f, -5.0f);
    m_scene = ScenePtr(new Scene());
    
    m_threads = std::thread::hardware_concurrency();
    
};


void Raytracer::setupScene(int width, int height)
{
    // Configure the base scene
    m_sectionSize = height / m_threads;
    
	m_scene->addLight(vec3(0.0f, -10.0f, -10.0f));
	//scene->addLight(vec3(0.0f, 10.f, 10.0f));
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

    m_scene->setupRender(width, height);
}

void Raytracer::run()
{
    // Render
    m_scene->moveTo(camPos);
    
    std::vector<ThreadPtr> threadList;
    
    auto start = std::chrono::steady_clock::now();
    
    // Kick off rendering
    for ( int i = 0; i < m_threads; i++ )
    {
        // Configure the thread render region
        int from = i * m_sectionSize;
        int to = from + m_sectionSize;
        std::cout << "From " << from << " To: " << to << std::endl;
        ThreadPtr renderThread = ThreadPtr(new std::thread(std::bind(&Scene::renderArea, m_scene, from, to)) );
        threadList.push_back( renderThread );
    }

    // Join the render threads
    for ( auto &thread : threadList )
    {
        thread->join();
    }
    
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    
    std::cout << "Calculation Time: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

}
    
void Raytracer::WriteToSurface(SDL_Surface * surface, int width, int height)
{
    auto start = std::chrono::steady_clock::now();
    
    PixelsPtr pixels = m_scene->getPixels();
    
    std::cout << "height: " << pixels->size() << std::endl;
    
    if ( pixels->size() == height )
	{

		//for ( auto & line : m_pixels )
		for ( int y = 0; y < height; y++ )
		{
			PixelLinePtr line = pixels->at(y);

			if ( y == 0 )
			{
				std::cout << "width: " << line->size() << std::endl;

				if ( line->size() != width )
				{
					std::cout << "ERROR: Invalid number of x pixels: " << line->size() << std::endl;
					std::cout << "ERROR: Stopping...\n";
				}
			}
            
			//for ( auto & pixel : line)
			for ( int x = 0; x < width; x++ )
			{
				vec3 pixel = line->at(x);

				//std::cout << "r: " << pixel.x << " g: " << pixel.y << " z: " << pixel.z << std::endl;
                
				Uint8 r = std::min(255.0f, (pixel.x * 255));
				//Uint8 r = std::min(255.0f, (x/640.0f * 255));
				Uint8 g = std::min(255.0f, (pixel.y * 255));
				//Uint8 g = std::min(255.0f, (y/480.0f * 255));
                
				Uint8 b = std::min(255.0f, (pixel.z * 255));
				PutPixel32(surface, x, y, SDL_MapRGB(surface->format, r, g, b) );
				//x++;
			}
			//y++;
		}
	}
	else
	{
		std::cout << "ERROR: Invalid number of y pixels: " << m_pixels->size() << std::endl;
	}
    
    
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    
    std::cout << "Blit Time: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
}