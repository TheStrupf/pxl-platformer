# About
I'm documenting my coding journey of writing a retro platformer from scratch in C, heavily inspired by Handmade Hero (Casey Muratori) and [Raylib](https://www.raylib.com/). I'm originally coming from C# so coding in C is pretty new to me. I'll figure things out on the go.

However, I probably won't include any other game assets besides the source code.

## Game idea
The game will end up as some kind of Metroidvania. I'm thinking about a healthy mix of The Legend of Zelda: The Wind Waker, Ori and the Blind Forest and Celeste. I'm basically trying to make the game NES Zelda II should have been. We'll see how it goes.

## Programming
- C99 standard
- Custom coded indexed software rendering (PICO-8 extended palette), final frame buffer pushed to OpenGL via Raylib

I'll try to keep the dependency on Raylib and other external libraries low.

### Dependencies
- Raylib
- stdlib.h
- stdint.h
- stdbool.h
- float.h

### Building

I currently compile and run my game with GCC on Windows using a simple .bat:
```
cd /d %~dp0
gcc src/*.c -o game.exe -O1 -Wall -std=c99 -Wno-missing-braces -I src/include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
game.exe
pause
```
