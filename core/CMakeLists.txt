cmake_minimum_required( VERSION 3.0 )

project( peptalk_core VERSION 1.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )

add_library( peptalk_core SHARED src/counting.cpp src/profiling.cpp src/timing.cpp )
target_include_directories( peptalk_core PUBLIC include/ )
target_link_libraries( peptalk_core peptalk_io papi stdc++fs )
set_property( TARGET peptalk_core PROPERTY POSITION_INDEPENDENT_CODE ON )

add_library( peptalk_core_static STATIC src/counting.cpp src/profiling.cpp src/timing.cpp )
target_include_directories( peptalk_core_static PUBLIC include/ )
target_link_libraries( peptalk_core_static peptalk_io_static papi stdc++fs )
set_property( TARGET peptalk_core_static PROPERTY POSITION_INDEPENDENT_CODE ON )
