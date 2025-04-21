cmake_minimum_required(VERSION 3.10)
project(VoxelGame)  # Project name
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)


add_compile_definitions(GLEW_STATIC)


# Add executable target
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(my_program ${SOURCES})
# Specify the directory containing the libraries
target_compile_options(my_program PRIVATE -mavx2)

target_link_directories(my_program PRIVATE ${CMAKE_SOURCE_DIR}/bin/lib/x64/)
target_link_libraries(my_program PRIVATE
    glew32s
    libglfw3
    opengl32
    gdi32
)