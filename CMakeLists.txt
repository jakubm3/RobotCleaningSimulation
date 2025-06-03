cmake_minimum_required(VERSION 3.14)
project(RobotCleaningProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Find required packages
find_package(GTest REQUIRED)

# Include directories
include_directories(Robot)

# Create library for Robot classes
add_library(RobotLib
    Robot/Tile.cpp
    Robot/Floor.cpp
    Robot/Obstacle.cpp
    Robot/Charger.cpp
    Robot/UnVisited.cpp
    Robot/Map.cpp
    Robot/Robot.cpp
    Robot/Simulation.cpp
)

# Main executable
add_executable(RobotMain
    Robot/Main.cpp
    Robot/Simulation.cpp
)

target_link_libraries(RobotMain RobotLib)

# Add subdirectory for tests
add_subdirectory(tests)
