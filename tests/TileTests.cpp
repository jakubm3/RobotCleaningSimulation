#include <gtest/gtest.h>
#include <memory>
#include "../Robot/Tile.h"
#include "../Robot/Floor.h"
#include "../Robot/Obstacle.h"
#include "../Robot/Charger.h"
#include "../Robot/UnVisited.h"

// Test base Tile class functionality
class TileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup common test data
    }

    void TearDown() override {
        // Cleanup
    }
};

// Tests for Floor class
class FloorTest : public ::testing::Test {
protected:
    void SetUp() override {
        cleanFloor = std::make_unique<Floor>(0, 0);
        dirtyFloor = std::make_unique<Floor>(1, 5);
        maxDirtyFloor = std::make_unique<Floor>(2, 9);
    }

    std::unique_ptr<Floor> cleanFloor;
    std::unique_ptr<Floor> dirtyFloor;
    std::unique_ptr<Floor> maxDirtyFloor;
};

// Tests for Obstacle class
class ObstacleTest : public ::testing::Test {
protected:
    void SetUp() override {
        obstacle = std::make_unique<Obstacle>(10);
    }

    std::unique_ptr<Obstacle> obstacle;
};

// Tests for Charger class
class ChargerTest : public ::testing::Test {
protected:
    void SetUp() override {
        charger = std::make_unique<Charger>(20);
    }

    std::unique_ptr<Charger> charger;
};

// Tests for UnVisited class
class UnVisitedTest : public ::testing::Test {
protected:
    void SetUp() override {
        unvisited = std::make_unique<UnVisited>(30);
    }

    std::unique_ptr<UnVisited> unvisited;
};

// ========== FLOOR TESTS ==========

TEST_F(FloorTest, ConstructorWithValidCleanliness) {
    Floor floor(5, 3);
    EXPECT_EQ(floor.getId(), 5);
    EXPECT_EQ(floor.getCleanliness(), 3);
}

TEST_F(FloorTest, ConstructorWithInvalidCleanliness) {
    Floor floor(5, 15); // Should be capped at 9
    EXPECT_EQ(floor.getCleanliness(), 9);
}

TEST_F(FloorTest, DefaultConstructor) {
    Floor floor;
    EXPECT_EQ(floor.getCleanliness(), 0);
}

TEST_F(FloorTest, GetCleanlinessInitialValues) {
    EXPECT_EQ(cleanFloor->getCleanliness(), 0);
    EXPECT_EQ(dirtyFloor->getCleanliness(), 5);
    EXPECT_EQ(maxDirtyFloor->getCleanliness(), 9);
}

TEST_F(FloorTest, SetCleanliness) {
    cleanFloor->setCleanliness(7);
    EXPECT_EQ(cleanFloor->getCleanliness(), 7);
}

TEST_F(FloorTest, SetCleanlinessOverLimit) {
    cleanFloor->setCleanliness(15); // Should be capped at 9
    EXPECT_EQ(cleanFloor->getCleanliness(), 9);
}

TEST_F(FloorTest, IsDirtyWhenClean) {
    EXPECT_FALSE(cleanFloor->isDirty());
}

TEST_F(FloorTest, IsDirtyWhenDirty) {
    EXPECT_TRUE(dirtyFloor->isDirty());
    EXPECT_TRUE(maxDirtyFloor->isDirty());
}

TEST_F(FloorTest, GetDirtyIncreasesCleanliness) {
    unsigned int initialCleanliness = cleanFloor->getCleanliness();
    cleanFloor->getDirty(3);
    EXPECT_EQ(cleanFloor->getCleanliness(), initialCleanliness + 3);
}

TEST_F(FloorTest, GetDirtyCapAtMaximum) {
    cleanFloor->getDirty(15); // Should be capped at 9
    EXPECT_EQ(cleanFloor->getCleanliness(), 9);
}

TEST_F(FloorTest, GetDirtyFromAlreadyDirty) {
    dirtyFloor->getDirty(2);
    EXPECT_EQ(dirtyFloor->getCleanliness(), 7);
}

