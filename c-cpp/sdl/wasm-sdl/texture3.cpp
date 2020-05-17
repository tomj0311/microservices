#include <SDL2/SDL.h>

//In order to access SDL_image features, we must include SDL_image.h
#include <SDL2/SDL_image.h>


int main(int argc, char** args)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    //For loading PNG images
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Getting Started", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Event input;
    bool quit = false;

    SDL_Texture* texture = NULL;
    SDL_Surface* temp = IMG_Load("awesome_face.png");
    //Filling texture with the image using a surface
    texture = SDL_CreateTextureFromSurface(renderer, temp);
    //Deleting the temporary surface
    SDL_FreeSurface(temp);

    SDL_Rect rect;
    rect.x = 0; //Extreme left of the window
    rect.y = 500; //Very bottom of the window
    rect.w = 100; //100 pixels width
    rect.h = 100; //100 pixels height
    //'rect' defines the dimensions for the bottom-left of the window

    while (!quit)
    {
        while (SDL_PollEvent(&input) > 0)
        {
            if (input.type == SDL_QUIT) quit = true;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //Copying the texture on to the window using renderer and rectangle
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_RenderPresent(renderer);
    }

    //Deleting the texture
    SDL_DestroyTexture(texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    //For quitting IMG systems
    IMG_Quit();

    SDL_Quit();

    return 0;
}