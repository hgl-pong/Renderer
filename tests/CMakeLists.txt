set(UNIT_TEST UnitTest)

file(GLOB_RECURSE TEST_FILES
    *.cpp
)

add_executable(${UNIT_TEST} ${TEST_FILES})
target_include_directories(${UNIT_TEST}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/ThirdParty/HMath/include
    ./
)

target_link_libraries(${UNIT_TEST}
    PRIVATE
    Renderer
    GTest::gtest 
    GTest::gtest_main 
    GTest::gmock 
    GTest::gmock_main
) 

enable_testing()
add_test(AllTestsInMain main)