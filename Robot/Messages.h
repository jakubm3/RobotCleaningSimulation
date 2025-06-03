// Messages.h (Updated with ALL constants from Simulation.cpp)
#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

namespace Messages {
    // --- General Input Validation & Prompts ---
    const std::string INPUT_EMPTY_ERROR = "Input cannot be empty. Please enter a positive whole number.\n";
    const std::string INVALID_NUMERIC_INPUT_ERROR = "Invalid input. Please enter a positive whole number.\n";
    const std::string INVALID_CHOICE_ERROR = "Invalid input. Please enter a number from the menu.\n";
    const std::string ENTER_CHOICE_PROMPT = "Enter your choice: ";
    const std::string ENTER_PROMPT = "\nPress Enter to continue...";
    const std::string ENTER_FILE_PATH_PROMPT = "Enter file name (e.g., my_file.txt): ";
    const std::string INVALID_FILE_PATH_ERROR = "Invalid file path. Please try again.\n";
    const std::string ENTER_TILE_ID_RUBBISH_PROMPT = "Enter Tile ID to add rubbish to (or 'r' for random Floor tile): ";
    const std::string ENTER_DIRTINESS_LEVEL_PROMPT = "Enter dirtiness level (e.g., 1-9): ";
    const std::string SELECTED_RANDOM_TILE_ID = "Selected random Floor Tile ID: ";
    const std::string ENTER_TOTAL_RUBBISH_AMOUNT_PROMPT = "Enter TOTAL amount of rubbish points to distribute: ";
    const std::string ENTER_NEW_ROBOT_POS_PROMPT = "Enter new Tile ID for robot's position: ";
    const std::string ENTER_TARGET_TILE_MOVE_PROMPT = "Enter target Tile ID for robot to move: ";
    const std::string ENTER_TILE_CLEAN_PROMPT = "Enter Tile ID for robot to clean: ";
    const std::string ENTER_CLEANING_RADIUS_PROMPT = "Enter cleaning radius: ";
    const std::string ENTER_SIM_STEPS_PROMPT = "Enter number of simulation steps to run: ";
    const std::string ENTER_SAVE_FILENAME_PROMPT = "Enter filename to save simulation (e.g., my_sim.txt): ";
    const std::string ENTER_LOAD_FILENAME_PROMPT = "Enter filename to load simulation from (e.g., other_sim.txt): ";
    const std::string DO_YOU_WANT_SAVE_LOGS_PROMPT = "Do you want to save the simulation log to a file? (y/N): ";
    const std::string ENTER_LOG_FILENAME_PROMPT = "Enter log file name (e.g., simulation_log.txt): ";

    // --- Welcome & Exit --
    const std::string WELCOME_HEADER_TOP = "---------------------------------------------------\n";
    const std::string WELCOME_HEADER_MIDDLE_1 = "           WELCOME TO THE ROBOT VACUUM CLEANER        \n";
    const std::string WELCOME_HEADER_MIDDLE_2 = "                 SIMULATION GAME!                   \n";
    const std::string WELCOME_HEADER_BOTTOM = "---------------------------------------------------\n";
    const std::string EXIT_SIMULATION_GOODBYE = "Exiting simulation. Goodbye!\n";
    const std::string SIMULATION_FINISHED_THANK_YOU = "Thank you for playing the Robot Vacuum Cleaner Simulation!\n";
    const std::string SIMULATION_EXITING = "Exiting simulation\n";
    const std::string SIMULATION_FINISHED_HEADER = "\n--- Simulation Finished ---\n";
    const std::string SIMULATION_LOG_NOT_SAVED = "Simulation log not saved.\n";
    const std::string SIMULATION_LOG_SAVED_TO = "Simulation log saved to '";
    const std::string SIMULATION_LOG_SAVED_TO_CONT = "'.\n";

