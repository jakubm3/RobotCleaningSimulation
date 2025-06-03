#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "../Robot/Map.h"
#include "../Robot/Floor.h"
#include "../Robot/Obstacle.h"
#include "../Robot/Charger.h"
#include "../Robot/UnVisited.h"

class MapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple 3x3 map for testing
        simpleMapStr = "012\n345\n6B8\n";

        // Create a map with obstacles
        obstacleMapStr = "0P2\n3B5\n678\n";

        // Invalid maps for testing
        invalidMapNoCharger = "012\n345\n678\n";
        invalidMapMultipleChargers = "0B2\n345\n6B8\n";
        invalidMapNonRectangular = "012\n34\n678\n";
        emptyMap = "";
    }

    std::string simpleMapStr;
    std::string obstacleMapStr;
    std::string invalidMapNoCharger;
    std::string invalidMapMultipleChargers;
    std::string invalidMapNonRectangular;
    std::string emptyMap;
};

// Test default constructor
TEST_F(MapTest, DefaultConstructor) {
    Map map;
    EXPECT_EQ(map.getWidth(), 0);
    EXPECT_EQ(map.getHeight(), 0);
    EXPECT_EQ(map.getSize(), 0);
    EXPECT_FALSE(map.isMapValid());
}

// Test constructor with dimensions
TEST_F(MapTest, DimensionConstructor) {
    Map map(3, 3, 4);

    EXPECT_EQ(map.getWidth(), 3);
    EXPECT_EQ(map.getHeight(), 3);
    EXPECT_EQ(map.getSize(), 9);
    EXPECT_EQ(map.getChargerId(), 4);
    EXPECT_TRUE(map.isMapValid());

    // Check that charger tile exists
    const Tile* chargerTile = map.getTile(4);
    ASSERT_NE(chargerTile, nullptr);
    EXPECT_TRUE(dynamic_cast<const Charger*>(chargerTile) != nullptr);

    // Check that other tiles are floors
    for (size_t i = 0; i < 9; ++i) {
        if (i != 4) {
            const Tile* tile = map.getTile(i);
            ASSERT_NE(tile, nullptr);
            EXPECT_TRUE(dynamic_cast<const Floor*>(tile) != nullptr);
        }
    }
}

// Test stream constructor with valid map
TEST_F(MapTest, StreamConstructorValid) {
    std::istringstream iss(simpleMapStr);
    Map map(iss);

    EXPECT_EQ(map.getWidth(), 3);
    EXPECT_EQ(map.getHeight(), 3);
    EXPECT_EQ(map.getSize(), 9);
    EXPECT_EQ(map.getChargerId(), 7);
    EXPECT_TRUE(map.isMapValid());
}

// Test stream constructor with obstacles
TEST_F(MapTest, StreamConstructorWithObstacles) {
    std::istringstream iss(obstacleMapStr);
    Map map(iss);

    EXPECT_EQ(map.getWidth(), 3);
    EXPECT_EQ(map.getHeight(), 3);
    EXPECT_EQ(map.getChargerId(), 4);
    EXPECT_TRUE(map.isMapValid());

    // Check obstacle tile
    const Tile* obstacleTile = map.getTile(1);
    ASSERT_NE(obstacleTile, nullptr);
    EXPECT_TRUE(dynamic_cast<const Obstacle*>(obstacleTile) != nullptr);
    EXPECT_FALSE(obstacleTile->isMoveValid());
}

// Test invalid maps
TEST_F(MapTest, InvalidMaps) {
    // No charger
    std::istringstream iss1(invalidMapNoCharger);
    EXPECT_THROW(Map map1(iss1), std::runtime_error);

    // Multiple chargers
    std::istringstream iss2(invalidMapMultipleChargers);
    EXPECT_THROW(Map map2(iss2), std::runtime_error);

    // Non-rectangular
    std::istringstream iss3(invalidMapNonRectangular);
    EXPECT_THROW(Map map3(iss3), std::runtime_error);

    // Empty map
    std::istringstream iss4(emptyMap);
    EXPECT_THROW(Map map4(iss4), std::runtime_error);
}

// Test copy constructor
TEST_F(MapTest, CopyConstructor) {
    std::istringstream iss(simpleMapStr);
    Map original(iss);
    Map copy(original);

    EXPECT_EQ(copy.getWidth(), original.getWidth());
    EXPECT_EQ(copy.getHeight(), original.getHeight());
    EXPECT_EQ(copy.getChargerId(), original.getChargerId());
    EXPECT_EQ(copy.getSize(), original.getSize());
    EXPECT_TRUE(copy.isMapValid());

    // Verify tiles are properly copied
    for (size_t i = 0; i < copy.getSize(); ++i) {
        const Tile* originalTile = original.getTile(i);
        const Tile* copiedTile = copy.getTile(i);
        ASSERT_NE(originalTile, nullptr);
        ASSERT_NE(copiedTile, nullptr);
        EXPECT_NE(originalTile, copiedTile); // Different objects
        EXPECT_EQ(originalTile->getId(), copiedTile->getId());
    }
}

