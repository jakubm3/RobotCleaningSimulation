cmake_minimum_required(VERSION 3.14)
project(RobotTests)

# Find required packages
find_package(GTest REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIRS})

# Test executables
add_executable(MapTests
    MapTests.cpp
)

add_executable(RobotTests
    RobotTests.cpp
)

add_executable(SimulationTests
    SimulationTests.cpp
)

add_executable(TileTests
    TileTests.cpp
)

add_executable(FileManagerTests
    FileManagerTests.cpp
)

# Link libraries
target_link_libraries(MapTests
    RobotLib
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    pthread
)

target_link_libraries(RobotTests
    RobotLib
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    pthread
)

target_link_libraries(SimulationTests
    RobotLib
    GTest::GTest
    GTest::Main
)

target_link_libraries(TileTests
    RobotLib
    GTest::GTest
    GTest::Main
)

target_link_libraries(FileManagerTests
    RobotLib
    GTest::GTest
    GTest::Main
)

# Register tests
add_test(NAME MapTests COMMAND MapTests)
add_test(NAME RobotTests COMMAND RobotTests)
add_test(NAME SimulationTests COMMAND SimulationTests)
add_test(NAME TileTests COMMAND TileTests)
add_test(NAME FileManagerTests COMMAND FileManagerTests)

# Optional: Add more specific tests
gtest_discover_tests(MapTests)
gtest_discover_tests(RobotTests)
gtest_discover_tests(SimulationTests)
gtest_discover_tests(TileTests)
gtest_discover_tests(FileManagerTests)

# Create combined test executable
add_executable(AllTests
    MapTests.cpp
    RobotTests.cpp
    SimulationTests.cpp
    TileTests.cpp
    FileManagerTests.cpp
)

target_link_libraries(AllTests
    RobotLib
    GTest::GTest
    GTest::Main
)

add_test(NAME AllTests COMMAND AllTests)
gtest_discover_tests(AllTests)
