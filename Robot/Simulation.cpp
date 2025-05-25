#include "Simulation.h"
#include <string>
#include <iostream>
#include <limits> // For numeric_limits
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <random>     // For random number generation
#include <chrono>     // For seeding random number generator
#include <optional>   // For optional return types
#include <tuple>      // For std::tuple (assuming Robot::makeAction returns a tuple)
#include <vector>     // For std::vector
#include <algorithm>  // For std::shuffle

// Static random device and generator for random tile selection
static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());

// --- Helper functions ---
void clearScreen() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For Linux/macOS
#endif
}

void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    // No need for std::cin.ignore here if all previous inputs use std::getline
    std::cin.get();
}

// Function to get validated size_t input from the user
size_t getValidatedSizeTInput(const std::string& prompt) {
    size_t value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line); // Read the whole line of input

        if (line.empty()) {
            std::cout << "Input cannot be empty. Please enter a positive whole number.\n";
            continue; // Re-prompt if input is empty
        }

        std::stringstream ss(line);
        ss >> value; // Attempt to parse the string to size_t
        // Check for parsing failure or if there are extra characters after the number
        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid input. Please enter a positive whole number.\n";
        }
        else {
            return value; // Return valid input
        }
    }
}

// Function to get validated unsigned int input from the user
unsigned int getValidatedUnsignedIntInput(const std::string& prompt) {
    unsigned int value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line); // Read the whole line of input

        if (line.empty()) {
            std::cout << "Input cannot be empty. Please enter a positive whole number.\n";
            continue; // Re-prompt if input is empty
        }

        std::stringstream ss(line);
        ss >> value; // Attempt to parse the string to unsigned int
        // Check for parsing failure or if there are extra characters after the number
        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid input. Please enter a positive whole number.\n";
        }
        else {
            return value; // Return valid input
        }
    }
}

// Function to get file path input from the user
fs::path getFilePathInput(const std::string& prompt) {
    std::string pathStr;
    std::cout << prompt;
    std::getline(std::cin, pathStr); // Read the entire line for the path
    return fs::path(pathStr);
}

// --- Simulation Option Functions ---

// Validates if the map is properly set up and ready for simulation.
bool Simulation::isSimulationValid() const {
    if (!map.isMapValid()) {
        std::cerr << "Error: The map is not valid for simulation.\n";
        return false;
    }
    std::cout << "Simulation validity check passed.\n";
    return true;
}

// Adds rubbish to a specified tile ID with a given dirtiness level.
void Simulation::addRubbish(size_t tileId, unsigned int dirtiness) {
    std::cout << "Action: Attempting to add " << dirtiness << " rubbish to Tile ID: " << tileId << ".\n";

    Tile* targetTile = map.getTile(tileId);

    if (targetTile) {
        // Dynamically cast to Floor* to check if it's a cleanable tile
        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
        if (floorTile) {
            floorTile->getDirty(dirtiness); // Apply dirtiness
            std::cout << "Rubbish added to Tile " << tileId << ". New cleanliness: " << floorTile->getCleanliness() << ".\n";
        }
        else {
            std::cout << "Failed to add rubbish: Tile ID " << tileId << " is not a Floor tile (it's an Obstacle, Charger, or UnVisited).\n";
        }
    }
    else {
        std::cout << "Failed to add rubbish: Tile ID " << tileId << " does not exist on the map.\n";
    }
}