    // --- Initial Setup Menu ---
    const std::string INITIAL_SETUP_HEADER = "\n--- Initial Setup ---\n";
    const std::string INITIAL_MENU_LOAD = "1. Load Simulation from file\n";
    const std::string INITIAL_MENU_EXIT = "0. Exit Simulation\n";
    const std::string INITIAL_LOAD_ATTEMPT = "Attempting to load initial simulation data from: ";
    const std::string INITIAL_LOAD_SUCCESS = "Initial simulation state loaded and validated.\n";
    const std::string INITIAL_LOAD_FAIL_INVALID = "Initial simulation state is invalid or failed to load from '";
    const std::string INITIAL_LOAD_FAIL_PROMPT = "'. Please load a valid simulation file to proceed.\n";
    const std::string STARTING_NEW_SIMULATION = "Starting new simulation (no file loaded initially).\n";
    const std::string SIMULATION_LOAD_SUCCESS = "Simulation loaded successfully from '";
    const std::string SIMULATION_LOAD_SUCCESS_CONT = "'!\n";
    const std::string SIMULATION_LOAD_FAIL = "Failed to load simulation from '";
    const std::string SIMULATION_LOAD_FAIL_RETRY = "' or loaded state is invalid. Please try again.\n";

    // --- Main Menu ---
    const std::string MAIN_MENU_HEADER = "\n--- Main Menu ---\n";
    const std::string MAIN_MENU_OPTION_1 = "1. Add Rubbish to a Tile (single)\n";
    const std::string MAIN_MENU_OPTION_2 = "2. Add Rubbish to Multiple Random Tiles (serial)\n";
    const std::string MAIN_MENU_OPTION_3 = "3. Change Robot's Position\n";
    const std::string MAIN_MENU_OPTION_4 = "4. Order Robot to Go Home\n";
    const std::string MAIN_MENU_OPTION_5 = "5. Order Robot to Move to a Tile\n";
    const std::string MAIN_MENU_OPTION_6 = "6. Order Robot to Clean a Tile\n";
    const std::string MAIN_MENU_OPTION_7 = "7. Reset Robot's Memory\n";
    const std::string MAIN_MENU_OPTION_8 = "8. Run Simulation Steps\n";
    const std::string MAIN_MENU_OPTION_9 = "9. Save Current Simulation State\n";
    const std::string MAIN_MENU_OPTION_10 = "10. Load Simulation from File\n";
    const std::string MAIN_MENU_OPTION_11 = "11. Order Robot to Clean Efficiently\n";
    const std::string MAIN_MENU_OPTION_0 = "0. Exit Simulation\n";