TEST_F(FloorTest, GetDirtyExceedingMaximum) {
    dirtyFloor->getDirty(10); // 5 + 10 = 15, should be capped at 9
    EXPECT_EQ(dirtyFloor->getCleanliness(), 9);
}

TEST_F(FloorTest, GetCleanedReducesCleanliness) {
    dirtyFloor->getCleaned(2);
    EXPECT_EQ(dirtyFloor->getCleanliness(), 3);
}

TEST_F(FloorTest, GetCleanedCompletelyCleans) {
    dirtyFloor->getCleaned(10); // More than current cleanliness
    EXPECT_EQ(dirtyFloor->getCleanliness(), 0);
}

TEST_F(FloorTest, GetCleanedExactAmount) {
    dirtyFloor->getCleaned(5); // Exact amount
    EXPECT_EQ(dirtyFloor->getCleanliness(), 0);
}

TEST_F(FloorTest, GetCleanedAlreadyClean) {
    cleanFloor->getCleaned(5);
    EXPECT_EQ(cleanFloor->getCleanliness(), 0); // Should remain 0
}

TEST_F(FloorTest, IsMoveValidAlwaysTrue) {
    EXPECT_TRUE(cleanFloor->isMoveValid());
    EXPECT_TRUE(dirtyFloor->isMoveValid());
    EXPECT_TRUE(maxDirtyFloor->isMoveValid());
}

TEST_F(FloorTest, CloneCreatesCorrectCopy) {
    auto cloned = dirtyFloor->clone();
    Floor* clonedFloor = dynamic_cast<Floor*>(cloned.get());

    ASSERT_NE(clonedFloor, nullptr);
    EXPECT_EQ(clonedFloor->getCleanliness(), dirtyFloor->getCleanliness());
    EXPECT_EQ(clonedFloor->getId(), dirtyFloor->getId());
}

// ========== OBSTACLE TESTS ==========

TEST_F(ObstacleTest, ConstructorSetsId) {
    EXPECT_EQ(obstacle->getId(), 10);
}

TEST_F(ObstacleTest, DefaultConstructor) {
    Obstacle defaultObstacle;
    EXPECT_EQ(defaultObstacle.getId(), 0);
}

TEST_F(ObstacleTest, IsMoveValidAlwaysFalse) {
    EXPECT_FALSE(obstacle->isMoveValid());
}

TEST_F(ObstacleTest, CloneCreatesCorrectCopy) {
    auto cloned = obstacle->clone();
    Obstacle* clonedObstacle = dynamic_cast<Obstacle*>(cloned.get());

    ASSERT_NE(clonedObstacle, nullptr);
    EXPECT_EQ(clonedObstacle->getId(), obstacle->getId());
}

// ========== CHARGER TESTS ==========

TEST_F(ChargerTest, ConstructorSetsId) {
    EXPECT_EQ(charger->getId(), 20);
}

TEST_F(ChargerTest, DefaultConstructor) {
    Charger defaultCharger;
    EXPECT_EQ(defaultCharger.getId(), 0);
}

TEST_F(ChargerTest, IsMoveValidAlwaysTrue) {
    EXPECT_TRUE(charger->isMoveValid());
}

TEST_F(ChargerTest, CloneCreatesCorrectCopy) {
    auto cloned = charger->clone();
    Charger* clonedCharger = dynamic_cast<Charger*>(cloned.get());

    ASSERT_NE(clonedCharger, nullptr);
    EXPECT_EQ(clonedCharger->getId(), charger->getId());
}

// ========== UNVISITED TESTS ==========

TEST_F(UnVisitedTest, ConstructorSetsId) {
    EXPECT_EQ(unvisited->getId(), 30);
}

TEST_F(UnVisitedTest, DefaultConstructor) {
    UnVisited defaultUnvisited;
    EXPECT_EQ(defaultUnvisited.getId(), 0);
}

TEST_F(UnVisitedTest, IsMoveValidAlwaysFalse) {
    EXPECT_FALSE(unvisited->isMoveValid());
}

TEST_F(UnVisitedTest, CloneCreatesCorrectCopy) {
    auto cloned = unvisited->clone();
    UnVisited* clonedUnvisited = dynamic_cast<UnVisited*>(cloned.get());

    ASSERT_NE(clonedUnvisited, nullptr);
    EXPECT_EQ(clonedUnvisited->getId(), unvisited->getId());
}

