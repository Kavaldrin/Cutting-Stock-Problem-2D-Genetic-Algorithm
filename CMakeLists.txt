cmake_minimum_required(VERSION 3.10)
project(Zad1)


add_definitions("-Wall -Werror -O2 -pedantic-errors -fpermissive")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(GALIB REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include
		    ${FUNC_INCLUDE}
		   ${GALIB_INCLUDE_DIR})
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cxx)


add_executable(Zad1 ${SRC_FILES} )
target_link_libraries(Zad1 ${GALIB_LIBRARIES})

set_target_properties(Zad1 PROPERTIES
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED YES)
