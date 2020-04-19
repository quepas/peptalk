cmake_minimum_required( VERSION 3.0 )

project( pep2csv VERSION 1.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )

add_executable( pep2csv main.cpp )
target_link_libraries( pep2csv peptalk_io )
