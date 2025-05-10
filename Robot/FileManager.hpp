#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

template <typename T>
class FileManager
{
private:
    enum FileMode
    {
        write,
        read,
        none
    };
    std::ifstream readingFile;
    std::ofstream writingFile;
    FileMode currMode = none;

public:
    FileManager() = default;
    ~FileManager() {
        closeFiles();
    }

    bool readingMode(const fs::path& filePath) {
        closeFiles();

        readingFile.open(filePath);
        if (!readingFile.is_open()) {
            return false;
        }
        currMode = FileMode::read;
        return true;
    }

    bool writingMode(const fs::path& filePath, bool overwrite = false) {
        closeFiles();

        writingFile.open(filePath, overwrite ? std::ios::trunc : std::ios::app);
        if (!writingFile.is_open()) {
            return false;
        }
        currMode = FileMode::write;
        return true;
    }

    void closeFiles() {
        if (readingFile.is_open()) {
            readingFile.close();
        }
        if (writingFile.is_open()) {
            writingFile.close();
        }
    }

    bool isFileOpen() const { return readingFile.is_open() || writingFile.is_open(); }
    bool isFullyRead() const { return readingFile.eof(); }
    bool isReading() const { return currMode == FileMode::read; }
    bool isWriting() const { return currMode == FileMode::write; }

    bool readLine(std::string& line) {
        if (currMode == FileMode::read && std::getline(readingFile, line)) {
            return true;
        }
        return false;
    }

    bool write(const T& data) {
        if (currMode != FileMode::write || !writingFile.is_open()) {
            return false;
        }
        writingFile << data;
        return true;
    }

    bool loadFromFile(std::ostream& os) {
        if (currMode != FileMode::read) {
            return false;
        }

        // Save pointer position
        std::streampos originalPos = readingFile.tellg();
        readingFile.clear();
        readingFile.seekg(0, std::ios::beg);

        // Read whole file
        std::string line;
        while (std::getline(readingFile, line)) {
            os << line << "\n";
        }

        // Restore pointer position
        readingFile.clear();
        readingFile.seekg(originalPos);
        return true;
    }

    bool writeToFile(std::istream& is) {
        if (currMode != FileMode::write) {
            return false;
        }

        T data;
        while (is >> data) {
            write(data);
        }
        return true;
    }
};
