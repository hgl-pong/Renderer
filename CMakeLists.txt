cmake_minimum_required(VERSION 3.6)
project(Renderer)

set(CMAKE_CXX_STANDARD 11)

set(MY_TARGET Renderer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
# set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

file (GLOB SOURCE_FILES 
main.cpp
include/*.h
src/*.cpp
)
add_executable(${MY_TARGET} ${SOURCE_FILES} )

# Add the required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_include_directories(${MY_TARGET} PUBLIC ./include PRIVATE ${OPENGL_INCLUDE_DIR})
# Link the required libraries
target_link_libraries(${MY_TARGET} PRIVATE ${OPENGL_LIBRARIES}  GLEW::GLEW glfw imgui::imgui assimp::assimp)
