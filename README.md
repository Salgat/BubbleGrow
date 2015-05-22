BubbleGrow
=================

BubbleGrow is a free open-source game developed with the intention of being friendly for developers (particularly inexperienced) to contribute to. The goal includes not only being a fun game to play, but for the dependencies to be minimized, including limiting the number of external libraries used.

Build
-----------------

The easiest way to build BubbleGrow from source is to use CLion (IDE) or stand-alone CMake. For CLion, remember to go to settings->build->CMake->build output path and set to C:\your_directory\BubbleGrow\bin. The only dependancy (currently) is SFML. If you are compiling with 64 bit GCC (MinGW), the included binaries will work. If you aren't, you can use the SFML website to download the correct binaries for your compiler (located in /lib and /bin/Debug and /bin/Release).

I plan on including a tutorial for beginners to setup a build environment for this game, and highly suggest (for convenience) downloading the latest GCC from http://nuwen.net/mingw.html.