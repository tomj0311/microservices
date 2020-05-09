## Working with SDL2

It is important ot have int main(int argc, char *argv[])        

cl.exe 
/Zi                                                             ** Debug mode else release
/EHsc 
/MD                                                             ** Multithreaded Debugger ot avoid warn mcvcrt conflict
/Fe                                                             ** Spcify output file and folder
./bin/debug/${filename}.exe
C:\vcpkg\installed\x64-windows\lib\SDL2.lib 
C:\vcpkg\installed\x64-windows\lib\manual-link\SDL2main.lib     ** Dynamic linking
/link /SUBSYSTEM:WINDOWS                                        ** entry point 

Textures - https://stackoverflow.com/questions/37644732/sdl-c-rendering-multiple-textures

Specifying SDL_HWSURFACE would cause the surface to be created using video memory. PCs without GPUs still have video memory and it is faster to put data in that memory area onto the screen than to copy it from system RAM.

Textures are uploaded to the GPU's own dedicated RAM and data must be passed through that memory in order to be put on the screen at all. SDL2 no longer has the SDL_HWSURFACE flag because the rendering subsystem uses the GPU via OpenGL or Direct3D and cannot use the old way to get graphics on the screen.