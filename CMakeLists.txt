cmake_minimum_required(VERSION 3.6)
project(RTDestruction)

set(CMAKE_CXX_STANDARD 11)

set(MY_TARGET RTDestruction)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
# set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(SOURCE_FILES 
main.cpp
include/PhysicsModule.h
src/PhysicsModule.cpp
src/Scene.cpp
include/Scene.h
include/Actor.h
src/Actor.cpp
include/Component.h
src/Component.cpp
include/Model.h
src/Model.cpp
include/Types.h
include/AssetLoader.h
src/AssetLoader.cpp
include/RenderModule.h
src/RenderModule.cpp 
include/Functions.h 
include/Application.h
src/Application.cpp
include/Module.h
src/Module.cpp
include/Manager.h
src/Manager.cpp


)
add_executable(${MY_TARGET} ${SOURCE_FILES} )

find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
# Add the required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_include_directories(${MY_TARGET} PUBLIC ./include PRIVATE ./ThirdParty/blast/_build/windows-x86_64/${name}/blast-sdk/include PRIVATE ${OPENGL_INCLUDE_DIR})
# Link the required libraries
target_link_libraries(${MY_TARGET} PUBLIC ./ThirdParty/blast/_build/windows-x86_64/${name}/blast-sdk/bin/*.lib 
PRIVATE ${OPENGL_LIBRARIES}  GLEW::GLEW glfw imgui::imgui unofficial::omniverse-physx-sdk::sdk assimp::assimp)

if(TARGET unofficial::omniverse-physx-sdk::gpu-library)
if(UNIX)
    # Add rpath setting to find .so libraries on unix based systems
    set_target_properties(${MY_TARGET} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
    )
endif()
add_custom_command(TARGET ${MY_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-library>
                $<TARGET_FILE_DIR:${MY_TARGET}>)
if(WIN32)
    add_custom_command(TARGET ${MY_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-device-library>
                    $<TARGET_FILE_DIR:${MY_TARGET}>)
endif()
else()
message(WARNING "GPU acceleration library target not defined - GPU acceleration will NOT be available!")
endif()