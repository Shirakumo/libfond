## About Libfond
This is a library for font rendering, allowing you to easily and quickly render TrueType font text to OpenGL.

## API
The API is documented in [fond.h](fond.h).

## Building It
Requirements:

* CMake 3.1+
* A C99 compiler
* OpenGL

Steps:

* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

If GLEW and GLFW are installed on the system as well, a quick demo application is compiled as well.

## Included Sources
* [stb_rect_pack.h](https://github.com/nothings/stb/blob/master/stb_rect_pack.h)
* [stb_truetype.h](https://github.com/nothings/stb/blob/master/stb_truetype.h)
* [utf8.h](https://github.com/sheredom/utf8.h/blob/master/utf8.h)
* [GL/glext.h](https://www.khronos.org/registry/OpenGL/api/GL/glext.h)
* [GL/wglext.h](https://www.khronos.org/registry/OpenGL/api/GL/wglext.h)
