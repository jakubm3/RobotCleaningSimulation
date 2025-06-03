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

#include "Messages.h"

static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());

// Clears the console screen.
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Prompts the user to press Enter to continue.
void pressEnterToContinue() {
    std::cout << Messages::ENTER_PROMPT;
    std::cin.get();
}

// Gets a validated size_t input from the user.
size_t getValidatedSizeTInput(const std::string& prompt) {
    size_t value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        if (line.empty()) {
            std::cout << Messages::INPUT_EMPTY_ERROR;
            continue;
        }

        std::stringstream ss(line);
        ss >> value;
        if (ss.fail() || !ss.eof()) {
            std::cout << Messages::INVALID_NUMERIC_INPUT_ERROR;
        }
        else {
            return value;
        }
    }
}

// Gets a validated unsigned int input from the user.
unsigned int getValidatedUnsignedIntInput(const std::string& prompt) {
    unsigned int value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        if (line.empty()) {
            std::cout << Messages::INPUT_EMPTY_ERROR;
            continue;
        }

        std::stringstream ss(line);
        ss >> value;
        if (ss.fail() || !ss.eof()) {
            std::cout << Messages::INVALID_NUMERIC_INPUT_ERROR;
        }
        else {
            return value;
        }
    }
}

// Gets a file path input from the user.
fs::path getFilePathInput(const std::string& prompt) {
    std::string pathStr;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, pathStr);
        if (pathStr.empty()) {
            std::cout << Messages::INVALID_FILE_PATH_ERROR;
            continue;
        }
        return fs::path(pathStr);
    }
}

bool Simulation::isRobotValid() const {
    // Check if robot's position is within valid range
    size_t robotPosition = robot.getPosition();
    if (robotPosition >= map.getSize()) {
        std::cerr << "Robot Validation Error: Robot position (" << robotPosition
            << ") is out of bounds. Map size is " << map.getSize() << ".\n";
        return false;
    }

    // Check if the tile at robot's position exists
    const Tile* robotTile = map.getTile(robotPosition);
    if (!robotTile) {
        std::cerr << "Robot Validation Error: Tile at robot position " << robotPosition
            << " does not exist.\n";
        return false;
    }

    std::cout << "Robot validation check passed.\n";
    return true;
}

// Checks if the current simulation state is valid.
bool Simulation::isSimulationValid() const {
    // Symulacja nie pozwala na UnVisited tiles w prawdziwej mapie
    if (!map.isMapValid(false)) {
        std::cerr << Messages::MAP_NOT_VALID_ERROR;
        return false;
    }

    // Charger ID Validation
    size_t chargerId = map.getChargerId();

    // Check if charger ID is within valid range
    if (chargerId >= map.getSize()) {
        std::cerr << "Validation Error: Charger ID (" << chargerId
            << ") is out of bounds. Map size is " << map.getSize() << ".\n";
        return false;
    }

    // Check if the tile at charger ID exists
    const Tile* chargerTile = map.getTile(chargerId);
    if (!chargerTile) {
        std::cerr << "Validation Error: Charger tile at ID " << chargerId
            << " does not exist.\n";
        return false;
    }

    // Check if the tile at charger ID is actually a charger
    const Charger* charger = dynamic_cast<const Charger*>(chargerTile);
    if (!charger) {
        std::cerr << "Validation Error: Tile at charger ID " << chargerId
            << " is not a charger tile.\n";
        return false;
    }

    if (!isRobotValid()) {
        return false;
    }

    std::cout << Messages::SIMULATION_VALIDITY_CHECK_PASSED;
    return true;
}

// Adds rubbish to a specific tile on the map.
void Simulation::addRubbish(size_t tileId, unsigned int dirtiness) {
    std::cout << Messages::ADD_RUBBISH_ACTION << dirtiness << " rubbish to Tile ID: " << tileId << ".\n";

    Tile* targetTile = map.getTile(tileId);

    if (targetTile) {
        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
        if (floorTile) {
            floorTile->getDirty(dirtiness);
            std::cout << Messages::RUBBISH_ADDED_SUCCESS_PART1 << tileId << Messages::RUBBISH_ADDED_SUCCESS_PART2 << floorTile->getCleanliness() << Messages::RUBBISH_ADDED_SUCCESS_PART3;
        }
        else {
            std::cout << Messages::ADD_RUBBISH_NOT_FLOOR_ERROR << tileId << Messages::ADD_RUBBISH_NOT_FLOOR_ERROR_CONT;
        }
    }
    else {
        std::cout << Messages::ADD_RUBBISH_TILE_NOT_EXIST_ERROR << tileId << Messages::ADD_RUBBISH_TILE_NOT_EXIST_ERROR_CONT;
    }
}

