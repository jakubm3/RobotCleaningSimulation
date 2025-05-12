#include <iostream>
#include <filesystem>
#include "FileManager.hpp"
#include "Simulation.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	FileManager<Simulation> fm;
	fs::path filePath;
	
	if (argc < 2) {
		// No file given
	}
	else {
		filePath = argv[1];
		bool correctFile = false;
		if (fs::exists(filePath)) {
			correctFile = fm.readingMode(filePath);
		}
		// Try to load file
	}

	std::cout << "\nCLEANING ROBOT SIMULATION\n\n\n";
	// If file not loaded, give path
	if (true) {
		std::cout << "Give file path: ";
		std::string givenPath;
		std::cin >> givenPath;
	}

	// If file loaded succesfully, start simulation

	return 0;
}