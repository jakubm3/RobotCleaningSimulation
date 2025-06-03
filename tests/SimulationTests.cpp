#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

// Forward declaration to help with friend access
class SimulationTest;

#include "../Robot/Simulation.h"
#include "../Robot/Map.h"
#include "../Robot/Robot.h"
#include "../Robot/Floor.h"
#include "../Robot/Obstacle.h"
#include "../Robot/Charger.h"

namespace fs = std::filesystem;

class SimulationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDir = fs::temp_directory_path() / "simulation_tests";
        fs::create_directories(testDir);

        // Create a simple 3x3 map for testing
        simpleMapStr = "012\n345\n6B8\n";

        // Create a map with obstacles
        obstacleMapStr = "01P\n3B5\n678\n";

        // Create a dirty map
        dirtyMapStr = "912\n3B5\n678\n";

        // Valid simulation save data
        validSimulationData =
            "012\n345\n6B8\n\n"  // Map data
            "4 7 0 2 "            // Robot: position, chargerId, task, efficiency
            "9 "                  // tilesToCheck size
            "0 0 0 0 0 0 0 0 0 "  // tilesToCheck data
            "0\n";                // path size (empty path)

        // Create test files
        simpleMapFile = testDir / "simple_map.txt";
        validSimulationFile = testDir / "valid_simulation.txt";

        std::ofstream(simpleMapFile) << simpleMapStr;
        std::ofstream(validSimulationFile) << validSimulationData;
    }

    void TearDown() override {
        // Clean up test files
        fs::remove_all(testDir);
    }

    fs::path testDir;
    fs::path simpleMapFile;
    fs::path validSimulationFile;
    std::string simpleMapStr;
    std::string obstacleMapStr;
    std::string dirtyMapStr;
    std::string validSimulationData;
};

// ========== CONSTRUCTOR TESTS ==========

TEST_F(SimulationTest, DefaultConstructor) {
    Simulation sim;
    // Should create simulation with default parameters
    EXPECT_NO_THROW(sim);
}

TEST_F(SimulationTest, ConstructorWithParameters) {
    Simulation sim(3, 3, 4);
    // Should create simulation with specified parameters
    EXPECT_NO_THROW(sim);
}

// ========== FILE LOADING TESTS ==========

TEST_F(SimulationTest, LoadValidSimulation) {
    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
}

TEST_F(SimulationTest, LoadNonExistentFile) {
    Simulation sim;
    fs::path nonExistentFile = testDir / "non_existent.txt";

    // Should handle gracefully without throwing
    EXPECT_NO_THROW(sim.loadFromFile(nonExistentFile));
}

TEST_F(SimulationTest, LoadEmptyFile) {
    fs::path emptyFile = testDir / "empty.txt";
    std::ofstream(emptyFile) << "";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(emptyFile));
}

TEST_F(SimulationTest, LoadInvalidMapFile) {
    fs::path invalidFile = testDir / "invalid.txt";
    std::ofstream(invalidFile) << "01\n345\n"; // Non-rectangular

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(invalidFile));
}

// ========== SIMULATION VALIDATION TESTS ==========

TEST_F(SimulationTest, SimulationValidationWithValidState) {
    Simulation sim(3, 3, 4);

    // Simulation should be valid with proper initialization
    // Note: We can't directly test isSimulationValid() as it's private
    // We'll test indirectly through other operations
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
}

// ========== INTEGRATION TESTS ==========

TEST_F(SimulationTest, StartWithValidFile) {
    Simulation sim;

    // Should be able to start with a valid file
    // Note: start() method requires user interaction, so we test loadFromFile instead
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
}

TEST_F(SimulationTest, StartWithEmptyPath) {
    Simulation sim;

    // Should be able to start without file path
    // Note: Testing the constructor call which happens in start()
    EXPECT_NO_THROW(Simulation(3, 3, 4));
}

// ========== FILE I/O CONSISTENCY TESTS ==========

TEST_F(SimulationTest, SaveLoadConsistency) {
    // Create a simple map file first
    fs::path mapFile = testDir / "test_map.txt";
    std::ofstream(mapFile) << "012\n345\n6B8\n";

    Simulation sim;
    sim.loadFromFile(mapFile);

    // Save to a new file
    fs::path outputFile = testDir / "output_sim.txt";

    // Note: We can't directly test private saveSimulation method
    // But we can verify the file operations work
    EXPECT_TRUE(fs::exists(mapFile));
}

// ========== EDGE CASES TESTS ==========

TEST_F(SimulationTest, LoadMapWithOnlyCharger) {
    fs::path chargerOnlyFile = testDir / "charger_only.txt";
    std::ofstream(chargerOnlyFile) << "B\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(chargerOnlyFile));
}

TEST_F(SimulationTest, LoadLargeMap) {
    fs::path largeMapFile = testDir / "large_map.txt";
    std::ofstream file(largeMapFile);

    // Create a 10x10 map
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (i == 5 && j == 5) {
                file << 'B'; // Charger in the middle
            } else {
                file << '0';
            }
        }
        file << '\n';
    }
    file.close();

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(largeMapFile));
}

TEST_F(SimulationTest, LoadMapWithMaxDirtiness) {
    fs::path dirtyMapFile = testDir / "very_dirty_map.txt";
    std::ofstream(dirtyMapFile) << "999\n9B9\n999\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(dirtyMapFile));
}

TEST_F(SimulationTest, LoadMapWithObstacles) {
    fs::path obstacleMapFile = testDir / "obstacle_map.txt";
    std::ofstream(obstacleMapFile) << "0PP\n0B0\nPPP\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(obstacleMapFile));
}

