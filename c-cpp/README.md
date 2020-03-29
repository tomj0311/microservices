# Setting up C-CPP Development for Microservice Environment (Windows 10 VsCode)

#### Install Visual Studio Build Tools or Community edition or Professional Edition

## Global INLCUDE/LIB PATH in Envrionment Settings
INCLUDE Path 
C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um;C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\shared;C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.23.28105\include;

LIB 
C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.23.28105\lib\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\um\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\ucrt\x64;

## include LIBPATH eg. ZeroMQ
cl /Zi /EHsc /Fe  z1.cpp /build/z1.exe D:\vcpkg\installed\x64-windows\lib\libzmq-mt-4_3_3.lib 

cl.exe /Zi /EHsc /MD /Fe: c:\Users\1026420\Desktop\microservices\c-cpp\build\z1.exe c:\Users\1026420\Desktop\microservices\c-cpp\z1.cpp /link /LIBPATH: D:\vcpkg\installed\x64-windows\lib\libzmq-mt-4_3_3.lib 

cl.exe /Zi /EHsc /MD -DSFML_STATIC /Fe: .\build\z1.exe .\z1.cpp C:\vcpkg\installed\x64-windows\lib\libzmq-mt-4_3_3.lib
