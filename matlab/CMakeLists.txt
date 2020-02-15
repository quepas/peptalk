cmake_minimum_required( VERSION 3.0 )

project( peptalk_matlab VERSION 1.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )

find_package( Matlab )

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )

#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
#find_package(PAPI)

include_directories( ${Matlab_INCLUDE_DIRS} ${PAPI_INCLUDE_DIRS} )
link_directories( ${PAPI_LIBRARY_DIRS} )

matlab_add_mex( NAME peptalk_profiling_init SRC src/peptalk_profiling_init.cpp LINK_TO papi peptalk_core )
matlab_add_mex( NAME peptalk_profiling_start SRC src/peptalk_profiling_start.cpp LINK_TO papi peptalk_core )
matlab_add_mex( NAME peptalk_profiling_stop SRC src/peptalk_profiling_stop.cpp LINK_TO papi peptalk_core )
