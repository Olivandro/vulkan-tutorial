#  Baisc Vulkan Triangle rendering in C 

Basic vulkan renderer, using LunarG SDK and MoltenVK (mac OS). This project only used the c language. This program is based of the excellent vulkan tutorial from [https://vulkan-tutorial.com](https://vulkan-tutorial.com). This tutorial, while great, is unfortunately in cpp, and I don't like writing cpp or oop languages. I have not seen (if at all) any guidance on how to use or create a vulkan renderer using only c, thus, I started this little project. 

As part of the project paridigm.. this project only uses c based external libraries for maths, image loading and model loading. The following header only libraries used:

1. [linmath.h](https://github.com/datenwolf/linmath.h) by datenwolf. _A simple header only math lib that follows GLSL_.
2. [stb_image.h](https://github.com/nothings/stb) by nothing.  
3. [tinyobj_loader_c.h](https://github.com/syoyo/tinyobjloader-c) by Syoyo. _A c lang version of tiny obj loader in cpp_

Included as part of the renderer is a shader compiling function that `<shaderc/shaderc.h>` from the vulkan library, which compiles the shaders at runtime and provides error return on GLSL syntax errors.


## Usage notes:
Currently this renderer can out of the box load simple 1 shape models with a single texture. 

Before compiling program, please change direct paths at the top of the main.c file to match the system paths of your machine. This is required for the shaders, textures and models. 

You do not have to compile your shaders separately! Simply ensure the absolute path to your .shader files and compile along side the rest of the program. At runtime the programe will compile the shaders. Currently only vert and frag shaders are supported. However, the functions provided can easily be extended to detect further shader types for compilation. 

Vulkan validation layer is enabled by default. NOTE: validation layer is incredibly useful and has saved me more time than I can count. If you have errors please read the validation error and refer to the Vulkan documentation.    



## TO DO:
1. Add MakeFile for Linux / Unix implemenation.
2. Clear up buffer creation with pass by ref (void function()) to simplifiy implementation. 
3. loadModel function to be adjusted to product unique vertices DrawingData struct.  
