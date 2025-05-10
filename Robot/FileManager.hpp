#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
        if (readingFile.is_open()) {
            readingFile.close();
        }
        if (writingFile.is_open()) {
            writingFile.close();
        }
    }

    bool readingMode(const std::string& filePath) {
        if (currMode != FileMode::none && readingFile.is_open()) {
            readingFile.close();
        }

        readingFile.open(filePath);
        if (!readingFile.is_open()) {
            return false;
        }
        currMode = FileMode::read;
        return true;
    }

    bool writingMode(const std::string& filePath, bool overwrite = false) {
        if (currMode != FileMode::none && readingFile.is_open()) {
            readingFile.close();
        }
        writingFile.open(filePath, overwrite ? std::ios::trunc : std::ios::app);
        if (!writingFile.is_open()) {
            return false;
        }
        currMode = FileMode::write;
        return true;
    }

    void closeFile() {
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

    friend std::ostream& operator<<(std::ostream& os, const FileManager<T>& fm);
    friend std::istream& operator>>(std::istream& is, FileManager<T>& fm);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const FileManager<T>& fm) {
    if (fm.currMode != FileManager<T>::FileMode::read) {
        return os;
    }

    // Save pointer position
    std::streampos originalPos = fm.readingFile.tellg();
    fm.readingFile.clear();
    fm.readingFile.seekg(0, std::ios::beg);

    // Read whole file
    std::string line;
    while (std::getline(fm.readingFile, line)) {
        os << line << "\n";
    }

    // Restore pointer position
    fm.readingFile.clear();
    fm.readingFile.seekg(originalPos);
    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, FileManager<T>& fm) {
    if (fm.currMode != FileManager<T>::FileMode::read) {
        return is;
    }

    T data;
    while (is >> data) {
        fm.write(data); 
    }

    return is;
}
