cmake_minimum_required(VERSION 3.8)
project(Snake-Game)

# Specify where to find source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
set(GLAD_SRC external/glad/src/glad.c)
set(STB_IMAGE_SRC external/stb-image/src/stb_image.cpp)
add_subdirectory(external/glfw-3.4)
add_subdirectory(external/freetype-2.13.3)

# Specify everything that is required for .exe
add_executable(Snake-Game ${SOURCES} ${GLAD_SRC} ${STB_IMAGE_SRC})
find_package(OpenGL REQUIRED)
target_link_libraries(Snake-Game PRIVATE glfw ${OPENGL_gl_LIBRARY})
target_link_libraries(Snake-Game PRIVATE freetype)
target_link_libraries(Snake-Game PRIVATE irrKlang)

# Specify where to find header files
target_include_directories(Snake-Game PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb-image/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype-2.13.3/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/irrKlang/include
)

# Specify where to find libraries
target_link_directories(Snake-Game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/irrKlang/lib
)

set_property(TARGET Snake-Game PROPERTY CXX_STANDARD 20)

# Command that moves 'irrKlang.dll' copy in the same dir where game executable located 
# otherwise .exe fails to run
add_custom_command(TARGET Snake-Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/external/irrKlang/lib/irrKlang.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/out/build/Debug
)
