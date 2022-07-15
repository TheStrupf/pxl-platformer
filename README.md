# About
I'm documenting my coding journey of writing a retro platformer from scratch in C, heavily inspired by Handmade Hero (Casey Muratori) and [Raylib](https://www.raylib.com/). I'm originally coming from C# so coding in C is new to me. But I still feel pretty confident about this. I'll figure things out on the run. However, I probably won't include any other game assets besides the source code.

### Game concept

The game will end up as some kind of Metroidvania. I'm thinking about a healthy mix of The Legend of Zelda: The Wind Waker, Ori and the Blind Forest and Celeste. It should be lighthearted for the most part similar to TLOZ: The Wind Waker and A Hat in Time. I want to include multiple dedicated dungeons with boss fights and unique items needed for progression. I'm basically trying to make the game NES Zelda II should have been, at least according to my own vision.

# Tools
### Programming

* Visual Studio Code
* GCC compiler

Besides the base library for images, sound and input most things will be coded from scratch in C(99) with minimal use of external libraries. I'm using the following libraries:

* Raylib
* stdlib.h
* stdint.h
* stdbool.h
* stdio.h
* string.h
* float.h
* math.h

I currently compile and run my game with GCC on Windows using a simple .bat (see below). That .bat is located in the same folder as the src folder, next to the created game.exe, a lib folder for raylib and an asset folder for art, sounds, music and maps. Raylib is statically linked right now.
```
cd /d %~dp0
gcc src/*.c src/util/*.c src/game/*.c -o game.exe -O1 -Wall -std=c99 -Wno-missing-braces -I src/ -I src/include/ -I src/util/ -I src/game/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
game.exe
pause
```

### Art
* Aseprite

For Aseprite I'm using a [custom script](https://github.com/TheStrupf/pxl-platformer/blob/main/aseprite_export_indexed_json.lua) to export indexed images to a .json file. Aseprite palette: 0 = transparent index, 1 = first "regular" color. The script writes all indices to a long string of hex values: "255, 1, 31, ..." = "FF011 ...". However, all indices get reduces by 1 so the written index conforms to the actual palette index and the transparent index 0 becomes 255 = FF. Credits for the very first iteration: [community.aseprite.org/t/exporting-indices/5295](https://community.aseprite.org/t/exporting-indices/5295).

### Sound & Music
* Bfxr

### Misc
* Tiled Map Editor

# Links of (personal) interest

### Programming & Development
* Using Rust For Game Development ([video](https://www.youtube.com/watch?v=aKLntZcp27M), [blog](https://kyren.github.io/2018/09/14/rustconf-talk.html)) - Catherine West
* Celeste and TowerFall Physics ([blog](https://maddythorson.medium.com/celeste-and-towerfall-physics-d24bd2ae0fc5)) - Maddy Thorson
* Semantic Compression ([blog](https://caseymuratori.com/blog_0015)) - Casey Muratori
* Handmade Hero ([YT channel](https://www.youtube.com/c/MollyRocket) - Casey Muratori

### Game Design & Art
* Boss Keys ([video playlist](https://www.youtube.com/playlist?list=PLc38fcMFcV_ul4D6OChdWhsNsYY3NA5B2)) - Mark Brown (GMTK)
* Android Arts: Zelda 1-3 ([blog](https://androidarts.com/zelda/Zelda.htm)) - Arne Niklas Jansson

### Misc
* TONC - GBA programming ([website](https://www.coranac.com/tonc/text/))
* Super Mario 64 decompilation ([code](https://github.com/n64decomp/sm64))
* The Legend of Zelda: The Minish Cap decompilation ([code](https://github.com/zeldaret/tmc))
* Super NES features ([video playlist](https://www.youtube.com/playlist?list=PLHQ0utQyFw5KCcj1ljIhExH_lvGwfn6GV)) - Retro Game Mechanics Explained
* Android Arts ([website](https://androidarts.com/)) - Arne Niklas Jansson