// ========== TILE BASE CLASS TESTS ==========

TEST_F(TileTest, TileIdOperations) {
    Floor floor(42, 0);
    EXPECT_EQ(floor.getId(), 42);

    floor.setId(100);
    EXPECT_EQ(floor.getId(), 100);
}

TEST_F(TileTest, InvalidIdConstant) {
    EXPECT_EQ(Tile::INVALID_ID, static_cast<size_t>(-1));
}

// ========== POLYMORPHISM TESTS ==========

TEST_F(TileTest, PolymorphicBehavior) {
    std::vector<std::unique_ptr<Tile>> tiles;
    tiles.push_back(std::make_unique<Floor>(0, 3));
    tiles.push_back(std::make_unique<Obstacle>(1));
    tiles.push_back(std::make_unique<Charger>(2));
    tiles.push_back(std::make_unique<UnVisited>(3));

    // Test polymorphic behavior
    EXPECT_TRUE(tiles[0]->isMoveValid());  // Floor
    EXPECT_FALSE(tiles[1]->isMoveValid()); // Obstacle
    EXPECT_TRUE(tiles[2]->isMoveValid());  // Charger
    EXPECT_FALSE(tiles[3]->isMoveValid()); // UnVisited
}

TEST_F(TileTest, ClonePolymorphism) {
    std::unique_ptr<Tile> originalFloor = std::make_unique<Floor>(5, 7);
    auto clonedTile = originalFloor->clone();

    // Should be able to cast back to Floor
    Floor* clonedFloor = dynamic_cast<Floor*>(clonedTile.get());
    ASSERT_NE(clonedFloor, nullptr);
    EXPECT_EQ(clonedFloor->getCleanliness(), 7);
}

// ========== EDGE CASE TESTS ==========

TEST_F(FloorTest, EdgeCaseZeroDirtiness) {
    cleanFloor->getDirty(0);
    EXPECT_EQ(cleanFloor->getCleanliness(), 0);
}

TEST_F(FloorTest, EdgeCaseZeroEfficiency) {
    dirtyFloor->getCleaned(0);
    EXPECT_EQ(dirtyFloor->getCleanliness(), 5); // Should remain unchanged
}

TEST_F(FloorTest, EdgeCaseMaxDirtiness) {
    Floor floor(0, 9);
    floor.getDirty(1);
    EXPECT_EQ(floor.getCleanliness(), 9); // Should remain at max
}

TEST_F(FloorTest, RepeatedOperations) {
    Floor floor(0, 0);

    // Make dirty, then clean, repeatedly
    for (int i = 0; i < 5; ++i) {
        floor.getDirty(3);
        EXPECT_EQ(floor.getCleanliness(), 3);

        floor.getCleaned(3);
        EXPECT_EQ(floor.getCleanliness(), 0);
    }
}

// ========== TYPE IDENTIFICATION TESTS ==========

TEST_F(TileTest, TypeIdentificationWithDynamicCast) {
    std::unique_ptr<Tile> floor = std::make_unique<Floor>(0, 5);
    std::unique_ptr<Tile> obstacle = std::make_unique<Obstacle>(1);
    std::unique_ptr<Tile> charger = std::make_unique<Charger>(2);
    std::unique_ptr<Tile> unvisited = std::make_unique<UnVisited>(3);

    EXPECT_NE(dynamic_cast<Floor*>(floor.get()), nullptr);
    EXPECT_EQ(dynamic_cast<Obstacle*>(floor.get()), nullptr);

    EXPECT_NE(dynamic_cast<Obstacle*>(obstacle.get()), nullptr);
    EXPECT_EQ(dynamic_cast<Floor*>(obstacle.get()), nullptr);

    EXPECT_NE(dynamic_cast<Charger*>(charger.get()), nullptr);
    EXPECT_EQ(dynamic_cast<Floor*>(charger.get()), nullptr);

    EXPECT_NE(dynamic_cast<UnVisited*>(unvisited.get()), nullptr);
    EXPECT_EQ(dynamic_cast<Floor*>(unvisited.get()), nullptr);
}
