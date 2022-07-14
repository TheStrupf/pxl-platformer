# About
I'm documenting my coding journey of writing a retro platformer from scratch in C, heavily inspired by Handmade Hero (Casey Muratori) and [Raylib](https://www.raylib.com/). I'm originally coming from C# so coding in C is new to me. But I still feel pretty confident about this. I'll figure things out on the run.

However, I probably won't include any other game assets besides the source code.

## Game idea
The game will end up as some kind of Metroidvania. I'm thinking about a healthy mix of The Legend of Zelda: The Wind Waker, Ori and the Blind Forest and Celeste. I'm basically trying to make the game NES Zelda II should have been. We'll see how it goes.

## Programming
- C99 standard
- Custom coded indexed software rendering (PICO-8 extended palette), final frame buffer pushed to OpenGL via Raylib

### Dependencies
- Raylib
- stdlib.h
- stdint.h
- stdbool.h
- stdio.h
- string.h
- float.h

I'll try to keep the dependency on Raylib and other external libraries low.

### Building

I currently compile and run my game with GCC on Windows using a simple .bat (see below). That .bat is located in the same folder as the src folder, next to the created game.exe, a lib folder for raylib and an asset folder for art, sounds, music and maps. Raylib is statically linked right now.
```
cd /d %~dp0
gcc src/*.c src/util/*.c src/game/*.c -o game.exe -O1 -Wall -std=c99 -Wno-missing-braces -I src/ -I src/include/ -I src/util/ -I src/game/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
game.exe
pause
```