    // --- Map & Robot Related Messages ---
    const std::string MAP_NOT_VALID_ERROR = "Error: The map is not valid for simulation.\n";
    const std::string SIMULATION_VALIDITY_CHECK_PASSED = "Simulation validity check passed.\n";
    const std::string ADD_RUBBISH_ACTION = "Action: Attempting to add ";
    const std::string RUBBISH_ADDED_SUCCESS_PART1 = "Rubbish added to Tile ";
    const std::string RUBBISH_ADDED_SUCCESS_PART2 = ". New cleanliness: ";
    const std::string RUBBISH_ADDED_SUCCESS_PART3 = ".\n";
    const std::string ADD_RUBBISH_NOT_FLOOR_ERROR = "Failed to add rubbish: Tile ID ";
    const std::string ADD_RUBBISH_NOT_FLOOR_ERROR_CONT = " is not a Floor tile (it's an Obstacle, Charger, or UnVisited).\n";
    const std::string ADD_RUBBISH_TILE_NOT_EXIST_ERROR = "Failed to add rubbish: Tile ID ";
    const std::string ADD_RUBBISH_TILE_NOT_EXIST_ERROR_CONT = " does not exist on the map.\n";
    const std::string MAP_EMPTY_SERIAL_RUBBISH_ERROR = "Error: Map is empty, cannot add serial rubbish.\n";
    const std::string NO_RUBBISH_POINTS_TO_ADD = "No rubbish points to add.\n";
    const std::string NO_FLOOR_TILES_ERROR = "Error: No Floor tiles found on the map to add rubbish.\n";
    const std::string DISTRIBUTING_RUBBISH_ACTION = "Action: Distributing a total of ";
    const std::string RUBBISH_DISTRIBUTED_SUCCESS = "Successfully distributed ";
    const std::string WARNING_COULD_NOT_FIND_TILE = "Warning: Could not find a suitable floor tile to add remaining ";
    const std::string WARNING_COULD_NOT_FIND_TILE_CONT = " rubbish points after ";
    const std::string WARNING_COULD_NOT_FIND_TILE_CONT2 = " attempts. All available floor tiles might be fully dirty or an unexpected error occurred.\n";
    const std::string ROBOT_POS_CHANGE_SUCCESS = "Robot's position successfully changed to Tile ID: ";
    const std::string ROBOT_MEM_UPDATE_WARNING = "Warning: Could not retrieve actual tile for ID ";
    const std::string ROBOT_MEM_UPDATE_WARNING_CONT = " to update robot memory.\n";
    const std::string ROBOT_ORDER_HOME_SUCCESS = "Robot ordered to go home.\n";
    const std::string ROBOT_ORDER_MOVE_SUCCESS = "Robot ordered to move to Tile ID: ";
    const std::string ROBOT_ORDER_MOVE_FAIL = "Failed to order robot to move to Tile ID: ";
    const std::string ROBOT_ORDER_MOVE_FAIL_CONT = ". Path not found or invalid target.\n";
    const std::string ROBOT_ORDER_CLEAN_SUCCESS = "Robot ordered to clean Tile ID: ";
    const std::string ROBOT_ORDER_CLEAN_FAIL = "Failed to order robot to clean Tile ID: ";
    const std::string ROBOT_ORDER_CLEAN_FAIL_CONT = ". Invalid target or already clean.\n";
    const std::string ROBOT_ORDER_CLEAN_EFFICIENTLY_SUCCESS = "Robot ordered to clean efficiently (based on current position).\n";
    const std::string ROBOT_MEMORY_RESET_SUCCESS = "Robot's memory successfully reset.\n";
    const std::string SIMULATION_SAVE_ERROR_FILE_OPEN = "Error: Could not open file for saving: ";
    const std::string SIMULATION_SAVE_ERROR_DURING_SAVE = "Error during saving: ";
    const std::string SIMULATION_SAVE_SUCCESS = "Simulation state saved successfully to: ";
    const std::string SIMULATION_LOAD_ACTION = "Action: Loading simulation from file: ";
    const std::string CURRENT_SIMULATION_STATE_HEADER = "\n--- Current Simulation State ---\n";
    const std::string MAP_STATE_HEADER = "Map State:\n";
    const std::string ROBOT_STATE_HEADER = "\nRobot State:\n";
    const std::string STATE_FOOTER = "--------------------------------\n";

    // --- Run Simulation Specific Messages ---
    const std::string NO_STEPS_TO_RUN = "No steps to run.\n";
    const std::string RUNNING_SIMULATION_ACTION = "Action: Running simulation for ";
    const std::string SIMULATION_STEPS_COMPLETED = " steps...\n";
    const std::string SIMULATION_STEP_HEADER_START = "\n--- Step ";
    const std::string SIMULATION_STEP_HEADER_END = " ---\n";
    const std::string ROBOT_INVALID_POS_ERROR = "Error: Robot at invalid position ";
    const std::string ROBOT_INVALID_POS_ERROR_CONT = " or tile not found on main map. Stopping simulation.\n";
    const std::string ROBOT_ACTION_PROMPT = "Robot action: ";
    const std::string ROBOT_ACTION_MOVE = "Move";
    const std::string ROBOT_ACTION_CLEAN = "Clean";
    const std::string ROBOT_ACTION_EXPLORE = "Explore";
    const std::string ROBOT_ACTION_NONE_COMPLETE = "None (Robot has completed all known tasks).\n";
    const std::string ROBOT_CLEAN_EFFICIENTLY_PROMPT = "Do you want to order the robot to clean efficiently (y/N)? ";
    const std::string ROBOT_CLEAN_EFFICIENTLY_ORDERED = "Robot ordered to clean efficiently. Continuing simulation.\n";
    const std::string ROBOT_ERROR_MESSAGE = "Robot error: ";
    const std::string SIMULATION_PREMATURE_END = "Simulation finished prematurely after ";
    const std::string SIMULATION_PREMATURE_END_REASON = " steps due to robot error.\n";
    const std::string SIMULATION_FINISHED = "Simulation finished after ";
    const std::string SIMULATION_STEPS_MESSAGE = " steps.\n";
    const std::string ALL_FLOOR_TILES_DIRTY_WARNING = "Could not find a cleanable random Floor tile after ";
    const std::string ALL_FLOOR_TILES_DIRTY_WARNING_CONT = " attempts. All floor tiles might be fully dirty.\n";
    const std::string WARNING_DIRTINESS_CAP = "Warning: Requested dirtiness ";
    const std::string WARNING_DIRTINESS_CAP_CONT = " exceeds tile capacity. Adding ";
    const std::string WARNING_DIRTINESS_CAP_CONT2 = " instead.\n";
    const std::string TILE_ALREADY_MAX_DIRTY = "Tile already at maximum dirtiness or no dirtiness requested.\n";
    const std::string INVALID_TILE_ID_NOT_FLOOR = "Invalid input: Tile ID ";
    const std::string INVALID_TILE_ID_NOT_FLOOR_CONT = " is not a valid Floor tile or does not exist.\n";
    const std::string TILE_ID_ALREADY_MAX_DIRTY = "Tile ID ";
    const std::string TILE_ID_ALREADY_MAX_DIRTY_CONT = " is already at maximum dirtiness (9). Cannot add more rubbish.\n";
    const std::string INVALID_TILE_ID_INPUT_ERROR = "Invalid input for Tile ID. Please enter a valid number or 'r'.\n";

