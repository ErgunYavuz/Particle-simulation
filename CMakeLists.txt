cmake_minimum_required(VERSION 3.20)
project(verletIntegration)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Add executable
add_executable(verletIntegration main.cpp Particle.cpp simulation.cpp)

# Set SFML's directories and libraries
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64") # Path to the MSYS2 MinGW64 directory

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Link SFML libraries
target_link_libraries(verletIntegration sfml-graphics sfml-window sfml-system)
target_link_libraries(verletIntegration sfml-graphics sfml-window sfml-system winmm)