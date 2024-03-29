﻿cmake_minimum_required(VERSION 3.6)
project(Renderer)

set(CMAKE_CXX_STANDARD 20)

set(MY_TARGET Renderer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
file(GLOB SOURCE_FILES
    main.cpp
    include/*.h
    include/Common/*.h
    include/RenderSystem/*.h
    include/Engine/*.h
    src/*.h
    src/*.cpp
    src/RenderSystem/*.h
    src/RenderSystem/*.cpp
    src/RenderSystem/imgui/*.h
    src/RenderSystem/imgui/*.cpp
    src/Engine/*.cpp
)
add_executable(${MY_TARGET} ${SOURCE_FILES})
target_precompile_headers(${MY_TARGET} PRIVATE include/Common/pch.h)

file(GLOB IMGUI_SOURCES ThirdParty/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})

# Add the required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

target_include_directories(${MY_TARGET}
    PUBLIC
    ./include
    ./src
    ThirdParty/imgui
    PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

# Link the required libraries
target_link_libraries(${MY_TARGET}
    PRIVATE
    ${OPENGL_LIBRARIES}
    Vulkan::Vulkan
    GLEW::GLEW
    imgui
    assimp::assimp
    Eigen3::Eigen
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    soil2
    rapidjson
)
