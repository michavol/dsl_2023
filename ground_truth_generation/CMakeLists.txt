# FILEPATH: /home/mike/Masters_DS/dsl_2023_all/dsl_2023/ground_truth_generation/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

project(ground_truth_generation)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add the executable
add_executable(droplet_simulation droplet_simulation.cpp)

#add_executable(droplet_simulation_2 droplet_simulation_2.cpp)
# # Link any necessary libraries
# target_link_libraries(data_generation ${REQUIRED_LIBRARIES})
