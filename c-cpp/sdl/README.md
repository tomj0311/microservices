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

