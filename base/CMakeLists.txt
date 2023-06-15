project(asibase VERSION 1.0.0)

set(PUBLIC_HEADER 
    "include/patch.hpp"
    "include/snippets.hpp"
    "include/SAMP.hpp")
set(SOURCES 
    "src/patch.cpp"
    "src/snippets.cpp"
    "src/SAMP.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${PUBLIC_HEADER})
add_library(ASIPLUGIN::BASE ALIAS ${PROJECT_NAME})

find_package(PolyHook_2 CONFIG REQUIRED)
find_package(Boost COMPONENTS Signals2)
target_link_libraries(${PROJECT_NAME} PUBLIC cyanide::cyanide)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set_property(TARGET ${PROJECT_NAME}
                PROPERTY 
                    CXX_STANDARD 17
)
