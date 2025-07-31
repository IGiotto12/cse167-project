cmake_minimum_required(VERSION 3.10)

# Project name and C++ standard
project(RayTracer CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Homebrew path
execute_process(COMMAND brew --prefix OUTPUT_VARIABLE BREWPATH OUTPUT_STRIP_TRAILING_WHITESPACE)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wno-deprecated-register -Wno-deprecated-declarations -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED")

# Set source directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)

# Gather source files
file(GLOB_RECURSE SRC_FILES 
    "main.cpp"
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/scenes/*.inl"  # Ensure inlined files are tracked
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Include directories
include_directories(${INC_DIR} ${BREWPATH}/include)

# Define executable
add_executable(RayTracer ${SRC_FILES})

# Link required libraries
target_link_libraries(RayTracer 
    "-framework OpenGL"
    "-framework GLUT"
    ${BREWPATH}/lib/libfreeimage.dylib
)

# Create obj directory for intermediate compiled objects
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/obj)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
