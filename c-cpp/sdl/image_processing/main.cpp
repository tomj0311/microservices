#include <iostream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Transform.h"
//#include "GUI.h"

using namespace std;
using namespace transform;

const int SCREEN_WID		= 1280;
const int SCREEN_HEI		= 960;
const int WINDOW_WID		= SCREEN_WID;
const int WINDOW_HEI		= SCREEN_HEI;

int main(int argc, char* argv[]){
	if (SDL_Init(SDL_INIT_EVERYTHING) >0){
		cout << "Error: SDL failed to initialize." << endl;
		return 0;
	}

	SDL_Window*	window = SDL_CreateWindow("SDL Image Manipulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WID, WINDOW_HEI, NULL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	//Load and modify image
	SDL_Surface* image = IMG_Load("Resources/test.jpg");								//Load source image
	SDL_Surface* filtered = EmptySurface(image);											//Copy source image's pixel format, pixel data, and dimensions
	SDL_Surface* bg = EmptySurface(image, SCREEN_WID, SCREEN_HEI);

	//Generate background
	Fill(bg, CreateColor(50,50,50,255));

	SDL_Texture* tImage = SDL_CreateTextureFromSurface(renderer, image);
	SDL_Texture* tPost = SDL_CreateTextureFromSurface(renderer, filtered);
	SDL_Texture* back = SDL_CreateTextureFromSurface(renderer, bg);

	//Clean Up
	SDL_FreeSurface(image);
	SDL_FreeSurface(filtered);

	//Main loop
	bool quit = false;

	while (quit != true){
		while (SDL_PollEvent(&event) != 0){
			switch (event.key.keysym.sym){
				case SDLK_ESCAPE:
					quit = true;
					break;
			}
		}

		//Render media
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, back, NULL, NULL);
		SDL_RenderCopy(renderer, tImage, NULL, &CreateRect(0,SCREEN_HEI/4, SCREEN_WID/2, SCREEN_HEI/2));
		SDL_RenderCopy(renderer, tPost, NULL, &CreateRect(SCREEN_WID/2, SCREEN_HEI/4, SCREEN_WID/2, SCREEN_HEI/2));
		SDL_RenderPresent(renderer);
	}

	//Clean up
	SDL_Quit();

	return 0;
}