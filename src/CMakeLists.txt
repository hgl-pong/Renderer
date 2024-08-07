set(RENDERER Renderer)
file(GLOB SRC_FILES
    *.cpp
    RenderSystem/*.cpp
    RenderSystem/imgui/*.cpp
    RenderSystem/DX11/*
    Engine/*.cpp
)
add_library(${RENDERER} STATIC ${SRC_FILES})
target_precompile_headers(${RENDERER} PRIVATE ${PROJECT_SOURCE_DIR}/include/Common/pch.h)

target_include_directories(${RENDERER}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}ThirdParty/imgui
	${PROJECT_SOURCE_DIR}/ThirdParty/HMath/include
    PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

# Link the required libraries
target_link_libraries(${RENDERER}
    PRIVATE
    ${OPENGL_LIBRARIES}
    Vulkan::Vulkan
    GLEW::GLEW
    imgui
    glfw
    assimp::assimp
    Eigen3::Eigen 
    rapidjson
) 

