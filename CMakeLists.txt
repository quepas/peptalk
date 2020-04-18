cmake_minimum_required( VERSION 3.0 )

project( pep-talk VERSION 1.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )

add_subdirectory( io )
add_subdirectory( core )
set( Matlab_ROOT_DIR /usr/local/MATLAB/R2018b )
add_subdirectory( matlab )