// Adds a specified total amount of rubbish to random floor tiles.
void Simulation::addSerialRubbish(unsigned int totalRubbishAmount) {
    if (map.getSize() == 0) {
        std::cerr << Messages::MAP_EMPTY_SERIAL_RUBBISH_ERROR;
        return;
    }
    if (totalRubbishAmount == 0) {
        std::cout << Messages::NO_RUBBISH_POINTS_TO_ADD;
        return;
    }

    std::vector<size_t> floorTileIds;
    for (size_t i = 0; i < map.getSize(); ++i) {
        Tile* currentTile = map.getTile(i);
        if (currentTile && dynamic_cast<Floor*>(currentTile)) {
            floorTileIds.push_back(i);
        }
    }

    if (floorTileIds.empty()) {
        std::cerr << Messages::NO_FLOOR_TILES_ERROR;
        return;
    }

    std::uniform_int_distribution<size_t> tileIndexDistrib(0, floorTileIds.size() - 1);

    std::cout << Messages::DISTRIBUTING_RUBBISH_ACTION << totalRubbishAmount << " rubbish points across random floor tiles.\n";

    unsigned int rubbishPointsDistributed = 0;
    const unsigned int maxAttemptsToFindTile = 100 * floorTileIds.size();

    while (rubbishPointsDistributed < totalRubbishAmount) {
        unsigned int attempts = 0;
        bool tileDirtiedInThisIteration = false;

        while (attempts < maxAttemptsToFindTile && !tileDirtiedInThisIteration) {
            size_t tileId = floorTileIds[tileIndexDistrib(gen)];
            Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));

            if (floorTile) {
                unsigned int currentCleanliness = floorTile->getCleanliness();

                if (currentCleanliness == 9) {
                    attempts++;
                    continue;
                }

                unsigned int maxDirtinessCanAccept = 9 - currentCleanliness;

                if (maxDirtinessCanAccept == 0) {
                    attempts++;
                    continue;
                }

                std::uniform_int_distribution<unsigned int> dirtinessAmountDistrib(1, maxDirtinessCanAccept);

                unsigned int dirtinessToAttempt = dirtinessAmountDistrib(gen);

                unsigned int actualDirtiness = std::min(dirtinessToAttempt, totalRubbishAmount - rubbishPointsDistributed);


                if (actualDirtiness > 0) {
                    floorTile->getDirty(actualDirtiness);
                    rubbishPointsDistributed += actualDirtiness;
                    tileDirtiedInThisIteration = true;

                    std::cout << "  Added " << actualDirtiness << " rubbish to Tile ID " << tileId
                        << ". New cleanliness: " << floorTile->getCleanliness()
                        << " (Total distributed: " << rubbishPointsDistributed << "/" << totalRubbishAmount << ").\n";
                }
                else {
                    attempts++;
                }
            }
            else {
                attempts++;
            }
        }

        if (!tileDirtiedInThisIteration) {
            std::cerr << Messages::WARNING_COULD_NOT_FIND_TILE
                << (totalRubbishAmount - rubbishPointsDistributed)
                << Messages::WARNING_COULD_NOT_FIND_TILE_CONT
                << attempts
                << Messages::WARNING_COULD_NOT_FIND_TILE_CONT2;
            break;
        }
    }
    std::cout << Messages::RUBBISH_DISTRIBUTED_SUCCESS << rubbishPointsDistributed << " rubbish points in total.\n";
}

