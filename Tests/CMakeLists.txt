project(${project_name}-Tests)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "*.cpp")

if(NOT "${sources}" STREQUAL "")
  add_executable(${PROJECT_NAME})

  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${project_name}::Lib
    klib::klib-test-main
  )

  target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}" ,
    ../Lib/Src
  )

  target_sources(${PROJECT_NAME} PRIVATE ${sources})

  add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
endif()
