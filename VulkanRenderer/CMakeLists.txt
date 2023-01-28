find_library(TARGET GUI)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  if (DEFINED WL_DIST)
    add_definitions(-DWL_DIST)
  endif()
endif()
add_definitions(-DWL_PLATFORM_WINDOWS)



# Add source to this project's executable.
add_executable (VulkanRenderer 
  "VulkanRenderer.cpp" "VulkanRenderer.h"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanRenderer PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(${PROJECT_NAME}
  GUI
)