// Test copy assignment operator
TEST_F(MapTest, CopyAssignment) {
    std::istringstream iss1(simpleMapStr);
    std::istringstream iss2(obstacleMapStr);
    Map map1(iss1);
    Map map2(iss2);

    map2 = map1;

    EXPECT_EQ(map2.getWidth(), map1.getWidth());
    EXPECT_EQ(map2.getHeight(), map1.getHeight());
    EXPECT_EQ(map2.getChargerId(), map1.getChargerId());
    EXPECT_TRUE(map2.isMapValid());
}

// Test getIndex method
TEST_F(MapTest, GetIndex) {
    std::istringstream iss(simpleMapStr);
    Map map(iss);

    // Test moving up from position 4 (center)
    auto upIndex = map.getIndex(4, Direction::up);
    ASSERT_TRUE(upIndex.has_value());
    EXPECT_EQ(upIndex.value(), 1);

    // Test moving down from position 4
    auto downIndex = map.getIndex(4, Direction::down);
    ASSERT_TRUE(downIndex.has_value());
    EXPECT_EQ(downIndex.value(), 7);

    // Test moving left from position 4
    auto leftIndex = map.getIndex(4, Direction::left);
    ASSERT_TRUE(leftIndex.has_value());
    EXPECT_EQ(leftIndex.value(), 3);

    // Test moving right from position 4
    auto rightIndex = map.getIndex(4, Direction::right);
    ASSERT_TRUE(rightIndex.has_value());
    EXPECT_EQ(rightIndex.value(), 5);

    // Test boundary conditions - moving up from top row
    auto invalidUp = map.getIndex(1, Direction::up);
    EXPECT_FALSE(invalidUp.has_value());

    // Test boundary conditions - moving left from left edge
    auto invalidLeft = map.getIndex(3, Direction::left);
    EXPECT_FALSE(invalidLeft.has_value());

    // Test none direction
    auto noneIndex = map.getIndex(4, Direction::none);
    ASSERT_TRUE(noneIndex.has_value());
    EXPECT_EQ(noneIndex.value(), 4);
}

// Test getTile methods
TEST_F(MapTest, GetTile) {
    std::istringstream iss(simpleMapStr);
    Map map(iss);

    // Test valid index
    const Tile* tile = map.getTile(7);
    ASSERT_NE(tile, nullptr);
    EXPECT_TRUE(dynamic_cast<const Charger*>(tile) != nullptr);

    // Test invalid index
    const Tile* invalidTile = map.getTile(100);
    EXPECT_EQ(invalidTile, nullptr);

    // Test getTile with direction
    const Tile* upTile = map.getTile(4, Direction::up);
    ASSERT_NE(upTile, nullptr);
    EXPECT_EQ(upTile->getId(), 1);

    // Test getTile with invalid direction
    const Tile* invalidDirectionTile = map.getTile(0, Direction::up);
    EXPECT_EQ(invalidDirectionTile, nullptr);
}

// Test canMoveOn method
TEST_F(MapTest, CanMoveOn) {
    std::istringstream iss(obstacleMapStr);
    Map map(iss);

    // Test moving on floor
    EXPECT_TRUE(map.canMoveOn(0));

    // Test moving on charger
    EXPECT_TRUE(map.canMoveOn(4));

    // Test moving on obstacle
    EXPECT_FALSE(map.canMoveOn(1));

    // Test invalid tile
    EXPECT_FALSE(map.canMoveOn(100));
}

// Test updateTile method
TEST_F(MapTest, UpdateTile) {
    Map map(3, 3, 4);

    // Create a dirty floor tile
    Floor dirtyFloor(0, 5);

    // Update tile 0 with dirty floor
    map.updateTile(0, &dirtyFloor);

    const Tile* updatedTile = map.getTile(0);
    ASSERT_NE(updatedTile, nullptr);
    const Floor* floorTile = dynamic_cast<const Floor*>(updatedTile);
    ASSERT_NE(floorTile, nullptr);
    EXPECT_EQ(floorTile->getCleanliness(), 5);
    EXPECT_EQ(floorTile->getId(), 0);

    // Test updating with invalid index
    EXPECT_THROW(map.updateTile(100, &dirtyFloor), std::out_of_range);
}

