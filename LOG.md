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
