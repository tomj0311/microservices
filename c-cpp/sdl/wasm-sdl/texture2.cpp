#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

using namespace std;

const unsigned int texWidth = 800;
const unsigned int texHeight = 600;

SDL_Renderer* renderer = NULL;
SDL_Texture* texture  = NULL;
SDL_Surface* surface = NULL;

void drawRandonPixels(){
    
    vector<unsigned char> pixels( texWidth * texHeight * 4, 0 );

    for( unsigned int i = 0; i < 100000; i++ )
    {
        const unsigned int x = rand() % texWidth;
        const unsigned int y = rand() % texHeight;

        const unsigned int offset = ( texWidth * 4 * y ) + x * 4;
        pixels[ offset + 0 ] = rand() % 256;                // b
        pixels[ offset + 1 ] = rand() % 256;                // g
        pixels[ offset + 2 ] = rand() % 256;                // r
        pixels[ offset + 3 ] = SDL_ALPHA_OPAQUE;       // a
    }        
    
    // Old way of doing texures
    // unsigned char* lockedPixels;
    // int pitch;
    // SDL_LockTexture(
    //    texture,
    //    NULL,
    //    reinterpret_cast< void** >( &lockedPixels ),
    //    &pitch
    // );

    // std::copy( pixels.begin(), pixels.end(), lockedPixels );
    // SDL_UnlockTexture( texture );

    SDL_UpdateTexture(
        texture,
        NULL,
        &pixels[0],
        texWidth * 4
    );

    SDL_RenderClear(renderer);
    SDL_RenderCopy( renderer, texture, NULL, NULL );
    SDL_RenderPresent( renderer );
}

int main( int argc, char** argv )
{
    SDL_Init( SDL_INIT_VIDEO );

    SDL_Window* window = SDL_CreateWindow(
        "SDL2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        texWidth, texHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        texWidth, texHeight
    );

    SDL_RendererInfo info;
    SDL_GetRendererInfo( renderer, &info );
    cout << "Renderer name: " << info.name << endl;
    cout << "Texture formats: " << endl;
    for( Uint32 i = 0; i < info.num_texture_formats; i++ )
    {
        cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << endl;
    }

    SDL_Event event;
    int running = 1, count  = 0;
    while( running )
    {
        const Uint64 start = SDL_GetPerformanceCounter();

        while( SDL_PollEvent( &event ) )
        {
            if( ( SDL_QUIT == event.type ) ||
                ( SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode ) )
            {
                running = false;
                break;
            }
        }

        drawRandonPixels();

        const Uint64 end = SDL_GetPerformanceCounter();
        const static Uint64 freq = SDL_GetPerformanceFrequency();
        const double seconds = ( end - start ) / static_cast< double >( freq );
        cout << "Frame time: " << seconds * 1000.0 << "ms count" << count << endl ; 
        count++; 
    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}