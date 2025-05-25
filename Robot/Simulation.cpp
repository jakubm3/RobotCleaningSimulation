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

void Simulation::addSerialRubbish(unsigned int totalRubbishAmount) {
    if (map.getSize() == 0) {
        std::cerr << "Error: Map is empty, cannot add serial rubbish.\n";
        return;
    }
    if (totalRubbishAmount == 0) {
        std::cout << "No rubbish points to add.\n";
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

    // Use a uniform distribution to pick a random index from the list of floor tile IDs
    std::uniform_int_distribution<size_t> tileIndexDistrib(0, floorTileIds.size() - 1);
    // This distribution will be created dynamically inside the loop based on remaining capacity

    std::cout << "Action: Distributing a total of " << totalRubbishAmount << " rubbish points across random floor tiles.\n";

    unsigned int rubbishPointsDistributed = 0;
    const unsigned int maxAttemptsToFindTile = 100 * floorTileIds.size(); // Safeguard to prevent infinite loops

    while (rubbishPointsDistributed < totalRubbishAmount) {
        int attempts = 0;
        bool tileDirtiedInThisIteration = false;

        while (attempts < maxAttemptsToFindTile && !tileDirtiedInThisIteration) {
            size_t tileId = floorTileIds[tileIndexDistrib(gen)]; // Pick a random floor tile ID
            Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));

            if (floorTile) {
                unsigned int currentCleanliness = floorTile->getCleanliness(); // Assuming 0=clean, 9=max dirty

                // OMIT TILES THAT ARE ALREADY DIRTINESS 9 (MAXIMALLY DIRTY)
                if (currentCleanliness == 9) {
                    attempts++;
                    continue; // Skip this tile, try another random one
                }

                // Calculate the maximum dirtiness this tile can still accept.
                // If cleanliness is 0 (clean), it can accept 9.
                // If cleanliness is 5, it can accept 4.
                // If cleanliness is 8, it can accept 1.
                unsigned int maxDirtinessCanAccept = 9 - currentCleanliness; // This is the UPPER bound for random amount

                // Ensure maxDirtinessCanAccept is at least 1, if it's 0 it means currentCleanliness is 9, already handled above.
                if (maxDirtinessCanAccept == 0) {
                    attempts++; // Should theoretically not be needed due to above 'continue'
                    continue;
                }

                // Create a distribution for the amount of dirtiness to add, from 1 up to maxDirtinessCanAccept.
                // This is the key change to get the random amount between 1 and (9 - current dirtiness)
                std::uniform_int_distribution<unsigned int> dirtinessAmountDistrib(1, maxDirtinessCanAccept);

                // Generate a random amount of dirtiness to try to add
                unsigned int dirtinessToAttempt = dirtinessAmountDistrib(gen);

                // The actual amount to add is limited by:
                // 1. The random amount we just generated (dirtinessToAttempt)
                // 2. The remaining total rubbish points we need to distribute
                unsigned int actualDirtiness = std::min(dirtinessToAttempt, totalRubbishAmount - rubbishPointsDistributed);


                if (actualDirtiness > 0) { // Only proceed if we can actually add some dirtiness
                    floorTile->getDirty(actualDirtiness); // Add the calculated actualDirtiness
                    rubbishPointsDistributed += actualDirtiness; // Decrement from total
                    tileDirtiedInThisIteration = true; // Mark as successful for this loop iteration

                    std::cout << "  Added " << actualDirtiness << " rubbish to Tile ID " << tileId
                        << ". New cleanliness: " << floorTile->getCleanliness()
                        << " (Total distributed: " << rubbishPointsDistributed << "/" << totalRubbishAmount << ").\n";
                }
                else {
                    // This 'else' block means actualDirtiness was 0.
                    // This can happen if totalRubbishAmount - rubbishPointsDistributed is 0 or less,
                    // or if dirtinessToAttempt was 0 (which shouldn't happen with the distribution range 1 to X).
                    // Just try another attempt.
                    attempts++;
                }
            }
            else {
                // This case should ideally not happen if floorTileIds only contains Floor tiles.
                attempts++;
            }
        } // End of inner while (attempts)

        if (!tileDirtiedInThisIteration) {
            std::cerr << "Warning: Could not find a suitable floor tile to add remaining "
                << (totalRubbishAmount - rubbishPointsDistributed)
                << " rubbish points after " << attempts << " attempts. All available floor tiles might be fully dirty or an unexpected error occurred.\n";
            break; // Exit the main loop if we couldn't find a valid tile
        }
    }
    std::cout << "Successfully distributed " << rubbishPointsDistributed << " rubbish points in total.\n";
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
    std::cout << "           WELCOME TO THE ROBOT VACUUM CLEANER        \n";
    std::cout << "                 SIMULATION GAME!                   \n";
    std::cout << "---------------------------------------------------\n";
    std::cout << "\n";

    // Flag to track if a simulation state (including map) is loaded and valid
    bool simulationReady = false;

    // --- Part 1: Handle initial file path argument if provided ---
    if (!filePath.empty()) {
        std::cout << "Attempting to load initial simulation data from: " << filePath << std::endl;
        loadFromFile(filePath); // Call loadFromFile (void return)

        if (isSimulationValid()) { // Then check validity (bool return)
            std::cout << "Initial simulation state loaded and validated.\n";
            simulationReady = true; // Set flag if valid
        }
        else {
            std::cerr << "Initial simulation state is invalid or failed to load from '" << filePath << "'.\n";
            std::cerr << "Please load a valid simulation file to proceed.\n";
            // simulationReady remains false
        }
        pressEnterToContinue();
    }
    else {
        std::cout << "Starting new simulation (no file loaded initially).\n";
        pressEnterToContinue();
    }

    // --- Part 2: Initial menu if no valid simulation is loaded yet ---
    if (!simulationReady) {
        bool initialMenuLoop = true;
        while (initialMenuLoop) {
            clearScreen(); // Clear screen for initial menu
            std::cout << "\n--- Initial Setup ---\n";
            std::cout << "1. Load Simulation from file\n";
            std::cout << "0. Exit Simulation\n";
            std::cout << "Enter your choice: ";

            std::string choiceStr;
            std::getline(std::cin, choiceStr);

            if (choiceStr.empty()) {
                continue; // Re-prompt if empty input
            }

            int choice;
            std::stringstream ss(choiceStr);
            ss >> choice;

            // Input validation for initial menu
            if (ss.fail() || !ss.eof() || (choice != 1 && choice != 0)) {
                std::cout << "Invalid input. Please enter 1 to Load Simulation or 0 to Exit.\n";
                pressEnterToContinue();
                continue;
            }

            switch (choice) {
            case 1: {
                fs::path loadPath = getFilePathInput("Enter simulation file name (e.g., my_sim.txt): ");
                loadFromFile(loadPath); // Call loadFromFile (void return)

                if (isSimulationValid()) { // Then check validity (bool return)
                    std::cout << "Simulation loaded successfully from '" << loadPath << "'!\n";
                    simulationReady = true;    // Simulation is now ready
                    initialMenuLoop = false;   // Exit initial setup loop
                }
                else {
                    std::cerr << "Failed to load simulation from '" << loadPath << "' or loaded state is invalid. Please try again.\n";
                    // simulationReady remains false, initialMenuLoop remains true to re-prompt
                }
                pressEnterToContinue(); // Pause before re-showing initial menu or proceeding
                break;
            }
            case 0:
                // If user exits from initial menu, immediately exit the start function
                std::cout << "Exiting simulation. Goodbye!\n";
                return; // Exit start() directly
            default:
                // Should not be reached due to validation
                break;
            }
        }
    }

    // --- Part 3: Main simulation loop (only accessible if simulationReady is true) ---
    // If we reach here, a simulation is definitely loaded and valid (either via filePath or initial menu)
    bool running = true;
    while (running) {
        clearScreen();
        printSimulation(); // Always show current state at the start of each loop iteration

        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Add Rubbish to a Tile (single)\n";
        std::cout << "2. Add Rubbish to Multiple Random Tiles (serial)\n";
        std::cout << "3. Change Robot's Position\n";
        std::cout << "4. Order Robot to Go Home\n";
        std::cout << "5. Order Robot to Move to a Tile\n";
        std::cout << "6. Order Robot to Clean a Tile\n";
        std::cout << "7. Reset Robot's Memory\n";
        std::cout << "8. Run Simulation Steps\n";
        std::cout << "9. Save Current Simulation State\n";
        std::cout << "10. Load Simulation from File\n"; // This re-loads the entire state
        std::cout << "11. Order Robot to Clean Efficiently\n";
        std::cout << "0. Exit Simulation\n";
        std::cout << "Enter your choice: ";

        std::string choiceStr;
        std::getline(std::cin, choiceStr);

        if (choiceStr.empty()) {
            continue;
        }

        int choice;
        std::stringstream ss(choiceStr);
        ss >> choice;

        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid input. Please enter a number from the menu.\n";
            pressEnterToContinue();
            continue;
        }

        switch (choice) {
        case 1: { // Add Rubbish (single)
            std::string input;
            size_t tileId;
            bool foundValidTile = false;

            std::cout << "Enter Tile ID to add rubbish to (or 'r' for random Floor tile): ";
            std::getline(std::cin, input);

            if (input.empty()) {
                std::cout << "Input for Tile ID cannot be empty.\n";
                break;
            }

            if (input == "r" || input == "R") {
                if (map.getSize() == 0) { // Using 'map' directly as it's a member
                    std::cerr << "Error: Map is empty, cannot add rubbish to a random tile.\n";
                    break;
                }

                std::vector<size_t> floorTileIds;
                for (size_t i = 0; i < map.getSize(); ++i) {
                    if (dynamic_cast<Floor*>(map.getTile(i))) {
                        floorTileIds.push_back(i);
                    }
                }

                if (floorTileIds.empty()) {
                    std::cerr << "Error: No Floor tiles found on the map.\n";
                    break;
                }

                std::uniform_int_distribution<size_t> distrib(0, floorTileIds.size() - 1);
                const int maxAttempts = 100 * floorTileIds.size();
                int attempts = 0;

                while (!foundValidTile && attempts < maxAttempts) {
                    size_t randomIndex = distrib(gen); // Assuming 'gen' is accessible
                    tileId = floorTileIds[randomIndex];
                    Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));

                    if (floorTile && floorTile->getCleanliness() < 9) { // Only pick if not max dirty (cleanliness 9)
                        foundValidTile = true;
                    }
                    attempts++;
                }

                if (foundValidTile) {
                    std::cout << "Selected random Floor Tile ID: " << tileId << ".\n";
                }
                else {
                    std::cerr << "Could not find a cleanable random Floor tile after " << maxAttempts << " attempts. All floor tiles might be fully dirty.\n";
                    break;
                }

            }
            else { // User entered a specific tile ID
                try {
                    tileId = std::stoul(input);
                    Tile* targetTile = map.getTile(tileId);
                    if (!targetTile || !dynamic_cast<Floor*>(targetTile)) {
                        std::cerr << "Invalid input: Tile ID " << tileId << " is not a valid Floor tile or does not exist.\n";
                        break;
                    }
                    if (dynamic_cast<Floor*>(targetTile)->getCleanliness() == 9) { // Check if it's already max dirty
                        std::cerr << "Tile ID " << tileId << " is already at maximum dirtiness (9). Cannot add more rubbish.\n";
                        break;
                    }
                    foundValidTile = true;
                }
                catch (const std::exception& e) {
                    std::cerr << "Invalid input for Tile ID. Please enter a valid number or 'r'.\n";
                    break;
                }
            }

            if (foundValidTile) {
                unsigned int dirtiness = getValidatedUnsignedIntInput("Enter dirtiness level (e.g., 1-9): ");
                Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));
                unsigned int maxAddable = 9 - floorTile->getCleanliness(); // Assuming 0=clean, 9=max dirty
                if (dirtiness > maxAddable) {
                    std::cout << "Warning: Requested dirtiness " << dirtiness << " exceeds tile capacity. Adding " << maxAddable << " instead.\n";
                    dirtiness = maxAddable;
                }
                if (dirtiness > 0) {
                    addRubbish(tileId, dirtiness); // Call your existing addRubbish
                }
                else {
                    std::cout << "Tile already at maximum dirtiness or no dirtiness requested.\n";
                }
            }
            break;
        }
        case 2: { // Add Rubbish to Multiple Random Tiles (serial)
            unsigned int totalRubbishAmount = getValidatedUnsignedIntInput("Enter TOTAL amount of rubbish points to distribute: ");
            addSerialRubbish(totalRubbishAmount); // Call your existing addSerialRubbish
            break;
        }
        case 3: { // Change Robot's Position
            size_t newPositionId = getValidatedSizeTInput("Enter new Tile ID for robot's position: ");
            changeRobotsPosition(newPositionId);
            break;
        }
        case 4: { // Order Robot to Go Home
            orderRobotToGoHome();
            break;
        }
        case 5: { // Order Robot to Move
            size_t targetTileId = getValidatedSizeTInput("Enter target Tile ID for robot to move: ");
            orderRobotToMove(targetTileId);
            break;
        }
        case 6: { // Order Robot to Clean
            size_t tileId = getValidatedSizeTInput("Enter Tile ID for robot to clean: ");
            unsigned int radius = getValidatedUnsignedIntInput("Enter cleaning radius: ");
            orderRobotToClean(tileId, radius);
            break;
        }
        case 7: { // Reset Robot's Memory
            resetRobotMemory();
            break;
        }
        case 8: { // Run Simulation Steps
            unsigned int steps = getValidatedUnsignedIntInput("Enter number of simulation steps to run: ");
            runSimulation(steps);
            break;
        }
        case 9: { // Save Simulation
            fs::path savePath = getFilePathInput("Enter filename to save simulation (e.g., my_sim.txt): ");
            saveSimulation(savePath);
            break;
        }
        case 10: { // Load Simulation from File (this re-loads the entire state)
            fs::path loadPath = getFilePathInput("Enter filename to load simulation from (e.g., other_sim.txt): ");
            loadSimulation(loadPath); // Call loadSimulation (void return)

            if (isSimulationValid()) { // Then check validity (bool return)
                std::cout << "Simulation loaded successfully from '" << loadPath << "'.\n";
                simulationReady = true; // Ensure flag is true after successful load
            }
            else {
                std::cerr << "Failed to load simulation from '" << loadPath << "' or loaded state is invalid.\n";
                // If loading fails from here, the current state remains; simulationReady might be false.
                // Decide if you want to force a return to the initial menu here if load fails.
                // For now, it stays in the main menu.
            }
            break;
        }
        case 11: { // Order Robot to Clean Efficiently
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