// NEW: Method to add a specified number of rubbish points randomly on the map to *distinct* tiles.
// NEW: Method to add a specified number of rubbish points randomly on the map to *distinct* tiles.
void Simulation::addSerialRubbish(unsigned int numberOfRubbishPoints) { // maxDirtiness parameter removed
    if (map.getSize() == 0) {
        std::cerr << "Error: Map is empty, cannot add serial rubbish.\n";
        return;
    }

    std::vector<size_t> floorTileIds;
    // Collect all Floor tile IDs
    for (size_t i = 0; i < map.getSize(); ++i) {
        Tile* currentTile = map.getTile(i);
        if (currentTile && dynamic_cast<Floor*>(currentTile)) {
            floorTileIds.push_back(i);
        }
    }

    if (floorTileIds.empty()) {
        std::cerr << "Error: No Floor tiles found on the map to add rubbish.\n";
        return;
    }

    // Ensure we don't try to add more rubbish points than there are available floor tiles
    if (numberOfRubbishPoints > floorTileIds.size()) {
        std::cout << "Warning: Requested " << numberOfRubbishPoints
            << " rubbish points, but only " << floorTileIds.size()
            << " unique Floor tiles are available. Adding rubbish to all available Floor tiles.\n";
        numberOfRubbishPoints = static_cast<unsigned int>(floorTileIds.size());
    }

    // Shuffle the list of floor tile IDs to pick random unique ones
    std::shuffle(floorTileIds.begin(), floorTileIds.end(), gen);

    std::cout << "Action: Attempting to add " << numberOfRubbishPoints << " rubbish points randomly with dirtiness 1.\n";

    unsigned int successfulAdditions = 0;

    // Iterate through the shuffled list and add rubbish to the first 'numberOfRubbishPoints' tiles
    for (unsigned int i = 0; i < numberOfRubbishPoints; ++i) {
        size_t tileId = floorTileIds[i]; // Get a unique random floor tile ID
        Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId)); // Should always succeed based on how floorTileIds was populated

        if (floorTile) { // Double check, though it should be a Floor tile
            floorTile->getDirty(1); // Always add 1 dirtiness
            std::cout << "  Added 1 rubbish to Tile ID " << tileId << ". New cleanliness: " << floorTile->getCleanliness() << ".\n";
            successfulAdditions++;
        }
    }
    std::cout << "Successfully added " << successfulAdditions << " rubbish points in total.\n";
}


// Changes the robot's current position to a new tile ID.
void Simulation::changeRobotsPosition(size_t newPositionId) {
    robot.setPosition(newPositionId);
    std::cout << "Robot's position successfully changed to Tile ID: " << newPositionId << ".\n";
    const Tile* currentTile = map.getTile(newPositionId);
    if (currentTile) {
        updateRobotMemory(newPositionId, currentTile); // Update robot's memory of the new tile
    }
    else {
        std::cerr << "Warning: Could not retrieve actual tile for ID " << newPositionId << " to update robot memory.\n";
    }
}

// Orders the robot to go back to its home (charger) position.
void Simulation::orderRobotToGoHome() {
    robot.orderToGoHome();
    std::cout << "Robot ordered to go home.\n";
}

// Orders the robot to move to a specific target tile ID.
void Simulation::orderRobotToMove(size_t targetTileId) {
    if (robot.orderToMove(targetTileId)) {
        std::cout << "Robot ordered to move to Tile ID: " << targetTileId << ".\n";
    }
    else {
        std::cout << "Failed to order robot to move to Tile ID: " << targetTileId << ". Path not found or invalid target.\n";
    }
}

// Orders the robot to clean a specific tile ID with a given radius.
void Simulation::orderRobotToClean(size_t tileId, unsigned int radius) {
    if (robot.orderToClean(tileId, radius)) {
        std::cout << "Robot ordered to clean Tile ID: " << tileId << " with radius: " << radius << ".\n";
    }
    else {
        std::cout << "Failed to order robot to clean Tile ID: " << tileId << ". Invalid target or already clean.\n";
    }
}

// Orders the robot to clean efficiently (calls robot's internal efficient cleaning logic).
void Simulation::orderRobotToCleanEfficiently() {
    robot.orderToCleanEfficiently();
    std::cout << "Robot ordered to clean efficiently (based on current position).\n";
}

// Resets the robot's memory of the map.
void Simulation::resetRobotMemory() {
    robot.resetMemory();
    std::cout << "Robot's memory successfully reset.\n";
}

// Saves the current simulation state (map and robot) to a file.
void Simulation::saveSimulation(fs::path filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for saving: " << filePath << std::endl;
        return;
    }

    try {
        map.saveMap(outFile); // Save map data
        outFile << "\n"; // Separator
        robot.saveRobot(outFile); // Save robot data
        std::cout << "Simulation state saved successfully to: " << filePath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error during saving: " << e.what() << std::endl;
    }
    outFile.close();
}

// Loads a simulation state from a specified file.
void Simulation::loadSimulation(fs::path filePath) {
    std::cout << "Action: Loading simulation from file: " << filePath << ".\n";
    loadFromFile(filePath);
}

