#include <SDL2/SDL.h>
#include <cstdlib>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    
    // example: draw a moving rectangle
    
    // red background
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // moving blue rectangle
    SDL_Rect r;
    r.x = ctx->iteration % 255;
    r.y = 50;
    r.w = 50;
    r.h = 50;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect(renderer, &r );

    SDL_RenderPresent(renderer);

    ctx->iteration++;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
#else
    while(1){
        mainloop(&ctx);
        SDL_Delay(16);
    }
#endif    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
