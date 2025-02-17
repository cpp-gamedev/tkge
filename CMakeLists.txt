cmake_minimum_required(VERSION 3.24)

set(project_name Tkge)
project(${project_name})

option(TKGE_BUILD_TESTS "Build Tkge unit tests" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

add_subdirectory(Ext)

add_subdirectory(Lib)
add_subdirectory(App)

if(TKGE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif()
