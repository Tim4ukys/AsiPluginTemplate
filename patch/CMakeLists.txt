project(patch VERSION 1.1.0)

set(PUBLIC_HEADER "include/patch.hpp")
set(SOURCES "src/patch.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(PATCH::PATCH ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set_property(TARGET ${PROJECT_NAME}
				PROPERTY 
					CXX_STANDARD 17
)
