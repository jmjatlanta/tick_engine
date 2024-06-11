cmake_minimum_required(VERSION 3.25 )
cmake_policy(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)

project ( tick_engine_tests )

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

add_executable( tick_engine_tests 
    TickReaderTest.cpp
    TickWriterTest.cpp
)

target_include_directories(tick_engine_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(tick_engine_tests 
    tick
    GTest::gtest_main
)

