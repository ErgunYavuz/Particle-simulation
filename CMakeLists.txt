cmake_minimum_required(VERSION 3.20)
project(verletIntegration)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Add executable
add_executable(${PROJECT_NAME}
        main.cpp
        Particle.cpp
        simulation.cpp
        render.cpp
        uniformGrid.cpp
)

# Conditional path setting based on platform
if(WIN32)
    # Windows-specific configuration
    set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64") # Path to the MSYS2 MinGW64 directory
    # Find SFML package
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
    # Link SFML libraries with winmm for Windows
    target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system winmm)
else()
    # Explicitly set Linux SFML paths and ignore any Windows paths
    unset(SFML_DIR CACHE)
    unset(SFML_ROOT CACHE)
    set(CMAKE_PREFIX_PATH "")

    # Find SFML package using the system-installed version
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

    # Link against the Linux SFML libraries
    target_link_libraries(${PROJECT_NAME}
            sfml-graphics
            sfml-window
            sfml-system
    )

    # Optional: Add include directories
    # target_include_directories(${PROJECT_NAME} PRIVATE /usr/include)

    # Optional: Set additional compiler flags
    # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

endif()