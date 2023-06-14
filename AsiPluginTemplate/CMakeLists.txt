cmake_minimum_required(VERSION 3.13)
project(AsiPluginTemplate VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED "main.cpp" "main.hpp")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
