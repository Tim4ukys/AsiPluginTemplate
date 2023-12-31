# AsiPluginTemplate cmake file
# Authors: tim4ukys
# url: https://github.com/Tim4ukys/AsiPluginTemplate
# tg: https://t.me/tim4ukys
# Do not delete this comment block. Respect others' work!

project(asibase VERSION 1.0.0)

set(PUBLIC_HEADER 
    "include/patch.hpp"
    "include/snippets.hpp"
    "include/SAMP.hpp"
    "include/config.hpp"
    "include/hookD3D9.hpp"
    "include/hookWndProc.hpp"
)
set(SOURCES 
    "src/patch.cpp"
    "src/snippets.cpp"
    "src/SAMP.cpp" 
    "src/hookD3D9.cpp" 
    "src/hookWndProc.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${PUBLIC_HEADER})
add_library(ASIPLUGIN::BASE ALIAS ${PROJECT_NAME})

set(DIRECTX9_PATH $ENV{DXSDK_DIR})

find_package(Boost COMPONENTS Signals2)
target_link_libraries(${PROJECT_NAME} 
    PUBLIC cyanide::cyanide 
    PUBLIC "d3d9.lib"
    PUBLIC "d3dx9.lib"
)

target_link_directories(${PROJECT_NAME}
    PUBLIC "${DIRECTX9_PATH}Lib/x86/"
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PUBLIC "${DIRECTX9_PATH}Include/"
)

set_property(TARGET ${PROJECT_NAME}
                PROPERTY 
                    CXX_STANDARD 17
)
