Compiled with MinGW 1310 x64 on Windows11 x64 using Linux only Tools.

+ Toolchain uses:
	+ CMake, 
	+ MinGW1310 x64 GCC compiler suite from Qt6 SDK
	+ Git4Windows for bash Terminal on Win11
	- Does not use QT

+ Uses and links staticly to third party libraries:
	+ GLM
	+ stb_image
	+ stb_truetype
	+ GLEW
	+ GLFW
	+ Assimp
	+ ImGUI

+ Links staticly to GCC runtime, ergo only one exe file, no dll dependencies.

- Camera controls are very basic. you have to find a good camera pos yourself.
- No dynamic model loading so far, so it's only the included model so far.


