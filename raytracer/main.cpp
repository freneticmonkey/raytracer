//
//  main.m
//  SDL2Test
//
//  Created by Scott Porter on 21/09/13.
//

#include <iostream>
#include <stdio.h>
#include <time.h>

#include <chrono>
#include <ratio>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "raytracer.h"

#ifdef _WIN32
#include "windowsconsole.h"
#endif

void UpdateRender(Raytracer * raytracer, SDL_Window * window, int width, int height)
{
    auto start = std::chrono::steady_clock::now();
    
    raytracer->run(width, height);
    
    raytracer->WriteToSurface(SDL_GetWindowSurface(window), width, height);
    
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    
    std::cout << "Elapsed Time: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    
    SDL_UpdateWindowSurface(window);
}

int main(int argc, char * argv[])
{
#ifdef _WIN32
	OpenConsole();
#endif

    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        std::cout << "ERROR SDL_Init" << std::endl;
        
        return -1;
    }
    
    bool testRes = false;
    bool highRes = false;
    
    int width = 640;
    int height = 480;
    
    if ( testRes )
    {
        width = 128;
        height = 96;
    }
    
    if ( highRes )
    {
        width = 1920;
        height = 1080;
    }
    
    SDL_Window * window = SDL_CreateWindow("Raytracing!!",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width,
                                           height,
                                           SDL_WINDOW_OPENGL);
    
    Raytracer raytracer;
    
	UpdateRender(&raytracer, window, width, height);

	SDL_Event event;
    bool gameRunning = true;
    
    while ( gameRunning )
    {
        if ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )
            {
                gameRunning = false;
            }
            
            if ( event.type == SDL_KEYDOWN )
            {
				bool needUpdate = false;
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        gameRunning = false;
                        break;

					case SDLK_r:
						needUpdate = true;
						break;

					case SDLK_a:
						raytracer.moveX(1.0f);
						needUpdate = true;
						break;

					case SDLK_d:
						raytracer.moveX(-1.0f);
						needUpdate = true;
						break;

					case SDLK_w:
						raytracer.moveZ(1.0f);
						needUpdate = true;
						break;

					case SDLK_s:
						raytracer.moveZ(-1.0f);
						needUpdate = true;
						break;

					case SDLK_q:
						raytracer.moveY(-1.0f);
						needUpdate = true;
						break;

					case SDLK_z:
						raytracer.moveY(1.0f);
						needUpdate = true;
						break;
                }

				if ( needUpdate )
				{
					UpdateRender(&raytracer, window, width, height);
					needUpdate = false;
				}
            }
        }
        
        // Limit to 60 fps
        SDL_Delay(16);
    }
    
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}