// Changes the robot's current position.
void Simulation::changeRobotsPosition(size_t newPositionId) {
    robot.setPosition(newPositionId);
    std::cout << Messages::ROBOT_POS_CHANGE_SUCCESS << newPositionId << ".\n";
    const Tile* currentTile = map.getTile(newPositionId);
    if (currentTile) {
        updateRobotMemory(newPositionId, currentTile);
    }
    else {
        std::cerr << Messages::ROBOT_MEM_UPDATE_WARNING << newPositionId << Messages::ROBOT_MEM_UPDATE_WARNING_CONT;
    }
}

// Orders the robot to go back to its charging station.
void Simulation::orderRobotToGoHome() {
    robot.orderToGoHome();
    std::cout << Messages::ROBOT_ORDER_HOME_SUCCESS;
}

// Orders the robot to move to a specified target tile.
void Simulation::orderRobotToMove(size_t targetTileId) {
    if (robot.orderToMove(targetTileId)) {
        std::cout << Messages::ROBOT_ORDER_MOVE_SUCCESS << targetTileId << ".\n";
    }
    else {
        std::cout << Messages::ROBOT_ORDER_MOVE_FAIL << targetTileId << Messages::ROBOT_ORDER_MOVE_FAIL_CONT;
    }
}

// Orders the robot to clean a specific tile within a given radius.
void Simulation::orderRobotToClean(size_t tileId, unsigned int radius) {
    if (robot.orderToClean(tileId, radius)) {
        std::cout << Messages::ROBOT_ORDER_CLEAN_SUCCESS << tileId << " with radius: " << radius << ".\n";
    }
    else {
        std::cout << Messages::ROBOT_ORDER_CLEAN_FAIL << tileId << Messages::ROBOT_ORDER_CLEAN_FAIL_CONT;
    }
}

// Orders the robot to clean the map efficiently.
void Simulation::orderRobotToCleanEfficiently() {
    robot.orderToCleanEfficiently();
    std::cout << Messages::ROBOT_ORDER_CLEAN_EFFICIENTLY_SUCCESS;
}

// Resets the robot's memory of the map.
void Simulation::resetRobotMemory() {
    robot.resetMemory();
    std::cout << Messages::ROBOT_MEMORY_RESET_SUCCESS;
}

// Saves the current simulation state to a file.
void Simulation::saveSimulation(fs::path filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << Messages::SIMULATION_SAVE_ERROR_FILE_OPEN << filePath << std::endl;
        return;
    }

    try {
        map.saveMap(outFile);
        outFile << "\n";
        robot.saveRobot(outFile);
        std::cout << Messages::SIMULATION_SAVE_SUCCESS << filePath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << Messages::SIMULATION_SAVE_ERROR_DURING_SAVE << e.what() << std::endl;
    }
    outFile.close();
}

// Loads a simulation state from a specified file.
void Simulation::loadSimulation(fs::path filePath) {
    std::cout << Messages::SIMULATION_LOAD_ACTION << filePath << ".\n";
    loadFromFile(filePath);
}

