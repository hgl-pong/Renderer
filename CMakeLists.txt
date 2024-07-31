cmake_minimum_required(VERSION 3.6)
set(PROJECT_NAME Engine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
# 全局设置使用动态运行时库
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/executable/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# 编译ImGui库
file(GLOB IMGUI_SOURCES "${PROJECT_SOURCE_DIR}/ThirdParty/imgui/*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/ThirdParty/imgui")

# 找到所需的包
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# 添加src子目录
add_subdirectory(src)
add_subdirectory(tests)

# 设置shader编译脚本的路径
set(SHADER_COMPILE_SCRIPT ${CMAKE_SOURCE_DIR}/shaders/CompileGLSL.bat)

# 添加一个自定义命令来运行shader编译脚本
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders_compiled
    COMMAND ${SHADER_COMPILE_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/shaders
    COMMENT "Compiling GLSL shaders to SPIR-V"
)

# 添加一个自定义目标，依赖于上面的自定义命令
add_custom_target(compile_shaders ALL
    DEPENDS ${CMAKE_BINARY_DIR}/shaders_compiled
)

# # 编译主程序的可执行文件
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/ThirdParty/HMath/include
)
target_link_libraries(${PROJECT_NAME} 
	PRIVATE 
	Renderer
	Eigen3::Eigen  
	rapidjson
)