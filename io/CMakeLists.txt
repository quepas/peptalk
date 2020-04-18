cmake_minimum_required( VERSION 3.0 )

project( peptalk_io VERSION 1.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )

add_library( peptalk_io SHARED src/PEPReader.cpp src/PEPWriter.cpp )
target_include_directories( peptalk_io PUBLIC include/ )