// Runs the simulation for a specified number of steps.
void Simulation::runSimulation(unsigned int steps) {
    if (steps == 0) {
        std::cout << Messages::NO_STEPS_TO_RUN;
        addLog("Attempted to run 0 simulation steps. No action taken.");
        return;
    }
    std::cout << Messages::RUNNING_SIMULATION_ACTION << steps << Messages::SIMULATION_STEPS_COMPLETED;
    addLog("Initiating simulation run for " + std::to_string(steps) + " steps.");

    for (unsigned int i = 0; i < steps; ++i) {
        std::cout << Messages::SIMULATION_STEP_HEADER_START << (i + 1) << Messages::SIMULATION_STEP_HEADER_END;
        addLog("--- Starting Step " + std::to_string(i + 1) + " ---");

        size_t currentRobotPos = robot.getPosition();
        addLog("Robot's current position: Tile " + std::to_string(currentRobotPos));

        const Tile* currentTile = map.getTile(currentRobotPos);
        if (currentTile) {
            updateRobotMemory(currentRobotPos, currentTile);
            addLog("Robot updated memory for current Tile " + std::to_string(currentRobotPos) + ".");
        }
        else {
            std::cerr << Messages::ROBOT_INVALID_POS_ERROR << currentRobotPos << Messages::ROBOT_INVALID_POS_ERROR_CONT;
            addLog("ERROR: Robot at invalid position " + std::to_string(currentRobotPos) + ". Simulation stopped.");
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
                    std::string dirStr;
                    switch (dir) {
                    case Direction::up:    dirStr = "Up"; break;
                    case Direction::down:  dirStr = "Down"; break;
                    case Direction::left:  dirStr = "Left"; break;
                    case Direction::right: dirStr = "Right"; break;
                    default:               dirStr = Messages::UNKNOWN; break;
                    }
                    addLog("Robot updated memory for neighbor Tile " + std::to_string(neighborId) + " (" + dirStr + " of current).");
                }
            }
        }
        std::cout << robot;
        addLog("Robot's remembered map and state printed to console.");

        std::tuple<RobotAction, Direction> actionResult;
        try {
            actionResult = robot.makeAction();
        }
        catch (const std::exception& e) {
            std::cout << Messages::ROBOT_ERROR_MESSAGE << e.what() << std::endl;
            addLog("Robot encountered an error during makeAction(): " + std::string(e.what()));
            std::cout << Messages::SIMULATION_PREMATURE_END << (i + 1) << Messages::SIMULATION_PREMATURE_END_REASON;
            addLog("Simulation finished prematurely after " + std::to_string(i + 1) + " steps due to robot error.");
            return;
        }
        RobotAction action = std::get<0>(actionResult);
        Direction chosenDir = std::get<1>(actionResult);

        std::cout << Messages::ROBOT_ACTION_PROMPT;
        std::string actionLogMessage;
        switch (action) {
        case RobotAction::move: {
            std::cout << Messages::ROBOT_ACTION_MOVE;
            std::string dirStr;
            switch (chosenDir) {
            case Direction::up:    dirStr = "Up"; break;
            case Direction::down:  dirStr = "Down"; break;
            case Direction::left:  dirStr = "Left"; break;
            case Direction::right: dirStr = "Right"; break;
            default:               dirStr = Messages::UNKNOWN; break;
            }
            actionLogMessage = "Robot decided to **Move** " + dirStr;
            actionLogMessage += " to Tile " + std::to_string(robot.getPosition());
            break;
        }
        case RobotAction::clean: {
            std::cout << Messages::ROBOT_ACTION_CLEAN;
            actionLogMessage = "Robot decided to **Clean** current Tile " + std::to_string(robot.getPosition());
            break;
        }
        case RobotAction::explore: {
            std::cout << Messages::ROBOT_ACTION_EXPLORE;
            actionLogMessage = "Robot decided to **Explore**.";
            break;
        }
        case RobotAction::none: {
            std::cout << Messages::ROBOT_ACTION_NONE_COMPLETE;
            actionLogMessage = "Robot has completed all known exploration and cleaning tasks.";
            std::cout << Messages::ROBOT_CLEAN_EFFICIENTLY_PROMPT;
            std::string response;
            std::getline(std::cin, response);

            if (!response.empty() && (response[0] == 'y' || response[0] == 'Y')) {
                robot.orderToCleanEfficiently();
                std::cout << Messages::ROBOT_CLEAN_EFFICIENTLY_ORDERED;
                actionLogMessage += ". User ordered to **Clean Efficiently**. Continuing simulation.";
            }
            else {
                std::cout << Messages::SIMULATION_FINISHED << (i + 1) << Messages::SIMULATION_STEPS_MESSAGE;
                actionLogMessage += ". User chose NOT to clean efficiently. Simulation ending.";
                addLog(actionLogMessage);
                return;
            }
            break;
        }
        default: {
            std::cout << Messages::UNKNOWN;
            actionLogMessage = "Robot decided on an **Unknown** action.";
            break;
        }
        }
        std::cout << ".\n";
        if (action != RobotAction::none) {
            addLog(actionLogMessage);
        }

        if (action == RobotAction::clean) {
            cleanTile(robot.getPosition(), robot.getCleaningEfficiency());
            addLog("Robot **performed cleaning** on Tile " + std::to_string(robot.getPosition()) +
                " with efficiency " + std::to_string(robot.getCleaningEfficiency()) +
                ". Tile cleanliness updated.");
        }
    }
    std::cout << Messages::SIMULATION_FINISHED << steps << Messages::SIMULATION_STEPS_MESSAGE;
    addLog("Simulation successfully finished after " + std::to_string(steps) + " steps.");
}

