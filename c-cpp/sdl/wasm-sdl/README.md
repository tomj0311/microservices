Web Assembly started with assembly language in mind
JavaScript is an interpreted language
It rook brenden Eich to create live script in 10 days

Source code -> Baseline compiler -> bytecode -> interpreter -> binary -> cpu

V8 engine -> source code -> full code gen to binary -> cpu
                         -> crankshaft -> binay -> cpu
						 
The TurboFan optimization compiler can optimize this bytecode in the background (in separate threads) as the application is running and generate a very optimized machine code that will be replaced eventually.

https://emscripten.org/docs/compiling/Deploying-Pages.html

https://www.hadean.com/blog/using-wasm-to-run-c-in-browser

websockify, a tool that “translates WebSockets traffic to normal socket traffic”!
Running websockify with ./run -v 9881 localhost:8881 (listening on port 9881 and forwarding to localhost on port 8881) 

https://github.com/mbasso/awesome-wasm#examples
https://github.com/svoisen/wasm-imageviewer

SDL2 Web Assembly
https://github.com/timhutton/sdl-canvas-wasm
emcc core.cpp -s WASM=1 -s USE_SDL=2 -O3 -o index.js

BINARYEN is requried for webassemply
emcc compile with preload binaries
emcc main.c -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s SDL2_IMAGE_FORMATS='["png"]' -o ../dist/index.html -O2 --preload-file assets

Working compile command for C in windows
emcc main.c -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=["png"] -O3 -o index.js --preload-file assets -v
emcc main.c -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -O3 -o index.js 

Simple HTML with js and wasm - emcc .\texture.c -s WASM=1 -o index.html -s USE_SDL=2  -s ALLOW_MEMORY_GROWTH=1 //Allow memory length is optional for large memory access

perfect examples
http://main.lv/writeup/web_assembly_sdl_example.md
https://www.it.iitb.ac.in/frg/wiki/images/a/ad/P1_Project_Presentation.pdf

https://medium.com/@martin.sikora/libwebsockets-simple-websocket-server-68195343d64b

It seems you are comparing frame size (2.34MBit) with the network speed (54MBit/sec). If that's the case, and you are getting 10 fps, your actual rate is 23.4 MBit/sec –– which is not that bad on a 54 MBit/sec connection.

https://webrtchacks.com/zoom-avoids-using-webrtc/ - webrtc + webassembly

https://livebook.manning.com/book/webassembly-in-action/b-ccall-cwrap-and-direct-function-calls/v-7/27
https://marcoselvatici.github.io/WASM_tutorial/

https://gist.github.com/cure53/f4581cee76d2445d8bd91f03d4fa7d3b

<html>
    <head>
        <meta charset="utf-8">
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    </head>
    <body>
        <script type='text/javascript'>
          var Module = {};
          fetch('index.wasm')
            .then(response =>
              response.arrayBuffer()
            ).then(buffer => {
              Module.canvas = document.getElementById("canvas");
              Module.wasmBinary = buffer;
              var script = document.createElement('script');
              script.src = "index.js";
              script.onload = function() {
                console.log("Emscripten boilerplate loaded.")
              }
              document.body.appendChild(script);
            });
        </script>
        <canvas id="canvas"></canvas>

    </body>
</html>
