cmake_minimum_required(VERSION 3.25 )
cmake_policy(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)

project ( tick_cli )

find_package(Boost 1.69 COMPONENTS log log_setup REQUIRED)

add_executable( tick_cli
    main.cpp
)

target_include_directories(tick_cli PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ib_helper/src/ib_helper
    ${IB_CLIENT_DIR}
    ${Boost_INCLUDE_DIR}
)

target_link_libraries(tick_cli
    tick
    ib_helper
    ib_api
    dfp
    Boost::log
    Boost::log_setup
)

