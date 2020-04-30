cmake_minimum_required( VERSION 3.0 )

project( pep-talk-python VERSION 1.0 )

pybind11_add_module( peptalk main.cpp )
target_link_libraries( peptalk PRIVATE peptalk_core )
