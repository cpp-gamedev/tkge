project(${project_name}-Lib)

add_library(${PROJECT_NAME})
add_library(${project_name}::lib ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
  Include
)

target_include_directories(${PROJECT_NAME} PRIVATE
  rc
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  kvf::kvf

  klib::klib-warnings
  klib::klib-ndwerror
  klib::klib-msbuild-mp
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
  <vulkan/vulkan.hpp>
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "Src/*.[hc]pp")
target_sources(${PROJECT_NAME} PRIVATE
  ${sources})
