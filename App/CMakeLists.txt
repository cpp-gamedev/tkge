project(${project_name}-app)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${project_name}::lib
)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM ${project_name}-lib)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "src/*.[hc]pp")
target_sources(${PROJECT_NAME} PRIVATE
  ${sources}
)