// Exits the simulation.
void Simulation::exitSimulation() {
    std::cout << Messages::SIMULATION_EXITING;
}

// Prints the current state of the simulation, including the map and robot.
void Simulation::printSimulation() {
    std::cout << Messages::CURRENT_SIMULATION_STATE_HEADER;
    std::cout << Messages::MAP_STATE_HEADER;
    std::cout << map;
    std::cout << Messages::ROBOT_STATE_HEADER;
    std::cout << robot;
    std::cout << Messages::STATE_FOOTER;
}

// Updates the robot's internal memory with information about a tile.
void Simulation::updateRobotMemory(size_t tileId, const Tile* tileObj) {
    robot.exploreTile(tileId, tileObj);
}

// Cleans a specific tile on the map.
void Simulation::cleanTile(size_t tileId, unsigned int efficiency) {
    std::cout << Messages::INTERNAL_CLEANING_ATTEMPT << tileId << Messages::INTERNAL_CLEANING_ATTEMPT_CONT << efficiency << Messages::INTERNAL_CLEANING_ATTEMPT_CONT2;
    Tile* targetTile = map.getTile(tileId);

    if (targetTile) {
        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
        if (floorTile) {
            floorTile->getCleaned(efficiency);
            std::cout << Messages::INTERNAL_TILE_CLEANED << tileId << Messages::INTERNAL_TILE_CLEANED_CONT << floorTile->getCleanliness() << Messages::INTERNAL_TILE_CLEANED_CONT2;
        }
        else {
            std::cout << Messages::INTERNAL_FAILED_CLEAN_NOT_FLOOR << tileId << Messages::INTERNAL_FAILED_CLEAN_NOT_FLOOR_CONT;
        }
    }
    else {
        std::cout << Messages::INTERNAL_FAILED_CLEAN_TILE_NOT_EXIST << tileId << Messages::INTERNAL_FAILED_CLEAN_TILE_NOT_EXIST_CONT;
    }
}

// Adds a message to the simulation's log.
void Simulation::addLog(const std::string& message) {
    simulationLogs.push_back(message);
}

