project(${project_name}-App)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${project_name}::Lib
)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM ${project_name}-lib)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "Src/*.[hc]pp")
target_sources(${PROJECT_NAME} PRIVATE
  ${sources}
)
