file(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_SOURCE_DIR}/Src.zip" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(Src/kvf)
add_subdirectory(Src/djson)
