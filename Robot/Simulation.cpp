#include "Simulation.h"
#include <string>
#include <iostream>
#include <limits> 
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <random>     
#include <chrono>     
#include <optional>   
#include <tuple>      
#include <vector>     
#include <algorithm>  

// Static random device and generator for random tile selection
static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());

// --- Helper functions ---
void clearScreen() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For Linux
#endif
}

void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

// Function to get validated size_t input from the user
size_t getValidatedSizeTInput(const std::string& prompt) {
    size_t value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        if (line.empty()) {
            std::cout << "Input cannot be empty. Please enter a positive whole number.\n";
            continue;
        }

        std::stringstream ss(line);
        ss >> value;
        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid input. Please enter a positive whole number.\n";
        }
        else {
            return value;
        }
    }
}

// Function to get validated unsigned int input from the user
unsigned int getValidatedUnsignedIntInput(const std::string& prompt) {
    unsigned int value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        if (line.empty()) {
            std::cout << "Input cannot be empty. Please enter a positive whole number.\n";
            continue;
        }

        std::stringstream ss(line);
        ss >> value;
        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid input. Please enter a positive whole number.\n";
        }
        else {
            return value;
        }
    }
}

// Function to get file path input from the user
fs::path getFilePathInput(const std::string& prompt) {
    std::string pathStr;
    std::cout << prompt;
    std::getline(std::cin, pathStr);
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

                unsigned int maxDirtinessCanAccept = 9 - currentCleanliness; // This is the UPPER bound for random amount

                // Ensure maxDirtinessCanAccept is at least 1, if it's 0 it means currentCleanliness is 9, already handled above.
                if (maxDirtinessCanAccept == 0) {
                    attempts++; // Should theoretically not be needed due to above 'continue'
                    continue;
                }

                std::uniform_int_distribution<unsigned int> dirtinessAmountDistrib(1, maxDirtinessCanAccept);

                // Generate a random amount of dirtiness to try to add
                unsigned int dirtinessToAttempt = dirtinessAmountDistrib(gen);

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
        addLog("Attempted to run 0 simulation steps. No action taken."); // Log this edge case
        return;
    }
    std::cout << "Action: Running simulation for " << steps << " steps...\n";
    addLog("Initiating simulation run for " + std::to_string(steps) + " steps.");

    for (unsigned int i = 0; i < steps; ++i) {
        std::cout << "\n--- Step " << (i + 1) << " ---\n";
        addLog("--- Starting Step " + std::to_string(i + 1) + " ---"); // Log start of each step

        size_t currentRobotPos = robot.getPosition();
        addLog("Robot's current position: Tile " + std::to_string(currentRobotPos));

        // Update robot's memory with its current tile and its neighbors
        const Tile* currentTile = map.getTile(currentRobotPos);
        if (currentTile) {
            updateRobotMemory(currentRobotPos, currentTile);
            // FIX: Removed getTypeString() call as it doesn't exist
            addLog("Robot updated memory for current Tile " + std::to_string(currentRobotPos) + ".");
        }
        else {
            std::cerr << "Error: Robot at invalid position " << currentRobotPos << " or tile not found on main map. Stopping simulation.\n";
            addLog("ERROR: Robot at invalid position " + std::to_string(currentRobotPos) + ". Simulation stopped."); // Log error
            break; // Stop simulation if robot's position is invalid
        }

        Direction directions[] = { Direction::up, Direction::down, Direction::left, Direction::right };

        for (Direction dir : directions) {
            std::optional<size_t> neighborIdOpt = map.getIndex(currentRobotPos, dir);
            if (neighborIdOpt.has_value()) {
                size_t neighborId = neighborIdOpt.value();
                const Tile* neighborTile = map.getTile(neighborId);
                if (neighborTile) {
                    updateRobotMemory(neighborId, neighborTile);
                    // Convert direction enum to string for logging
                    std::string dirStr;
                    switch (dir) {
                    case Direction::up:    dirStr = "Up"; break;
                    case Direction::down:  dirStr = "Down"; break;
                    case Direction::left:  dirStr = "Left"; break;
                    case Direction::right: dirStr = "Right"; break;
                    default:               dirStr = "Unknown"; break;
                    }
                    addLog("Robot updated memory for neighbor Tile " + std::to_string(neighborId) + " (" + dirStr + " of current).");
                }
            }
        }
        std::cout << robot; // Print robot's state and its remembered map
        addLog("Robot's remembered map and state printed to console."); // Log that state was printed

        std::tuple<RobotAction, Direction> actionResult;
        try {
            actionResult = robot.makeAction(); // Robot decides its next action
        }
        catch (const std::exception& e) {
            std::cout << "Robot error: " << e.what() << std::endl;
            addLog("Robot encountered an error during makeAction(): " + std::string(e.what())); // Log the error
            std::cout << "Simulation finished prematurely after " << (i + 1) << " steps due to robot error.\n";
            addLog("Simulation finished prematurely after " + std::to_string(i + 1) + " steps due to robot error.");
            return; // End simulation immediately on robot error
        }
        RobotAction action = std::get<0>(actionResult);
        Direction chosenDir = std::get<1>(actionResult); // Get the chosen direction (relevant for 'move')

        std::cout << "Robot action: ";
        std::string actionLogMessage; // To build the log message for the action
        switch (action) {
        case RobotAction::move: {
            std::cout << "Move";
            std::string dirStr;
            switch (chosenDir) {
            case Direction::up:    dirStr = "Up"; break;
            case Direction::down:  dirStr = "Down"; break;
            case Direction::left:  dirStr = "Left"; break;
            case Direction::right: dirStr = "Right"; break;
            default:               dirStr = "Unknown"; break;
            }
            actionLogMessage = "Robot decided to **Move** " + dirStr;
            actionLogMessage += " to Tile " + std::to_string(robot.getPosition());
            break;
        }
        case RobotAction::clean: {
            std::cout << "Clean";
            actionLogMessage = "Robot decided to **Clean** current Tile " + std::to_string(robot.getPosition());
            break;
        }
        case RobotAction::explore: {
            std::cout << "Explore";
            actionLogMessage = "Robot decided to **Explore**.";
            break;
        }
        case RobotAction::none: {
            std::cout << "None (Robot has completed all known tasks).\n";
            actionLogMessage = "Robot has completed all known exploration and cleaning tasks.";
            std::cout << "Robot has completed all known exploration and cleaning tasks.\n";
            std::cout << "Do you want to order the robot to clean efficiently (y/N)? ";
            std::string response;
            std::getline(std::cin, response); // Read user response

            if (!response.empty() && (response[0] == 'y' || response[0] == 'Y')) {
                robot.orderToCleanEfficiently(); // Order the robot to clean efficiently
                std::cout << "Robot ordered to clean efficiently. Continuing simulation.\n";
                actionLogMessage += ". User ordered to **Clean Efficiently**. Continuing simulation.";
            }
            else {
                std::cout << "Simulation finished after " << (i + 1) << " steps.\n";
                actionLogMessage += ". User chose NOT to clean efficiently. Simulation ending.";
                addLog(actionLogMessage); // Log the final decision of ending
                return; // End simulation loop if no more tasks and user doesn't want efficient clean
            }
            break; // Break from switch, continue for loop
        }
        default: {
            std::cout << "Unknown";
            actionLogMessage = "Robot decided on an **Unknown** action.";
            break;
        }
        }
        std::cout << ".\n"; // Closes the "Robot action: " line on console
        // Only log if it's not a 'none' action that already handled its own logging and return.
        if (action != RobotAction::none) {
            addLog(actionLogMessage);
        }

        // --- Execute the decided action if it's 'clean' ---
        if (action == RobotAction::clean) {
            cleanTile(robot.getPosition(), robot.getCleaningEfficiency()); // Clean the tile if robot decided to clean
            addLog("Robot **performed cleaning** on Tile " + std::to_string(robot.getPosition()) +
                " with efficiency " + std::to_string(robot.getCleaningEfficiency()) +
                ". Tile cleanliness updated.");
        }
    }
    std::cout << "Simulation finished after " << steps << " steps.\n";
    addLog("Simulation successfully finished after " + std::to_string(steps) + " steps."); // Final log
}

