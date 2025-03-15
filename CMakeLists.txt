# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.16)

project (StagnoGraphy VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'header' directory so headers can be included by their name
include_directories(header)

# Set the utils directory path
set(UTILS_DIR "${CMAKE_SOURCE_DIR}/utils")

# Collect source files
file(GLOB SOURCE_FILES "source/*.cpp")

add_executable(StagnoGraphy ${SOURCE_FILES})

# Set the output directory for the executable to the 'utils' folder
set_target_properties(StagnoGraphy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${UTILS_DIR}")

# Pass the UTILS_DIR as a preprocessor macro to C++ code
target_compile_definitions(StagnoGraphy PRIVATE UTILS_DIR="${UTILS_DIR}")



