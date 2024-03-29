set(RENDERER Renderer)
file(GLOB SRC_FILES
    *.cpp
    RenderSystem/*.cpp
    RenderSystem/imgui/*.cpp
    Engine/*.cpp
)
# add_library(${RENDERER} SHARED ${SRC_FILES})
add_executable(${RENDERER} ${SRC_FILES} ${PROJECT_SOURCE_DIR}/main.cpp)

target_precompile_headers(${RENDERER} PRIVATE ${PROJECT_SOURCE_DIR}/include/Common/pch.h)

target_include_directories(${RENDERER}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}ThirdParty/imgui
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

