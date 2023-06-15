project(TestPlugin VERSION 1.0.0)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0091 NEW) 

add_library(${PROJECT_NAME} SHARED "main.cpp" "main.hpp")

target_link_libraries(${PROJECT_NAME} PRIVATE ASIPLUGIN::BASE)

set_target_properties(${PROJECT_NAME} 
                        PROPERTIES
                            SUFFIX ".ASI"
)