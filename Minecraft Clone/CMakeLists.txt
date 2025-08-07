cmake_minimum_required(VERSION 3.28)
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
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    # Add patterns for module interface files if they have specific extensions
    # "src/*.cppm"
    "src/*.ixx"
)
add_executable(my_program ${SOURCES})
# Specify the directory containing the libraries
target_compile_options(my_program PRIVATE
    $<$<CONFIG:Debug>:
        -g
        -O0
    >
)

target_compile_options(my_program PRIVATE
    $<$<CONFIG:Release>:
        -O3
        -fmodules-ts
        -mavx2
        -mbmi
    >
)

target_link_directories(my_program PRIVATE ${CMAKE_SOURCE_DIR}/bin/lib/x64/)
target_link_libraries(my_program PRIVATE
    glew32s
    libglfw3
    opengl32
    gdi32
)