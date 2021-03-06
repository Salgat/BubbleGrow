cmake_minimum_required(VERSION 3.2)
project(BubbleGrow)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

#Add -mwindows to remove console window
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -o3 -fopenmp -mwindows")

set(SOURCE_FILES src/main.cpp)
add_executable(BubbleGrow ${SOURCE_FILES} src/Unit.hpp src/common.hpp src/Request.hpp src/Unit.cpp src/Player.hpp src/Player.cpp src/World.hpp src/World.cpp src/Renderer.hpp src/Renderer.cpp src/Resources.hpp src/Resources.cpp src/BatchDrawer.hpp src/BatchDrawer.cpp src/SoundManager.hpp src/SoundManager.cpp src/Event.hpp src/InputHandler.hpp src/InputHandler.cpp)

target_link_libraries(BubbleGrow ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsfml-graphics.a
                                 ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsfml-window.a
                                 ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsfml-audio.a
                                 ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsfml-network.a
                                 ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsfml-system.a)