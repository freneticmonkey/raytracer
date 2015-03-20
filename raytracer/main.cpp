//
//  main.m
//  SDL2Test
//
//  Created by Scott Porter on 21/09/13.
//

#include <iostream>
#include <stdio.h>
#include <time.h>

#include <SDL.h>

#include "raytracer.h"

#ifdef _WIN32
#include "windowsconsole.h"
#endif

double diffclock(clock_t clock1,clock_t clock2) 
{ 
	double diffticks=clock1-clock2; 
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC; 
	return diffms; 
}

void UpdateRender(Raytracer * raytracer, SDL_Window * window, int width, int height)
{
	clock_t start = clock();
	std::cout << "Start: " << start << std::endl;
    raytracer->run(width, height, 8);
    
    raytracer->WriteToSurface(SDL_GetWindowSurface(window), width, height);
    
	clock_t end = clock();
	std::cout << "End: " << end << std::endl;

	printf("Elapsed Time: %3.3f\n", diffclock(end, start));
	fflush(stdout);

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
    
    bool testRes = true;
    
    int width = 640;
    int height = 480;
    
    if ( testRes )
    {
        width = 128;
        height = 96;
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