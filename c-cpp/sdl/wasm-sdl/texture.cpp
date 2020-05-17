#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdlib.h>

const unsigned int texWidth = 1024;
const unsigned int texHeight = 1024;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;

void drawRandomPixels() {

    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    Uint8 * pixels = (Uint8 *)surface->pixels;
    
    for (int i=0; i < 4194304; i++) {
        char randomByte = rand() % 255;
        pixels[i] = randomByte;
    }
    
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

    SDL_Texture *screenTexture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(screenTexture);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(texWidth, texHeight, 0, &window, &renderer);
    surface = SDL_CreateRGBSurface(0, 1024, 1024, 32, 0, 0, 0, 0);
    
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(drawRandomPixels, 0, 1);
    #else

    SDL_Event event;
    int running = 1, count = 1;
    while(running) {      

        printf("%d", count);
        count++;

        drawRandomPixels();
        
        while( SDL_PollEvent( &event ) )
        {
            if( ( SDL_QUIT == event.type ) ||
                ( SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode ) )
            {
                running = 0;
                break;
            }
        } 
    }
    #endif 
    return 0;
}