// Starts the main simulation loop, handling initial setup and user interaction.
void Simulation::start(fs::path filePath) {

    clearScreen();
    std::cout << Messages::WELCOME_HEADER_TOP;
    std::cout << Messages::WELCOME_HEADER_MIDDLE_1;
    std::cout << Messages::WELCOME_HEADER_MIDDLE_2;
    std::cout << Messages::WELCOME_HEADER_BOTTOM;
    std::cout << "\n";

    addLog("Simulation Started.");

    bool simulationReady = false;
    if (!filePath.empty()) {
        std::cout << Messages::INITIAL_LOAD_ATTEMPT << filePath << std::endl;
        addLog("Attempting to load initial simulation from: " + filePath.string());
        loadFromFile(filePath);
        if (isSimulationValid()) {
            std::cout << Messages::INITIAL_LOAD_SUCCESS;
            addLog("Initial simulation state loaded and validated successfully.");
            simulationReady = true;
        }
        else {
            std::cerr << Messages::INITIAL_LOAD_FAIL_INVALID << filePath << "'.\n";
            std::cerr << Messages::INITIAL_LOAD_FAIL_PROMPT;
            addLog("Initial simulation load failed or state is invalid from: " + filePath.string());
        }
        pressEnterToContinue();
    }
    else {
        std::cout << Messages::STARTING_NEW_SIMULATION;
        addLog("Starting new simulation (no initial file provided).");
        pressEnterToContinue();
    }

    if (!simulationReady) {
        bool initialMenuLoop = true;
        while (initialMenuLoop) {
            clearScreen();
            std::cout << Messages::INITIAL_SETUP_HEADER;
            std::cout << Messages::INITIAL_MENU_LOAD;
            std::cout << Messages::INITIAL_MENU_EXIT;
            std::cout << Messages::ENTER_CHOICE_PROMPT;

            std::string choiceStr;
            std::getline(std::cin, choiceStr);
            if (choiceStr.empty()) {
                continue;
            }

            int choice;
            std::stringstream ss(choiceStr);
            ss >> choice;
            if (ss.fail() || !ss.eof() || (choice != 1 && choice != 0)) {
                std::cout << Messages::INVALID_CHOICE_ERROR;
                pressEnterToContinue();
                continue;
            }

            switch (choice) {
            case 1: {
                fs::path loadPath = getFilePathInput(Messages::ENTER_FILE_PATH_PROMPT);
                addLog("User chose to load simulation from file: " + loadPath.string());
                loadFromFile(loadPath);
                if (isSimulationValid()) {
                    std::cout << Messages::SIMULATION_LOAD_SUCCESS << loadPath << Messages::SIMULATION_LOAD_SUCCESS_CONT;
                    addLog("Simulation loaded successfully from: " + loadPath.string());
                    simulationReady = true;
                    initialMenuLoop = false;
                }
                else {
                    std::cerr << Messages::SIMULATION_LOAD_FAIL << loadPath << Messages::SIMULATION_LOAD_FAIL_RETRY;
                    addLog("Failed to load or validate simulation from: " + loadPath.string());
                }
                pressEnterToContinue();
                break;
            }
            case 0:
                std::cout << Messages::EXIT_SIMULATION_GOODBYE;
                addLog("User chose to exit from initial menu.");
                askToSaveLogs();
                return;
            default:
                break;
            }
        }
    }

    bool running = true;
    while (running) {
        clearScreen();
        printSimulation();

        std::cout << Messages::MAIN_MENU_HEADER;
        std::cout << Messages::MAIN_MENU_OPTION_1;
        std::cout << Messages::MAIN_MENU_OPTION_2;
        std::cout << Messages::MAIN_MENU_OPTION_3;
        std::cout << Messages::MAIN_MENU_OPTION_4;
        std::cout << Messages::MAIN_MENU_OPTION_5;
        std::cout << Messages::MAIN_MENU_OPTION_6;
        std::cout << Messages::MAIN_MENU_OPTION_7;
        std::cout << Messages::MAIN_MENU_OPTION_8;
        std::cout << Messages::MAIN_MENU_OPTION_9;
        std::cout << Messages::MAIN_MENU_OPTION_10;
        std::cout << Messages::MAIN_MENU_OPTION_11;
        std::cout << Messages::MAIN_MENU_OPTION_0;
        std::cout << Messages::ENTER_CHOICE_PROMPT;

        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        if (choiceStr.empty()) {
            continue;
        }

        int choice;
        std::stringstream ss(choiceStr);
        ss >> choice;
        if (ss.fail() || !ss.eof()) {
            std::cout << Messages::INVALID_CHOICE_ERROR;
            pressEnterToContinue();
            continue;
        }

        switch (choice) {
        case 1: {
            addLog("User chose to add rubbish to a single tile.");
            std::string input;
            size_t tileId;
            bool foundValidTile = false;

            std::cout << Messages::ENTER_TILE_ID_RUBBISH_PROMPT;
            std::getline(std::cin, input);

            if (input.empty()) {
                std::cout << Messages::INPUT_EMPTY_ERROR;
                addLog("Input for Tile ID was empty.");
                break;
            }

            if (input == "r" || input == "R") {
                if (map.getSize() == 0) {
                    std::cerr << Messages::MAP_EMPTY_SERIAL_RUBBISH_ERROR;
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
                    std::cerr << Messages::NO_FLOOR_TILES_ERROR;
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
                    std::cout << Messages::SELECTED_RANDOM_TILE_ID << tileId << ".\n";
                    addLog("Selected random Floor Tile ID: " + std::to_string(tileId));
                }
                else {
                    std::cerr << Messages::ALL_FLOOR_TILES_DIRTY_WARNING << maxAttempts << Messages::ALL_FLOOR_TILES_DIRTY_WARNING_CONT;
                    addLog("Failed to find a cleanable random floor tile.");
                    break;
                }

            }
            else {
                try {
                    tileId = std::stoul(input);
                    Tile* targetTile = map.getTile(tileId);
                    if (!targetTile || !dynamic_cast<Floor*>(targetTile)) {
                        std::cerr << Messages::INVALID_TILE_ID_NOT_FLOOR << tileId << Messages::INVALID_TILE_ID_NOT_FLOOR_CONT;
                        addLog("Invalid tile ID for rubbish addition: " + std::to_string(tileId));
                        break;
                    }
                    if (dynamic_cast<Floor*>(targetTile)->getCleanliness() == 9) {
                        std::cerr << Messages::TILE_ID_ALREADY_MAX_DIRTY << tileId << Messages::TILE_ID_ALREADY_MAX_DIRTY_CONT;
                        addLog("Attempted to add rubbish to already max dirty tile: " + std::to_string(tileId));
                        break;
                    }
                    foundValidTile = true;
                }
                catch (const std::exception& e) {
                    std::cerr << Messages::INVALID_TILE_ID_INPUT_ERROR;
                    addLog("Invalid input for tile ID (non-numeric): " + input);
                    break;
                }
            }

            if (foundValidTile) {
                unsigned int dirtiness = getValidatedUnsignedIntInput(Messages::ENTER_DIRTINESS_LEVEL_PROMPT);
                Floor* floorTile = dynamic_cast<Floor*>(map.getTile(tileId));
                unsigned int maxAddable = 9 - floorTile->getCleanliness();
                if (dirtiness > maxAddable) {
                    std::cout << Messages::WARNING_DIRTINESS_CAP << dirtiness << Messages::WARNING_DIRTINESS_CAP_CONT << maxAddable << Messages::WARNING_DIRTINESS_CAP_CONT2;
                    addLog("Capped rubbish amount to " + std::to_string(maxAddable) + " for tile " + std::to_string(tileId));
                    dirtiness = maxAddable;
                }
                if (dirtiness > 0) {
                    addRubbish(tileId, dirtiness);
                    addLog("Added " + std::to_string(dirtiness) + " rubbish to Tile ID " + std::to_string(tileId));
                }
                else {
                    std::cout << Messages::TILE_ALREADY_MAX_DIRTY;
                    addLog("No rubbish added to tile " + std::to_string(tileId) + " (already max dirty or 0 requested).");
                }
            }
            break;
        }
        case 2: {
            unsigned int totalRubbishAmount = getValidatedUnsignedIntInput(Messages::ENTER_TOTAL_RUBBISH_AMOUNT_PROMPT);
            addLog("User chose to add " + std::to_string(totalRubbishAmount) + " rubbish points serially.");
            addSerialRubbish(totalRubbishAmount);
            break;
        }
        case 3: {
            size_t newPositionId = getValidatedSizeTInput(Messages::ENTER_NEW_ROBOT_POS_PROMPT);
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
            size_t targetTileId = getValidatedSizeTInput(Messages::ENTER_TARGET_TILE_MOVE_PROMPT);
            addLog("User ordered robot to move to Tile ID: " + std::to_string(targetTileId));
            orderRobotToMove(targetTileId);
            break;
        }
        case 6: {
            size_t tileId = getValidatedSizeTInput(Messages::ENTER_TILE_CLEAN_PROMPT);
            unsigned int radius = getValidatedUnsignedIntInput(Messages::ENTER_CLEANING_RADIUS_PROMPT);
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
            unsigned int steps = getValidatedUnsignedIntInput(Messages::ENTER_SIM_STEPS_PROMPT);
            addLog("User chose to run simulation for " + std::to_string(steps) + " steps.");
            runSimulation(steps);
            break;
        }
        case 9: {
            fs::path savePath = getFilePathInput(Messages::ENTER_SAVE_FILENAME_PROMPT);
            addLog("User chose to save simulation state to: " + savePath.string());
            saveSimulation(savePath);
            break;
        }
        case 10: {
            fs::path loadPath = getFilePathInput(Messages::ENTER_LOAD_FILENAME_PROMPT);
            addLog("User chose to load simulation state from: " + loadPath.string());
            loadSimulation(loadPath);
            if (isSimulationValid()) {
                std::cout << Messages::SIMULATION_LOAD_SUCCESS << loadPath << Messages::SIMULATION_LOAD_SUCCESS_CONT;
                addLog("Simulation loaded successfully from: " + loadPath.string());
                simulationReady = true;
            }
            else {
                std::cerr << Messages::SIMULATION_LOAD_FAIL << loadPath << " or loaded state is invalid.\n";
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
            std::cout << Messages::INVALID_CHOICE_ERROR;
            addLog("Invalid menu choice: " + std::to_string(choice));
            break;
        }

        if (running) {
            pressEnterToContinue();
        }
    }

    std::cout << Messages::SIMULATION_FINISHED_THANK_YOU;
    addLog("Simulation Ended.");

    askToSaveLogs();
}

// Asks the user if they want to save simulation logs and handles the saving process.
void Simulation::askToSaveLogs() {
    clearScreen();
    std::cout << Messages::SIMULATION_FINISHED_HEADER;
    std::cout << Messages::DO_YOU_WANT_SAVE_LOGS_PROMPT;
    std::string response;
    std::getline(std::cin, response);
    if (response == "y" || response == "Y") {
        fs::path logFilePath = getFilePathInput(Messages::ENTER_LOG_FILENAME_PROMPT);
        std::ofstream logFile(logFilePath);
        if (logFile.is_open()) {
            int lineNumber = 1;
            for (const std::string& logEntry : simulationLogs) {
                logFile << lineNumber << ". " << logEntry << "\n";
                lineNumber++;
            }
            logFile.close();
            std::cout << Messages::SIMULATION_LOG_SAVED_TO << logFilePath << Messages::SIMULATION_LOG_SAVED_TO_CONT;
        }
        else {
            std::cerr << Messages::ERROR_COULD_NOT_OPEN_FILE_FOR_LOG << logFilePath << Messages::ERROR_COULD_NOT_OPEN_FILE_FOR_LOG_CONT;
        }
    }
    else {
        std::cout << Messages::SIMULATION_LOG_NOT_SAVED;
    }
    pressEnterToContinue();
}

// Loads simulation data (map and robot) from a specified file path.
void Simulation::loadFromFile(fs::path filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << Messages::ERROR_COULD_NOT_OPEN_FILE << filePath << std::endl;
        return;
    }

    std::stringstream mapDataStream;
    std::stringstream robotDataStream;
    std::string line;
    bool readingMap = true;
    while (std::getline(inputFile, line)) {
        if (readingMap) {
            if (line.empty()) {
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
    inputFile.close();

    try {
        // Load simulation map without UnVisited tiles (real world)
        map.loadMap(mapDataStream, false);
        std::cout << Messages::MAP_DATA_LOADED_SUCCESSFULLY;
    }
    catch (const std::exception& e) {
        std::cerr << Messages::ERROR_LOADING_MAP_DATA << e.what() << std::endl;
        std::cerr << Messages::MAP_LOADING_FAILED_NO_ROBOT_INIT;
        return;
    }

    if (robotDataStream.str().empty()) {
        std::cout << Messages::ROBOT_DATA_NOT_FOUND_INIT;
        // Create robot with UnVisited memory
        robot = Robot(map.getWidth(), map.getHeight(), map.getChargerId());
        std::cout << Messages::ROBOT_INITIALIZED_WITH << map.getWidth()
            << Messages::ROBOT_INITIALIZED_WITH_CONT1 << map.getHeight()
            << Messages::ROBOT_INITIALIZED_WITH_CONT2 << map.getChargerId() << Messages::ROBOT_INITIALIZED_WITH_CONT3;
    }
    else {
        try {
            // Robot loads with his memory map (can contain UnVisited)
            robot.loadRobot(robotDataStream);
            std::cout << Messages::ROBOT_DATA_LOADED_SUCCESSFULLY;
        }
        catch (const std::exception& e) {
            std::cerr << Messages::ERROR_LOADING_ROBOT_DATA << e.what() << std::endl;
            std::cerr << Messages::ROBOT_LOADING_FAILED_FALLBACK;
            robot = Robot(map.getWidth(), map.getHeight(), map.getChargerId());
        }
    }
}