// Runs the simulation for a specified number of steps.
void Simulation::runSimulation(unsigned int steps) {
    if (steps == 0) {
        std::cout << "No steps to run.\n";
        return;
    }
    std::cout << "Action: Running simulation for " << steps << " steps...\n";

    for (unsigned int i = 0; i < steps; ++i) {
        std::cout << "\n--- Step " << (i + 1) << " ---\n";
        size_t currentRobotPos = robot.getPosition();

        // Update robot's memory with its current tile and its neighbors
        const Tile* currentTile = map.getTile(currentRobotPos);
        if (currentTile) {
            updateRobotMemory(currentRobotPos, currentTile);
        }
        else {
            std::cerr << "Error: Robot at invalid position " << currentRobotPos << " or tile not found on main map. Stopping simulation.\n";
            break;
        }

        Direction directions[] = { Direction::up, Direction::down, Direction::left, Direction::right };

        for (Direction dir : directions) {
            std::optional<size_t> neighborIdOpt = map.getIndex(currentRobotPos, dir);
            if (neighborIdOpt.has_value()) {
                size_t neighborId = neighborIdOpt.value();
                const Tile* neighborTile = map.getTile(neighborId);
                if (neighborTile) {
                    updateRobotMemory(neighborId, neighborTile);
                }
            }
        }
        std::cout << robot; // Print robot's state and its remembered map

        std::tuple<RobotAction, Direction> actionResult;
        try {
            actionResult = robot.makeAction(); // Robot decides its next action
        }
        catch (const std::exception& e) {
            std::cout << "Robot error: " << e.what() << std::endl;
            std::cout << "Simulation finished prematurely after " << (i + 1) << " steps due to robot error.\n";
            return;
        }
        RobotAction action = std::get<0>(actionResult);

        std::cout << "Robot action: ";
        switch (action) {
        case RobotAction::move: std::cout << "Move"; break;
        case RobotAction::clean: std::cout << "Clean"; break;
        case RobotAction::explore: std::cout << "Explore"; break;
        case RobotAction::none: { // Handle RobotAction::none
            std::cout << "None (Robot has completed all known tasks).\n";
            std::cout << "Robot has completed all known exploration and cleaning tasks.\n";
            std::cout << "Do you want to order the robot to clean efficiently (y/N)? ";
            std::string response;
            std::getline(std::cin, response); // Read user response

            if (!response.empty() && (response[0] == 'y' || response[0] == 'Y')) {
                robot.orderToCleanEfficiently();
                std::cout << "Robot ordered to clean efficiently. Continuing simulation.\n";
                // After ordering, the robot's internal state (currTask, path) might change.
                // The next iteration of the loop will call makeAction() again,
                // which will then execute the newly ordered task if any.
            }
            else {
                std::cout << "Simulation finished.\n";
                return;
            }
            break; // Break from switch, continue for loop
        }
        default: std::cout << "Unknown"; break;
        }
        std::cout << ".\n";

        if (action == RobotAction::clean) {
            cleanTile(robot.getPosition(), robot.getCleaningEfficiency()); // Clean the tile if robot decided to clean
        }
    }
    std::cout << "Simulation finished after " << steps << " steps.\n";
}

// Exits the simulation application.
void Simulation::exitSimulation() {
    std::cout << "Exiting simulation. Goodbye!\n";
}

// Prints the current state of the simulation (map and robot).
void Simulation::printSimulation() {
    std::cout << "\n--- Current Simulation State ---\n";
    std::cout << "Map State:\n";
    std::cout << map; // Prints the main map
    std::cout << "\nRobot State:\n";
    std::cout << robot; // Prints the robot's remembered map and state
    std::cout << "--------------------------------\n";
}

// Updates the robot's internal memory with information about a tile.
void Simulation::updateRobotMemory(size_t tileId, const Tile* tileObj) {
    robot.exploreTile(tileId, tileObj);
}

// Cleans a specific tile on the main map with a given efficiency.
void Simulation::cleanTile(size_t tileId, unsigned int efficiency) {
    std::cout << "Internal: Attempting to clean Tile ID: " << tileId << " with efficiency: " << efficiency << ".\n";

    Tile* targetTile = map.getTile(tileId);

    if (targetTile) {
        // Dynamically cast to Floor* to check if it's a cleanable tile
        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
        if (floorTile) {
            floorTile->getCleaned(efficiency); // Apply cleaning
            std::cout << "Tile " << tileId << " cleaned. New cleanliness: " << floorTile->getCleanliness() << ".\n";
        }
        else {
            std::cout << "Failed to clean: Tile ID " << tileId << " is not a Floor tile (it's an Obstacle, Charger, or UnVisited).\n";
        }
    }
    else {
        std::cout << "Failed to clean: Tile ID " << tileId << " does not exist on the map.\n";
    }
}

