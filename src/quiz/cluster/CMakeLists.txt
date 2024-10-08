cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Set C++ standard to C++17
add_definitions(-std=c++17)

# Enable all warnings
set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

# Add compiler flags for debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Set optimization level to none for debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

project(playback)

# Find the PCL package
find_package(PCL 1.2 REQUIRED)

# Find the Boost package with filesystem component required
find_package(Boost REQUIRED COMPONENTS filesystem)

# Include directories for PCL and Boost
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PCL_INCLUDE_DIRS})
include_directories(../../render)
include_directories(../..)

# Link directories for PCL
link_directories(${PCL_LIBRARY_DIRS})

# Add definitions from PCL
add_definitions(${PCL_DEFINITIONS})

# Remove potentially problematic PCL library
list(REMOVE_ITEM PCL_LIBRARIES "vtkproj4")

# Add source files to the executable
add_executable (cluster2D cluster2D.cpp ../../render/render.cpp)
add_executable (cluster3D cluster3D.cpp ../../render/render.cpp)

target_link_libraries (cluster2D ${PCL_LIBRARIES})
target_link_libraries (cluster3D ${PCL_LIBRARIES})




