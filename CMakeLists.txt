# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VulkanRenderer" C CXX)

# Include sub-projects.
add_subdirectory ("VulkanRenderer")
add_subdirectory ("GUI")
add_subdirectory ("3pp")
