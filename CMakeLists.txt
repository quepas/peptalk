cmake_minimum_required( VERSION 3.0 )

project( pep-talk VERSION 1.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/ )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib )

add_subdirectory( third-party/pybind11 )

add_subdirectory( io )
add_subdirectory( core )
add_subdirectory( example )
add_subdirectory( pep2csv )
set( Matlab_ROOT_DIR /usr/local/MATLAB/R2018b )
add_subdirectory( matlab )
add_subdirectory( python )
