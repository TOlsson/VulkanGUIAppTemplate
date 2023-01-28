# CMake for GUI
find_package(Vulkan REQUIRED)
find_library(TARGET 3pp)
find_library(TARGET stb_image)

file(GLOB GUI_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Input/*.cpp" 
  "${CMAKE_CURRENT_SOURCE_DIR}/Input/*.h"
)
add_library(GUI ${GUI_FILES})

set_target_properties(GUI PROPERTIES LINKER_LANGUAGE CXX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GUI PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(GUI 
  PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${Vulkan_INCLUDE_DIRS}
    ${3pp_INCLUDE_DIRS}
)

#Vulkan
target_link_libraries(GUI
  PUBLIC
    ${Vulkan_LIBRARIES}
    stb_image
    3pp
)