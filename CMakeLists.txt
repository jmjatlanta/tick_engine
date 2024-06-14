cmake_minimum_required(VERSION 3.25 )
cmake_policy(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
add_compile_options("-DBOOST_LOG_DYN_LINK")

project ( tick_engine_root )

add_subdirectory(ib_helper)
add_subdirectory(src)
add_subdirectory(test)

