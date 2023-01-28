#stb_image
add_library(stb_image INTERFACE)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stb_image
  INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
