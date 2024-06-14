cmake_minimum_required(VERSION 3.25 )
cmake_policy(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)

project ( tick_engine )

add_subdirectory(cli)

add_library( tick_engine
    TickReader.cpp
    TickWriter.cpp
    TickCoordinator.cpp
    BarBuilder.cpp
)

target_include_directories(tick_engine PRIVATE 
)

target_link_libraries(tick_engine 
)

