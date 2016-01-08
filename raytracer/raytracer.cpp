//
//  raytracer.cpp
//  SDL2Test
//
//  Created by Scott Porter on 21/09/13.
//

#include <iostream>
#include <thread>

#include "raytracer.h"

void Raytracer::setupScene(ScenePtr scene, int width, int height)
{
	scene->moveTo(camPos);
	scene->addLight(vec3(0.0f, -10.0f, -10.0f));
	//scene->addLight(vec3(0.0f, 10.f, 10.0f));
	scene->lookAt(vec3(0.0f, 4.0f, 0.0f));

	ObjectPtr firstSphere( new Sphere(vec3(-2.5f, 3.0f, 0.0f), 1.0f));
	firstSphere->material()->setColour(vec3(1.0f, 0.0f, 0.0f))->setTurbulance(0.5)->setBump(10.0);
	scene->addObject( firstSphere );
	
	//scene->addObject( ObjectPtr( new Sphere(vec3(1.25f, 3.0f, 0.0f), 1.0f, new Material(vec3(0.0f, 1.0f, 0.0f)))) );

	scene->addObject( ObjectPtr( new Sphere(vec3(2.5f, 3.0f, 0.0f), 
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
	scene->addObject( newSphere );

	//scene->addObject( ObjectPtr( new HalfSpace(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), new Material(vec3(1.0f, 0.0f, 0.0f)) )) );
	scene->addObject( ObjectPtr( new HalfSpace(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), new CheckerboardMaterial())) );
	
	// Walls
	//scene->addObject( ObjectPtr( new HalfSpace(vec3(-4.0f, 2.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), new Material(vec3(1.0f, 0.0f, 0.0f)) )) );
	//scene->addObject( ObjectPtr( new HalfSpace(vec3(4.0f, 2.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), new Material(vec3(0.0f, 1.0f, 0.0f)) )) );

	scene->setupRender(width, height);
}

void Raytracer::run(int width, int height)
{
	// Render
    
    unsigned int threads = std::thread::hardware_concurrency();
		
	if ( threads == 1 )
	{
		ScenePtr scene(new Scene(0, height));
		setupScene(scene, width, height);
		m_pixels = scene->getPixels();
	}
	else
	{
		m_pixels = PixelsPtr(new Pixels());

		int sectionSize = height / threads;
		std::vector<ThreadPtr> threadList;
		std::vector<ScenePtr> threadScenes;

		// Configure the thread objects
		for ( int i = 0; i < threads; i++ )
		{
			int from = i * sectionSize;
			int to = from + sectionSize;
			ScenePtr newScene = ScenePtr(new Scene(from, to) );
			setupScene(newScene, width, height);
			threadScenes.push_back(newScene);
		}

		// Kick off rendering
		for ( int i = 0; i < threads; i++ )
		{
			ThreadPtr renderThread = ThreadPtr(new std::thread(&Scene::renderArea, threadScenes[i]) );
			threadList.push_back( renderThread );
		}

		// Join the render threads
		for ( auto &thread : threadList )
		{
			thread->join();
		}

		
		// build the pixel result.
		int sectionCount = 0;
		for ( std::vector<ScenePtr>::iterator aScene = threadScenes.begin(); aScene != threadScenes.end(); aScene++ )
		{
			ScenePtr scene = *aScene;
			PixelsPtr pixels = scene->getPixels();

			std::cout << "Length Section: "<< sectionCount << " # lines: " << pixels->size() << std::endl;
			for (auto &line : *pixels)
			{
				m_pixels->push_back(line);
			}
			sectionCount++;
		}
	}

}
    
void Raytracer::WriteToSurface(SDL_Surface * surface, int width, int height)
{
    std::cout << "height: " << m_pixels->size() << std::endl;
        
    int y = 0;

	if ( m_pixels->size() == height )
	{

		//for ( auto & line : m_pixels )
		for ( int y = 0; y < height; y++ )
		{
			PixelLinePtr line = m_pixels->at(y);

			if ( y == 0 )
			{
				std::cout << "width: " << line->size() << std::endl;

				if ( line->size() != width )
				{
					std::cout << "ERROR: Invalid number of x pixels: " << line->size() << std::endl;
					std::cout << "ERROR: Stopping...\n";
				}
			}
            
			int x = 0;
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
}