// Test saveMap and loadMap consistency
TEST_F(MapTest, SaveLoadConsistency) {
    std::istringstream iss(simpleMapStr);
    Map originalMap(iss);

    // Save the map
    std::ostringstream oss;
    originalMap.saveMap(oss);

    // Load it back
    std::istringstream reloadStream(oss.str());
    Map reloadedMap(reloadStream);

    // Compare maps
    EXPECT_EQ(reloadedMap.getWidth(), originalMap.getWidth());
    EXPECT_EQ(reloadedMap.getHeight(), originalMap.getHeight());
    EXPECT_EQ(reloadedMap.getChargerId(), originalMap.getChargerId());
    EXPECT_TRUE(reloadedMap.isMapValid());

    // Compare individual tiles
    for (size_t i = 0; i < originalMap.getSize(); ++i) {
        const Tile* originalTile = originalMap.getTile(i);
        const Tile* reloadedTile = reloadedMap.getTile(i);
        ASSERT_NE(originalTile, nullptr);
        ASSERT_NE(reloadedTile, nullptr);

        // Check if tiles are of the same type
        if (dynamic_cast<const Floor*>(originalTile)) {
            const Floor* originalFloor = dynamic_cast<const Floor*>(originalTile);
            const Floor* reloadedFloor = dynamic_cast<const Floor*>(reloadedTile);
            ASSERT_NE(reloadedFloor, nullptr);
            EXPECT_EQ(reloadedFloor->getCleanliness(), originalFloor->getCleanliness());
        } else if (dynamic_cast<const Charger*>(originalTile)) {
            EXPECT_TRUE(dynamic_cast<const Charger*>(reloadedTile) != nullptr);
        } else if (dynamic_cast<const Obstacle*>(originalTile)) {
            EXPECT_TRUE(dynamic_cast<const Obstacle*>(reloadedTile) != nullptr);
        }
    }
}

// Test map validation
TEST_F(MapTest, MapValidation) {
    // Valid map
    std::istringstream iss1(simpleMapStr);
    Map validMap(iss1);
    EXPECT_TRUE(validMap.isMapValid());

    // Create invalid map manually (this would be difficult to create through normal means)
    Map invalidMap(3, 3, 4);
    // Manually corrupt the map by clearing tiles (this is a bit contrived but tests the validation)
    // We can't easily access private members, so we'll test the public interface
    EXPECT_TRUE(invalidMap.isMapValid()); // Should still be valid
}

// Test with UnVisited tiles (for robot memory)
TEST_F(MapTest, UnVisitedTiles) {
    Map map(2, 2, 0);

    // Update a tile to UnVisited
    UnVisited unvisited(1);
    map.updateTile(1, &unvisited);

    const Tile* tile = map.getTile(1);
    ASSERT_NE(tile, nullptr);
    EXPECT_TRUE(dynamic_cast<const UnVisited*>(tile) != nullptr);
    EXPECT_FALSE(tile->isMoveValid());

    // Test that canMoveOn returns false for UnVisited
    EXPECT_FALSE(map.canMoveOn(1));
}

// Test edge cases for getIndex
TEST_F(MapTest, GetIndexEdgeCases) {
    Map map(1, 1, 0); // Single tile map

    // All directions should be invalid for single tile
    EXPECT_FALSE(map.getIndex(0, Direction::up).has_value());
    EXPECT_FALSE(map.getIndex(0, Direction::down).has_value());
    EXPECT_FALSE(map.getIndex(0, Direction::left).has_value());
    EXPECT_FALSE(map.getIndex(0, Direction::right).has_value());

    // Test with invalid position
    EXPECT_FALSE(map.getIndex(100, Direction::up).has_value());
}

// Test move operations on different tile types
TEST_F(MapTest, MoveOperationsOnDifferentTiles) {
    std::istringstream iss(obstacleMapStr);
    Map map(iss);

    // Floor tile - should be movable
    const Floor* floorTile = dynamic_cast<const Floor*>(map.getTile(0));
    ASSERT_NE(floorTile, nullptr);
    EXPECT_TRUE(floorTile->isMoveValid());
    EXPECT_TRUE(map.canMoveOn(0));

    // Obstacle tile - should not be movable
    const Obstacle* obstacleTile = dynamic_cast<const Obstacle*>(map.getTile(1));
    ASSERT_NE(obstacleTile, nullptr);
    EXPECT_FALSE(obstacleTile->isMoveValid());
    EXPECT_FALSE(map.canMoveOn(1));

    // Charger tile - should be movable
    const Charger* chargerTile = dynamic_cast<const Charger*>(map.getTile(4));
    ASSERT_NE(chargerTile, nullptr);
    EXPECT_TRUE(chargerTile->isMoveValid());
    EXPECT_TRUE(map.canMoveOn(4));
}

// Test output operator
TEST_F(MapTest, OutputOperator) {
    std::istringstream iss(simpleMapStr);
    Map map(iss);

    std::ostringstream oss;
    oss << map;

    std::string output = oss.str();
    EXPECT_FALSE(output.empty());
    EXPECT_TRUE(output.find('B') != std::string::npos); // Should contain charger

    // Count lines
    size_t lineCount = std::count(output.begin(), output.end(), '\n');
    EXPECT_EQ(lineCount, 3); // Should have 3 lines for 3x3 map
}
