set(RENDERER Renderer)
file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/include/Common/*.h
    ${PROJECT_SOURCE_DIR}/include/RenderSystem/*.h
    ${PROJECT_SOURCE_DIR}/include/Engine/*.h
    *.h
    *.cpp
    RenderSystem/*.h
    RenderSystem/*.cpp
    RenderSystem/imgui/*.h
    RenderSystem/imgui/*.cpp
    Engine/*.cpp
)
add_library(${RENDERER} SHARED ${SRC_FILES})
target_precompile_headers(${RENDERER} PRIVATE ${PROJECT_SOURCE_DIR}/include/Common/pch.h)

target_include_directories(${RENDERER}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
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