    // --- Internal Debug/Info Messages (Can be removed or controlled by a debug flag) ---
    const std::string INTERNAL_CLEANING_ATTEMPT = "Internal: Attempting to clean Tile ID: ";
    const std::string INTERNAL_CLEANING_ATTEMPT_CONT = " with efficiency: ";
    const std::string INTERNAL_CLEANING_ATTEMPT_CONT2 = ".\n";
    const std::string INTERNAL_TILE_CLEANED = "Tile ";
    const std::string INTERNAL_TILE_CLEANED_CONT = " cleaned. New cleanliness: ";
    const std::string INTERNAL_TILE_CLEANED_CONT2 = ".\n";
    const std::string INTERNAL_FAILED_CLEAN_NOT_FLOOR = "Failed to clean: Tile ID ";
    const std::string INTERNAL_FAILED_CLEAN_NOT_FLOOR_CONT = " is not a Floor tile (it's an Obstacle, Charger, or UnVisited).\n";
    const std::string INTERNAL_FAILED_CLEAN_TILE_NOT_EXIST = "Failed to clean: Tile ID ";
    const std::string INTERNAL_FAILED_CLEAN_TILE_NOT_EXIST_CONT = " does not exist on the map.\n";
    const std::string MAP_DATA_LOADED_SUCCESSFULLY = "Map data loaded successfully.\n";
    const std::string ERROR_LOADING_MAP_DATA = "Error loading map data: ";
    const std::string MAP_LOADING_FAILED_NO_ROBOT_INIT = "Map loading failed. Cannot proceed with robot initialization. Returning.\n";
    const std::string ROBOT_DATA_NOT_FOUND_INIT = "Robot data not found in file. Initializing robot with map dimensions and charger ID.\n";
    const std::string ROBOT_INITIALIZED_WITH = "Robot initialized with: Width=";
    const std::string ROBOT_INITIALIZED_WITH_CONT1 = ", Height=";
    const std::string ROBOT_INITIALIZED_WITH_CONT2 = ", ChargerID=";
    const std::string ROBOT_INITIALIZED_WITH_CONT3 = ".\n";
    const std::string ROBOT_DATA_LOADED_SUCCESSFULLY = "Robot data loaded successfully.\n";
    const std::string ERROR_LOADING_ROBOT_DATA = "Error loading robot data: ";
    const std::string ROBOT_LOADING_FAILED_FALLBACK = "Robot loading failed. Re-initializing robot with map dimensions and charger ID as a fallback.\n";
    const std::string ERROR_COULD_NOT_OPEN_FILE = "Error: Could not open file ";
    const std::string ERROR_COULD_NOT_OPEN_FILE_FOR_LOG = "Error: Could not open file '";
    const std::string ERROR_COULD_NOT_OPEN_FILE_FOR_LOG_CONT = "' for saving the log.\n";
    const std::string UNKNOWN = "Unknown"; // For robot action switch
} // namespace Messages

#endif // MESSAGES_H