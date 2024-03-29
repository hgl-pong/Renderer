cmake_minimum_required(VERSION 3.6)
set(PROJECT_NAME Engine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# 编译ImGui库
file(GLOB IMGUI_SOURCES "${PROJECT_SOURCE_DIR}/ThirdParty/imgui/*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/ThirdParty/imgui")

# 找到所需的包
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# 添加src子目录
add_subdirectory(src)
add_subdirectory(tests)

# 编译主程序的可执行文件
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}ThirdParty/imgui
    PRIVATE
    ${OPENGL_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME} 
	PRIVATE 
	${RENDERER}
	imgui 
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
    TBB::tbb
    TBB::tbbmalloc
    TBB::tbbmalloc_proxy
)