// --- The start() function: Main simulation loop and menu ---
void Simulation::start(fs::path filePath) {
    clearScreen();
    std::cout << "---------------------------------------------------\n";
    std::cout << "          WELCOME TO THE ROBOT VACUUM CLEANER        \n";
    std::cout << "                SIMULATION GAME!                   \n";
    std::cout << "---------------------------------------------------\n";
    std::cout << "\n";

    if (!filePath.empty()) {
        std::cout << "Attempting to load initial simulation data from: " << filePath << std::endl;
        loadFromFile(filePath);
        if (isSimulationValid()) {
            std::cout << "Initial simulation state loaded and validated.\n";
        }
        else {
            std::cerr << "Initial simulation state is invalid. Proceeding with caution or resetting.\n";
        }
        pressEnterToContinue();
    }
    else {
        std::cout << "Starting new simulation (no file loaded).\n";
        pressEnterToContinue();
    }

    bool running = true;
    while (running) {
        clearScreen();
        printSimulation(); // Always show current state at the start of each loop iteration

        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Add Rubbish to a Tile (single)\n"; // Renamed for clarity
        std::cout << "2. Add Rubbish to Multiple Random Tiles (serial)\n"; // NEW MENU OPTION
        std::cout << "3. Change Robot's Position\n";
        std::cout << "4. Order Robot to Go Home\n";
        std::cout << "5. Order Robot to Move to a Tile\n";
        std::cout << "6. Order Robot to Clean a Tile\n";
        std::cout << "7. Reset Robot's Memory\n";
        std::cout << "8. Run Simulation Steps\n";
        std::cout << "9. Save Current Simulation State\n";
        std::cout << "10. Load Simulation from File\n";
        std::cout << "11. Order Robot to Clean Efficiently\n";
        std::cout << "0. Exit Simulation\n";
        std::cout << "Enter your choice: ";

        std::string choiceStr;
        std::getline(std::cin, choiceStr); // Read the whole line for the menu choice

        if (choiceStr.empty()) {
            // If user just pressed Enter, clear screen and re-prompt immediately
            continue; // Skip the rest of the loop and start over
        }

        int choice;
        std::stringstream ss(choiceStr);
        ss >> choice;

        // Validate if parsing was successful and no extra characters were left
        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid input. Please enter a number from the menu.\n";
            pressEnterToContinue();
            continue; // Re-prompt
        }

        switch (choice) {
        case 1: { // Add Rubbish (single)
            std::string input;
            size_t tileId;
            bool foundFloorTile = false; // Flag to indicate if a Floor tile was found

            std::cout << "Enter Tile ID to add rubbish to (or 'r' for random Floor tile): ";
            std::getline(std::cin, input); // Use getline to read whole line, allows "r"

            if (input.empty()) { // Handle empty input for add rubbish
                std::cout << "Input for Tile ID cannot be empty.\n";
                break; // Exit this case and go back to main menu
            }

            if (input == "r" || input == "R") {
                if (map.getSize() == 0) {
                    std::cerr << "Error: Map is empty, cannot add rubbish to a random tile.\n";
                    break; // Exit this case
                }

                // Loop to find a random Floor tile
                std::uniform_int_distribution<size_t> distrib(0, map.getSize() - 1);
                const int maxAttempts = 100; // Safeguard: max attempts to find a floor tile
                int attempts = 0;

                while (!foundFloorTile && attempts < maxAttempts) {
                    tileId = distrib(gen); // Generate random ID
                    Tile* targetTile = map.getTile(tileId);

                    if (targetTile) {
                        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
                        if (floorTile) {
                            foundFloorTile = true; // Found a Floor tile!
                        }
                    }
                    attempts++;
                }

                if (foundFloorTile) {
                    std::cout << "Selected random Floor Tile ID: " << tileId << ".\n";
                }
                else {
                    std::cerr << "Could not find a random Floor tile after " << maxAttempts << " attempts. Please check map configuration.\n";
                    break; // Exit this case if no Floor tile found
                }

            }
            else { // User entered a specific tile ID
                try {
                    tileId = std::stoul(input); // Convert string to size_t
                    // Validate if it's a Floor tile if user explicitly entered an ID
                    Tile* targetTile = map.getTile(tileId);
                    if (!targetTile || !dynamic_cast<Floor*>(targetTile)) {
                        std::cerr << "Invalid input: Tile ID " << tileId << " is not a valid Floor tile or does not exist.\n";
                        break; // Exit this case
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Invalid input for Tile ID. Please enter a valid number or 'r'.\n";
                    break; // Exit this case
                }
            }
            unsigned int dirtiness = getValidatedUnsignedIntInput("Enter dirtiness level (e.g., 1-9): ");
            addRubbish(tileId, dirtiness); // Call the existing addRubbish with the determined tileId
            break;
        }
        case 2: { // NEW: Add Rubbish to Multiple Random Tiles (serial)
            unsigned int numPoints = getValidatedUnsignedIntInput("Enter number of rubbish points to add: ");
            unsigned int maxDirtiness = getValidatedUnsignedIntInput("Enter maximum dirtiness level for each point (e.g., 1-9): ");
            addSerialRubbish(numPoints);
            break;
        }
        case 3: { // Change Robot's Position (shifted from 2)
            size_t newPositionId = getValidatedSizeTInput("Enter new Tile ID for robot's position: ");
            changeRobotsPosition(newPositionId);
            break;
        }
        case 4: { // Order Robot to Go Home (shifted from 3)
            orderRobotToGoHome();
            break;
        }
        case 5: { // Order Robot to Move (shifted from 4)
            size_t targetTileId = getValidatedSizeTInput("Enter target Tile ID for robot to move: ");
            orderRobotToMove(targetTileId);
            break;
        }
        case 6: { // Order Robot to Clean (shifted from 5)
            size_t tileId = getValidatedSizeTInput("Enter Tile ID for robot to clean: ");
            unsigned int radius = getValidatedUnsignedIntInput("Enter cleaning radius: ");
            orderRobotToClean(tileId, radius);
            break;
        }
        case 7: { // Reset Robot's Memory (shifted from 6)
            resetRobotMemory();
            break;
        }
        case 8: { // Run Simulation Steps (shifted from 7)
            unsigned int steps = getValidatedUnsignedIntInput("Enter number of simulation steps to run: ");
            runSimulation(steps);
            break;
        }
        case 9: { // Save Simulation (shifted from 8)
            fs::path savePath = getFilePathInput("Enter filename to save simulation (e.g., my_sim.txt): ");
            saveSimulation(savePath);
            break;
        }
        case 10: { // Load Simulation (shifted from 9)
            fs::path loadPath = getFilePathInput("Enter filename to load simulation from (e.g., other_sim.txt): ");
            loadSimulation(loadPath);
            break;
        }
        case 11: { // Order Robot to Clean Efficiently (shifted from 10)
            orderRobotToCleanEfficiently();
            break;
        }
        case 0: { // Exit Simulation
            exitSimulation();
            running = false;
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }

        // Only prompt to continue if the simulation is still running
        if (running) {
            pressEnterToContinue();
        }
    }

    std::cout << "Thank you for playing the Robot Vacuum Cleaner Simulation!\n";
}

// --- loadFromFile implementation ---
void Simulation::loadFromFile(fs::path filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::stringstream mapDataStream;
    std::stringstream robotDataStream;
    std::string line;
    bool readingMap = true;

    // Read file content into stringstreams
    while (std::getline(inputFile, line)) {
        if (readingMap) {
            if (line.empty()) { // Empty line separates map from robot data
                readingMap = false;
            }
            else {
                mapDataStream << line << "\n";
            }
        }
        else {
            robotDataStream << line << "\n";
        }
    }
    inputFile.close(); // Close the file after reading all content

    try {
        map.loadMap(mapDataStream); // Load map data from its stringstream
        std::cout << "Map data loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading map data: " << e.what() << std::endl;
        std::cerr << "Map loading failed. Cannot proceed with robot initialization. Returning.\n";
        return;
    }

    if (robotDataStream.str().empty()) {
        std::cout << "Robot data not found in file. Initializing robot with map dimensions and charger ID.\n";
        // Initialize robot with default values derived from the loaded map
        robot = Robot(map.getWidth(), map.getHeight(), map.getChargerId());

        std::cout << "Robot initialized with: Width=" << map.getWidth()
            << ", Height=" << map.getHeight()
            << ", ChargerID=" << map.getChargerId() << ".\n";
    }
    else {
        try {
            robot.loadRobot(robotDataStream); // Load robot data from its stringstream
            std::cout << "Robot data loaded successfully." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading robot data: " << e.what() << std::endl;
            std::cerr << "Robot loading failed. Re-initializing robot with map dimensions and charger ID as a fallback.\n";
            // Fallback: re-initialize robot if loading fails
            robot = Robot(map.getWidth(), map.getHeight(), map.getChargerId());
        }
    }
}