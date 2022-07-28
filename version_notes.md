**2022.07.28a**
- Restructured engine code
- Solid platform movement

**2022.07.22a**
- Lighting: Replaced sqrt lookup table by an inlined, unrolled and optimised sqrt function
- Added variable height jumping
- Added gfx circle methods

**2022.07.21a**
- Added a bresenham-based per pixel lighting system 

**2022.07.19a**
- Jumping, acceleration, drag
- Texture loading from file
- Added stack allocation functionality
- List struct list_find: Using memcmp for finding a provided element

**2022.07.17a**
- Fixed main loop bug
- Added sound playing
- Added basic camera functionality
- Prepared movement of platforms/solids
- Transformed vector functions to macros

**2022.07.16b**
- Get pixels of solid entities
- json: var naming fix
- json: allow trailing commas

**2022.07.16a**
- Added text file reader
- Added entity inheritance
- Added gfx palette
- Added custom token-based json parser

**2022.07.15a**
- Added math functions - integer square root, matrix 2x2 (float) and 2D vector (float)
- Added an affine sprite method - sprite shearing, scaling and rotation
- All textures are now pow2 sized internally to replace "* width" with "<< shift" for a minor performance boost

**2022.07.14d**
- Related to previous: Added a variant of list_get(list*, int, void*) using memcpy. Not as user-friendly as I would like but it does the job.
- Pulled the entity specific part out of world.h/.c

**2022.07.14c**
- Added custom memory allocator for fun (freelist) although it's not active yet
- There was a bug with the dynamic list void* list_get(list*, int) function. Apparantly it got me weird addresses. Casting the void* pointer of the list data to an array and then retrieving the element that way worked fine, though. Cut that function for now.
- Setting and querying of world pixels
- Entity creation
- Basic entity movement with solid pixels
- First hardcoded abstraction of the input system

**2022.07.14b**
- Added more gfx functionality (line, rect)
- Added world structure + methods/functions

**2022.07.14a**
- Changelog creation
- Added basic indexed software rendering: rectangular sprites with mirroring and 90 degree rotations
- Hardcoded Pico-8 extended palette
- Macros for binary literals (Credits: [https://stackoverflow.com/a/7577517](https://stackoverflow.com/a/7577517))
- Added a basic generic dynamic list/array implementation using void pointers