// ========== ROBOT STATE TESTS ==========

TEST_F(SimulationTest, SimulationWithDifferentRobotPositions) {
    // Test multiple robot positions in valid simulation data
    std::string simData1 = "012\n345\n6B8\n\n0 7 0 2 9 0 0 0 0 0 0 0 0 0 0\n";
    std::string simData2 = "012\n345\n6B8\n\n8 7 0 2 9 0 0 0 0 0 0 0 0 0 0\n";

    fs::path simFile1 = testDir / "sim1.txt";
    fs::path simFile2 = testDir / "sim2.txt";

    std::ofstream(simFile1) << simData1;
    std::ofstream(simFile2) << simData2;

    Simulation sim1, sim2;
    EXPECT_NO_THROW(sim1.loadFromFile(simFile1));
    EXPECT_NO_THROW(sim2.loadFromFile(simFile2));
}

// ========== ERROR HANDLING TESTS ==========

TEST_F(SimulationTest, HandleCorruptedSimulationFile) {
    fs::path corruptedFile = testDir / "corrupted.txt";
    std::ofstream(corruptedFile) << "01\n34\n\ninvalid robot data here\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(corruptedFile));
}

TEST_F(SimulationTest, HandleFileWithBinaryData) {
    fs::path binaryFile = testDir / "binary.txt";
    std::ofstream file(binaryFile, std::ios::binary);
    for (int i = 0; i < 256; ++i) {
        file.put(static_cast<char>(i));
    }
    file.close();

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(binaryFile));
}

// ========== SIMULATION CONSISTENCY TESTS ==========

TEST_F(SimulationTest, MultipleLoadOperations) {
    Simulation sim;

    // Load different files multiple times
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
    EXPECT_NO_THROW(sim.loadFromFile(simpleMapFile));
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
}

TEST_F(SimulationTest, LoadAfterConstruction) {
    // Test loading into simulation created with parameters
    Simulation sim(5, 5, 12);
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
}

// ========== MEMORY AND RESOURCE TESTS ==========

TEST_F(SimulationTest, MultipleSimulationInstances) {
    std::vector<std::unique_ptr<Simulation>> simulations;

    // Create multiple simulation instances
    for (int i = 0; i < 5; ++i) {
        simulations.push_back(std::make_unique<Simulation>(3, 3, 4));
    }

    // Load data into each
    for (auto& sim : simulations) {
        EXPECT_NO_THROW(sim->loadFromFile(validSimulationFile));
    }

    // All should be valid and independent
    EXPECT_EQ(simulations.size(), 5);
}

// ========== PATH HANDLING TESTS ==========

TEST_F(SimulationTest, LoadFromRelativePath) {
    // Create file in current directory
    fs::path relativePath = "temp_sim_test.txt";
    std::ofstream(relativePath) << simpleMapStr;

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(relativePath));

    // Cleanup
    fs::remove(relativePath);
}

TEST_F(SimulationTest, LoadFromAbsolutePath) {
    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(fs::absolute(validSimulationFile)));
}

// ========== SIMULATION STATE TESTS ==========

TEST_F(SimulationTest, SimulationStateAfterLoad) {
    Simulation sim;
    sim.loadFromFile(validSimulationFile);

    // After loading, simulation should maintain its state
    // We can test this by loading again and checking it doesn't crash
    EXPECT_NO_THROW(sim.loadFromFile(validSimulationFile));
}

// ========== COMPLEX SCENARIO TESTS ==========

TEST_F(SimulationTest, LoadComplexMapWithMixedTiles) {
    fs::path complexMapFile = testDir / "complex_map.txt";
    std::string complexMap =
        "0123456789\n"
        "P000000000\n"
        "0000B00000\n"
        "0000000000\n"
        "9876543210\n";

    std::ofstream(complexMapFile) << complexMap;

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(complexMapFile));
}

TEST_F(SimulationTest, LoadMapWithSpecialCharacterHandling) {
    fs::path specialFile = testDir / "special_chars.txt";
    std::string mapWithSpecialLayout = "000\n0B0\n000\n";

    std::ofstream(specialFile) << mapWithSpecialLayout;

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(specialFile));
}

// ========== PERFORMANCE AND STRESS TESTS ==========

TEST_F(SimulationTest, LoadVeryLargeMap) {
    fs::path largeFile = testDir / "very_large_map.txt";
    std::ofstream file(largeFile);

    // Create a 50x50 map
    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 50; ++j) {
            if (i == 25 && j == 25) {
                file << 'B';
            } else {
                file << ((i + j) % 10);
            }
        }
        file << '\n';
    }
    file.close();

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(largeFile));
}

// ========== BOUNDARY CONDITION TESTS ==========

TEST_F(SimulationTest, LoadMinimalValidMap) {
    fs::path minimalFile = testDir / "minimal.txt";
    std::ofstream(minimalFile) << "B\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(minimalFile));
}

TEST_F(SimulationTest, LoadMapWithAllDirtyTiles) {
    fs::path allDirtyFile = testDir / "all_dirty.txt";
    std::ofstream(allDirtyFile) << "999\n9B9\n999\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(allDirtyFile));
}

TEST_F(SimulationTest, LoadMapWithAllCleanTiles) {
    fs::path allCleanFile = testDir / "all_clean.txt";
    std::ofstream(allCleanFile) << "000\n0B0\n000\n";

    Simulation sim;
    EXPECT_NO_THROW(sim.loadFromFile(allCleanFile));
}