// Exits the simulation application.
void Simulation::exitSimulation() {
    std::cout << "Exiting simulation\n";
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

void Simulation::addLog(const std::string& message) {
    simulationLogs.push_back(message); // Just store the message directly
}
// --- END NEW HELPER METHOD ---


void Simulation::start(fs::path filePath) {
    // No stream redirection needed here. All output goes to console as normal.

    clearScreen();
    std::cout << "---------------------------------------------------\n";
    std::cout << "           WELCOME TO THE ROBOT VACUUM CLEANER        \n";
    std::cout << "                 SIMULATION GAME!                   \n";
    std::cout << "---------------------------------------------------\n";
    std::cout << "\n";

    addLog("Simulation Started."); // Log start event

    bool simulationReady = false;

    // --- Part 1: Handle initial file path argument if provided ---
    if (!filePath.empty()) {
        std::cout << "Attempting to load initial simulation data from: " << filePath << std::endl;
        addLog("Attempting to load initial simulation from: " + filePath.string());
        loadFromFile(filePath);

        if (isSimulationValid()) {
            std::cout << "Initial simulation state loaded and validated.\n";
            addLog("Initial simulation state loaded and validated successfully.");
            simulationReady = true;
        }
        else {
            std::cerr << "Initial simulation state is invalid or failed to load from '" << filePath << "'.\n";
            std::cerr << "Please load a valid simulation file to proceed.\n";
            addLog("Initial simulation load failed or state is invalid from: " + filePath.string());
        }
        pressEnterToContinue();
    }
    else {
        std::cout << "Starting new simulation (no file loaded initially).\n";
        addLog("Starting new simulation (no initial file provided).");
        pressEnterToContinue();
    }

    // --- Part 2: Initial menu if no valid simulation is loaded yet ---
    if (!simulationReady) {
        bool initialMenuLoop = true;
        while (initialMenuLoop) {
            clearScreen();
            std::cout << "\n--- Initial Setup ---\n";
            std::cout << "1. Load Simulation from file\n";
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

            if (ss.fail() || !ss.eof() || (choice != 1 && choice != 0)) {
                std::cout << "Invalid input. Please enter 1 to Load Simulation or 0 to Exit.\n";
                pressEnterToContinue();
                continue;
            }

            switch (choice) {
            case 1: {
                fs::path loadPath = getFilePathInput("Enter simulation file name (e.g., my_sim.txt): ");
                addLog("User chose to load simulation from file: " + loadPath.string());
                loadFromFile(loadPath);

                if (isSimulationValid()) {
                    std::cout << "Simulation loaded successfully from '" << loadPath << "'!\n";
                    addLog("Simulation loaded successfully from: " + loadPath.string());
                    simulationReady = true;
                    initialMenuLoop = false;
                }
                else {
                    std::cerr << "Failed to load simulation from '" << loadPath << "' or loaded state is invalid. Please try again.\n";
                    addLog("Failed to load or validate simulation from: " + loadPath.string());
                }
                pressEnterToContinue();
                break;
            }
            case 0:
                std::cout << "Exiting simulation. Goodbye!\n";
                addLog("User chose to exit from initial menu.");
                askToSaveLogs(); // Ask to save logs right before exiting
                return; // Exit start() directly
            default:
                break;
            }
        }
    }

    // --- Part 3: Main simulation loop ---
    bool running = true;
    while (running) {
        clearScreen();
        printSimulation();

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
        std::cout << "10. Load Simulation from File\n";
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
            addLog("User chose to add rubbish to a single tile.");
            std::string input;
            size_t tileId;
            bool foundValidTile = false;

            std::cout << "Enter Tile ID to add rubbish to (or 'r' for random Floor tile): ";
            std::getline(std::cin, input);

            if (input.empty()) {
                std::cout << "Input for Tile ID cannot be empty.\n";
                addLog("Input for Tile ID was empty.");
                break;
            }

            if (input == "r" || input == "R") {
                if (map.getSize() == 0) {
                    std::cerr << "Error: Map is empty, cannot add rubbish to a random tile.\n";
                    addLog("Error: Map is empty, cannot add rubbish to random tile.");
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
                    addLog("Error: No Floor tiles found on the map for rubbish addition.");
                    break;
                }

                std::uniform_int_distribution<size_t> distrib(0, floorTileIds.size() - 1);
                const int maxAttempts = 100 * floorTileIds.size();
                int attempts = 0;

                while (!foundValidTile && attempts < maxAttempts) {
                    size_t randomIndex = distrib(gen);
                    tileId = floorTileIds[randomIndex];
                    Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));

                    if (floorTile && floorTile->getCleanliness() < 9) {
                        foundValidTile = true;
                    }
                    attempts++;
                }

                if (foundValidTile) {
                    std::cout << "Selected random Floor Tile ID: " << tileId << ".\n";
                    addLog("Selected random Floor Tile ID: " + std::to_string(tileId));
                }
                else {
                    std::cerr << "Could not find a cleanable random Floor tile after " << maxAttempts << " attempts. All floor tiles might be fully dirty.\n";
                    addLog("Failed to find a cleanable random floor tile.");
                    break;
                }

            }
            else {
                try {
                    tileId = std::stoul(input);
                    Tile* targetTile = map.getTile(tileId);
                    if (!targetTile || !dynamic_cast<Floor*>(targetTile)) {
                        std::cerr << "Invalid input: Tile ID " << tileId << " is not a valid Floor tile or does not exist.\n";
                        addLog("Invalid tile ID for rubbish addition: " + std::to_string(tileId));
                        break;
                    }
                    if (dynamic_cast<Floor*>(targetTile)->getCleanliness() == 9) {
                        std::cerr << "Tile ID " << tileId << " is already at maximum dirtiness (9). Cannot add more rubbish.\n";
                        addLog("Attempted to add rubbish to already max dirty tile: " + std::to_string(tileId));
                        break;
                    }
                    foundValidTile = true;
                }
                catch (const std::exception& e) {
                    std::cerr << "Invalid input for Tile ID. Please enter a valid number or 'r'.\n";
                    addLog("Invalid input for tile ID (non-numeric): " + input);
                    break;
                }
            }

            if (foundValidTile) {
                unsigned int dirtiness = getValidatedUnsignedIntInput("Enter dirtiness level (e.g., 1-9): ");
                Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));
                unsigned int maxAddable = 9 - floorTile->getCleanliness();
                if (dirtiness > maxAddable) {
                    std::cout << "Warning: Requested dirtiness " << dirtiness << " exceeds tile capacity. Adding " << maxAddable << " instead.\n";
                    addLog("Capped rubbish amount to " + std::to_string(maxAddable) + " for tile " + std::to_string(tileId));
                    dirtiness = maxAddable;
                }
                if (dirtiness > 0) {
                    addRubbish(tileId, dirtiness);
                    addLog("Added " + std::to_string(dirtiness) + " rubbish to Tile ID " + std::to_string(tileId));
                }
                else {
                    std::cout << "Tile already at maximum dirtiness or no dirtiness requested.\n";
                    addLog("No rubbish added to tile " + std::to_string(tileId) + " (already max dirty or 0 requested).");
                }
            }
            break;
        }
        case 2: {
            unsigned int totalRubbishAmount = getValidatedUnsignedIntInput("Enter TOTAL amount of rubbish points to distribute: ");
            addLog("User chose to add " + std::to_string(totalRubbishAmount) + " rubbish points serially.");
            addSerialRubbish(totalRubbishAmount);
            break;
        }
        case 3: {
            size_t newPositionId = getValidatedSizeTInput("Enter new Tile ID for robot's position: ");
            addLog("User chose to change robot's position to Tile ID: " + std::to_string(newPositionId));
            changeRobotsPosition(newPositionId);
            break;
        }
        case 4: {
            addLog("User ordered robot to go home.");
            orderRobotToGoHome();
            break;
        }
        case 5: {
            size_t targetTileId = getValidatedSizeTInput("Enter target Tile ID for robot to move: ");
            addLog("User ordered robot to move to Tile ID: " + std::to_string(targetTileId));
            orderRobotToMove(targetTileId);
            break;
        }
        case 6: {
            size_t tileId = getValidatedSizeTInput("Enter Tile ID for robot to clean: ");
            unsigned int radius = getValidatedUnsignedIntInput("Enter cleaning radius: ");
            addLog("User ordered robot to clean Tile ID: " + std::to_string(tileId) + " with radius: " + std::to_string(radius));
            orderRobotToClean(tileId, radius);
            break;
        }
        case 7: {
            addLog("User chose to reset robot's memory.");
            resetRobotMemory();
            break;
        }
        case 8: {
            unsigned int steps = getValidatedUnsignedIntInput("Enter number of simulation steps to run: ");
            addLog("User chose to run simulation for " + std::to_string(steps) + " steps.");
            runSimulation(steps);
            break;
        }
        case 9: {
            fs::path savePath = getFilePathInput("Enter filename to save simulation (e.g., my_sim.txt): ");
            addLog("User chose to save simulation state to: " + savePath.string());
            saveSimulation(savePath);
            break;
        }
        case 10: {
            fs::path loadPath = getFilePathInput("Enter filename to load simulation from (e.g., other_sim.txt): ");
            addLog("User chose to load simulation state from: " + loadPath.string());
            loadSimulation(loadPath);

            if (isSimulationValid()) {
                std::cout << "Simulation loaded successfully from '" << loadPath << "'.\n";
                addLog("Simulation loaded successfully from: " + loadPath.string());
                simulationReady = true; // This flag is already true if we are in the main menu, but good for clarity
            }
            else {
                std::cerr << "Failed to load simulation from '" << loadPath << "' or loaded state is invalid.\n";
                addLog("Failed to load or validate simulation from: " + loadPath.string());
            }
            break;
        }
        case 11: {
            addLog("User ordered robot to clean efficiently.");
            orderRobotToCleanEfficiently();
            break;
        }
        case 0: {
            addLog("User chose to exit simulation.");
            exitSimulation();
            running = false;
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again.\n";
            addLog("Invalid menu choice: " + std::to_string(choice));
            break;
        }

        if (running) {
            pressEnterToContinue();
        }
    }

    std::cout << "Thank you for playing the Robot Vacuum Cleaner Simulation!\n";
    addLog("Simulation Ended.");

    // Final log saving prompt after the simulation loop ends
    askToSaveLogs();
}

void Simulation::askToSaveLogs() {
    clearScreen();
    std::cout << "\n--- Simulation Finished ---\n";
    std::cout << "Do you want to save the simulation log to a file? (y/n): ";
    std::string response;
    std::getline(std::cin, response);

    if (response == "y" || response == "Y") {
        fs::path logFilePath = getFilePathInput("Enter log file name (e.g., simulation_log.txt): ");
        std::ofstream logFile(logFilePath);
        if (logFile.is_open()) {
            int lineNumber = 1; // Initialize line number counter
            for (const std::string& logEntry : simulationLogs) {
                logFile << lineNumber << ". " << logEntry << "\n"; // Prepend number and a dot/space
                lineNumber++; // Increment for the next line
            }
            logFile.close();
            std::cout << "Simulation log saved to '" << logFilePath << "'.\n";
        }
        else {
            std::cerr << "Error: Could not open file '" << logFilePath << "' for saving the log.\n";
        }
    }
    else {
        std::cout << "Simulation log not saved.\n";
    }
    pressEnterToContinue();
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