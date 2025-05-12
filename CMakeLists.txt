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

# Specify where to find header files
target_include_directories(Snake-Game PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb-image/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype-2.13.3/include
)

set_property(TARGET Snake-Game PROPERTY CXX_STANDARD 20)
