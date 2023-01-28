# 3pp

#imgui
file(GLOB 
  IMGUI_SRC 
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp" 
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backend/imgui_impl_vulkan.*" 
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backend/imgui_impl_glfw.*"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backend/vulkan/*"
)

# GLFW
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

# Dear ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# Libraries
find_package(Vulkan REQUIRED)
set(LIBRARIES "glfw;Vulkan::Vulkan")

# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)

add_library(3pp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
  ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
  ${IMGUI_DIR}/imgui.cpp 
  ${IMGUI_DIR}/imgui_draw.cpp 
  ${IMGUI_DIR}/imgui_demo.cpp 
  ${IMGUI_DIR}/imgui_tables.cpp 
  ${IMGUI_DIR}/imgui_widgets.cpp
)

target_include_directories(3pp
  PUBLIC
    ${IMGUI_DIR}
    ${GLFW_DIR}/include
)

target_link_libraries(3pp ${LIBRARIES})
set_target_properties(3pp PROPERTIES LINKER_LANGUAGE CXX)

#stb_image
add_subdirectory("stb_image")