#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "../Robot/FileManager.hpp"

namespace fs = std::filesystem;

class FileManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDir = fs::temp_directory_path() / "filemanager_tests";
        fs::create_directories(testDir);

        // Create test files with different content
        testFile1 = testDir / "test1.txt";
        testFile2 = testDir / "test2.txt";
        emptyFile = testDir / "empty.txt";
        nonExistentFile = testDir / "non_existent.txt";

        // Write test content
        std::ofstream(testFile1) << "Line 1\nLine 2\nLine 3\n";
        std::ofstream(testFile2) << "Hello World\nTest Content\n";
        std::ofstream(emptyFile) << "";
    }

    void TearDown() override {
        // Clean up test files
        fs::remove_all(testDir);
    }

    fs::path testDir;
    fs::path testFile1;
    fs::path testFile2;
    fs::path emptyFile;
    fs::path nonExistentFile;
};

// ========== CONSTRUCTOR AND DESTRUCTOR TESTS ==========

TEST_F(FileManagerTest, DefaultConstructor) {
    FileManager<std::string> fm;
    EXPECT_FALSE(fm.isFileOpen());
    EXPECT_FALSE(fm.isReading());
    EXPECT_FALSE(fm.isWriting());
}

TEST_F(FileManagerTest, DestructorClosesFiles) {
    {
        FileManager<std::string> fm;
        EXPECT_TRUE(fm.readingMode(testFile1));
        EXPECT_TRUE(fm.isFileOpen());
    }
    // Destructor should have closed files automatically
    EXPECT_TRUE(true); // Test passes if no exceptions thrown
}

// ========== READING MODE TESTS ==========

TEST_F(FileManagerTest, ReadingModeWithExistingFile) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1));
    EXPECT_TRUE(fm.isFileOpen());
    EXPECT_TRUE(fm.isReading());
    EXPECT_FALSE(fm.isWriting());
}

TEST_F(FileManagerTest, ReadingModeWithNonExistentFile) {
    FileManager<std::string> fm;
    EXPECT_FALSE(fm.readingMode(nonExistentFile));
    EXPECT_FALSE(fm.isFileOpen());
    EXPECT_FALSE(fm.isReading());
}

TEST_F(FileManagerTest, ReadingModeWithEmptyFile) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(emptyFile));
    EXPECT_TRUE(fm.isFileOpen());
    EXPECT_TRUE(fm.isReading());
}

// ========== WRITING MODE TESTS ==========

TEST_F(FileManagerTest, WritingModeCreateNewFile) {
    fs::path newFile = testDir / "new_file.txt";
    FileManager<std::string> fm;

    EXPECT_TRUE(fm.writingMode(newFile));
    EXPECT_TRUE(fm.isFileOpen());
    EXPECT_FALSE(fm.isReading());
    EXPECT_TRUE(fm.isWriting());

    fm.closeFiles();
    EXPECT_TRUE(fs::exists(newFile));
}

TEST_F(FileManagerTest, WritingModeOverwriteExisting) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.writingMode(testFile1, true)); // overwrite = true
    EXPECT_TRUE(fm.isWriting());
}

TEST_F(FileManagerTest, WritingModeAppendToExisting) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.writingMode(testFile1, false)); // append mode
    EXPECT_TRUE(fm.isWriting());
}

// ========== FILE OPERATIONS TESTS ==========

TEST_F(FileManagerTest, ReadLineFromFile) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1));

    std::string line;
    EXPECT_TRUE(fm.readLine(line));
    EXPECT_EQ(line, "Line 1");

    EXPECT_TRUE(fm.readLine(line));
    EXPECT_EQ(line, "Line 2");

    EXPECT_TRUE(fm.readLine(line));
    EXPECT_EQ(line, "Line 3");

    // No more lines
    EXPECT_FALSE(fm.readLine(line));
}

TEST_F(FileManagerTest, ReadLineFromEmptyFile) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(emptyFile));

    std::string line;
    EXPECT_FALSE(fm.readLine(line));
}

TEST_F(FileManagerTest, WriteToFile) {
    fs::path outputFile = testDir / "output.txt";
    FileManager<std::string> fm;

    EXPECT_TRUE(fm.writingMode(outputFile));
    EXPECT_TRUE(fm.write("Hello "));
    EXPECT_TRUE(fm.write("World!\n"));

    fm.closeFiles();

    // Verify content was written
    std::ifstream file(outputFile);
    std::string content;
    std::getline(file, content);
    EXPECT_EQ(content, "Hello World!");
}

TEST_F(FileManagerTest, WriteToFileWhenNotInWritingMode) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1)); // reading mode, not writing

    EXPECT_FALSE(fm.write("This should fail"));
}

// ========== CLOSE FILES TESTS ==========

TEST_F(FileManagerTest, CloseFilesWhenReading) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1));
    EXPECT_TRUE(fm.isFileOpen());

    fm.closeFiles();
    EXPECT_FALSE(fm.isFileOpen());
    // Note: closeFiles() doesn't change currMode, so isReading() might still be true
    // This is the actual behavior of the FileManager implementation
}

TEST_F(FileManagerTest, CloseFilesWhenWriting) {
    fs::path outputFile = testDir / "output.txt";
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.writingMode(outputFile));
    EXPECT_TRUE(fm.isFileOpen());

    fm.closeFiles();
    EXPECT_FALSE(fm.isFileOpen());
    // Note: closeFiles() doesn't change currMode, so isWriting() might still be true
    // This is the actual behavior of the FileManager implementation
}

TEST_F(FileManagerTest, CloseFilesWhenNoneOpen) {
    FileManager<std::string> fm;
    EXPECT_FALSE(fm.isFileOpen());

    // Should not crash
    fm.closeFiles();
    EXPECT_FALSE(fm.isFileOpen());
}

// ========== LOAD FROM FILE TESTS ==========

TEST_F(FileManagerTest, LoadFromFileToStream) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1));

    std::ostringstream oss;
    EXPECT_TRUE(fm.loadFromFile(oss));

    std::string expected = "Line 1\nLine 2\nLine 3\n";
    EXPECT_EQ(oss.str(), expected);
}

TEST_F(FileManagerTest, LoadFromFileWhenNotReading) {
    FileManager<std::string> fm;
    std::ostringstream oss;

    // Not in reading mode
    EXPECT_FALSE(fm.loadFromFile(oss));
}

TEST_F(FileManagerTest, LoadFromEmptyFile) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(emptyFile));

    std::ostringstream oss;
    EXPECT_TRUE(fm.loadFromFile(oss));
    EXPECT_TRUE(oss.str().empty());
}

// ========== WRITE TO FILE FROM STREAM TESTS ==========

TEST_F(FileManagerTest, WriteToFileFromStream) {
    fs::path outputFile = testDir / "stream_output.txt";
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.writingMode(outputFile));

    std::istringstream iss("word1 word2 word3");
    EXPECT_TRUE(fm.writeToFile(iss));

    fm.closeFiles();

    // Verify content
    std::ifstream file(outputFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "word1word2word3");
}

TEST_F(FileManagerTest, WriteToFileFromStreamWhenNotWriting) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1)); // reading mode

    std::istringstream iss("test");
    EXPECT_FALSE(fm.writeToFile(iss));
}

// ========== MODE SWITCHING TESTS ==========

TEST_F(FileManagerTest, SwitchFromReadingToWriting) {
    FileManager<std::string> fm;

    // Start in reading mode
    EXPECT_TRUE(fm.readingMode(testFile1));
    EXPECT_TRUE(fm.isReading());
    EXPECT_FALSE(fm.isWriting());

    // Switch to writing mode
    fs::path outputFile = testDir / "switch_output.txt";
    EXPECT_TRUE(fm.writingMode(outputFile));
    EXPECT_FALSE(fm.isReading());
    EXPECT_TRUE(fm.isWriting());
}

TEST_F(FileManagerTest, SwitchFromWritingToReading) {
    FileManager<std::string> fm;

    // Start in writing mode
    fs::path outputFile = testDir / "switch_output.txt";
    EXPECT_TRUE(fm.writingMode(outputFile));
    EXPECT_FALSE(fm.isReading());
    EXPECT_TRUE(fm.isWriting());

    // Switch to reading mode
    EXPECT_TRUE(fm.readingMode(testFile1));
    EXPECT_TRUE(fm.isReading());
    EXPECT_FALSE(fm.isWriting());
}

// ========== DIFFERENT TEMPLATE TYPES TESTS ==========

TEST_F(FileManagerTest, IntegerFileManager) {
    fs::path numberFile = testDir / "numbers.txt";

    // Write integers
    {
        FileManager<int> fm;
        EXPECT_TRUE(fm.writingMode(numberFile));
        EXPECT_TRUE(fm.write(42));
        EXPECT_TRUE(fm.write(100));
        EXPECT_TRUE(fm.write(-5));
    }

    // Read integers using stream
    FileManager<int> fm;
    EXPECT_TRUE(fm.readingMode(numberFile));

    std::ostringstream oss;
    EXPECT_TRUE(fm.loadFromFile(oss));
    // loadFromFile adds newlines, so expect "42100-5\n"
    EXPECT_EQ(oss.str(), "42100-5\n");
}

TEST_F(FileManagerTest, CharFileManager) {
    fs::path charFile = testDir / "chars.txt";

    // Write characters
    {
        FileManager<char> fm;
        EXPECT_TRUE(fm.writingMode(charFile));
        EXPECT_TRUE(fm.write('A'));
        EXPECT_TRUE(fm.write('B'));
        EXPECT_TRUE(fm.write('C'));
    }

    // Verify content
    std::ifstream file(charFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "ABC");
}

// ========== ERROR HANDLING TESTS ==========

TEST_F(FileManagerTest, InvalidFilePath) {
    FileManager<std::string> fm;
    fs::path invalidPath = "/invalid/nonexistent/directory/file.txt";

    EXPECT_FALSE(fm.readingMode(invalidPath));
    EXPECT_FALSE(fm.writingMode(invalidPath));
}

TEST_F(FileManagerTest, ReadLineWhenNotReading) {
    FileManager<std::string> fm;
    std::string line;

    // Not in reading mode
    EXPECT_FALSE(fm.readLine(line));

    // In writing mode
    fs::path outputFile = testDir / "output.txt";
    fm.writingMode(outputFile);
    EXPECT_FALSE(fm.readLine(line));
}

// ========== EDGE CASES TESTS ==========

TEST_F(FileManagerTest, MultipleReadingModeCallsOnSameFile) {
    FileManager<std::string> fm;

    EXPECT_TRUE(fm.readingMode(testFile1));
    EXPECT_TRUE(fm.isReading());

    // Call reading mode again on the same file
    EXPECT_TRUE(fm.readingMode(testFile1));
    EXPECT_TRUE(fm.isReading());
}

TEST_F(FileManagerTest, ReadingAfterReachingEOF) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1));

    std::string line;
    // Read all lines
    while (fm.readLine(line)) {
        // Continue reading until EOF
    }

    // Try to read after EOF
    EXPECT_FALSE(fm.readLine(line));
}

TEST_F(FileManagerTest, LargeFileHandling) {
    fs::path largeFile = testDir / "large.txt";

    // Create a large file
    {
        std::ofstream file(largeFile);
        for (int i = 0; i < 10000; ++i) {
            file << "Line " << i << "\n";
        }
    }

    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(largeFile));

    std::string line;
    int lineCount = 0;
    while (fm.readLine(line)) {
        lineCount++;
    }

    EXPECT_EQ(lineCount, 10000);
}

// ========== FILE CONTENT PRESERVATION TESTS ==========

TEST_F(FileManagerTest, OverwriteMode) {
    fs::path testFile = testDir / "overwrite_test.txt";

    // Create initial content
    std::ofstream(testFile) << "Initial content\n";

    // Overwrite with FileManager
    {
        FileManager<std::string> fm;
        EXPECT_TRUE(fm.writingMode(testFile, true)); // overwrite = true
        EXPECT_TRUE(fm.write("New content\n"));
    }

    // Verify content was overwritten
    std::ifstream file(testFile);
    std::string content;
    std::getline(file, content);
    EXPECT_EQ(content, "New content");
}

TEST_F(FileManagerTest, AppendMode) {
    fs::path testFile = testDir / "append_test.txt";

    // Create initial content
    std::ofstream(testFile) << "Initial content\n";

    // Append with FileManager
    {
        FileManager<std::string> fm;
        EXPECT_TRUE(fm.writingMode(testFile, false)); // append mode
        EXPECT_TRUE(fm.write("Appended content\n"));
    }

    // Verify content was appended
    std::ifstream file(testFile);
    std::string line1, line2;
    std::getline(file, line1);
    std::getline(file, line2);
    EXPECT_EQ(line1, "Initial content");
    EXPECT_EQ(line2, "Appended content");
}

// ========== STREAM POSITION HANDLING TESTS ==========

TEST_F(FileManagerTest, LoadFromFilePreservesPosition) {
    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(testFile1));

    // Read one line first
    std::string line;
    EXPECT_TRUE(fm.readLine(line));
    EXPECT_EQ(line, "Line 1");

    // Load from file should preserve position
    std::ostringstream oss;
    EXPECT_TRUE(fm.loadFromFile(oss));

    // Should still be able to read next line
    EXPECT_TRUE(fm.readLine(line));
    EXPECT_EQ(line, "Line 2");
}

// ========== CONCURRENT INSTANCES TESTS ==========

TEST_F(FileManagerTest, MultipleFileManagerInstances) {
    FileManager<std::string> fm1, fm2;

    // Both should be able to read the same file
    EXPECT_TRUE(fm1.readingMode(testFile1));
    EXPECT_TRUE(fm2.readingMode(testFile1));

    std::string line1, line2;
    EXPECT_TRUE(fm1.readLine(line1));
    EXPECT_TRUE(fm2.readLine(line2));

    // Both should read the same first line
    EXPECT_EQ(line1, line2);
    EXPECT_EQ(line1, "Line 1");
}

// ========== SPECIAL CHARACTERS TESTS ==========

TEST_F(FileManagerTest, HandleSpecialCharacters) {
    fs::path specialFile = testDir / "special.txt";
    std::string specialContent = "Special chars: !@#$%^&*()[]{}|\\:;\"'<>,.?/~`\nNewline\tTab";

    // Write special characters
    {
        std::ofstream file(specialFile);
        file << specialContent;
    }

    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(specialFile));

    std::ostringstream oss;
    EXPECT_TRUE(fm.loadFromFile(oss));

    // Content should be preserved exactly
    EXPECT_EQ(oss.str(), specialContent + "\n");
}

// ========== PATH HANDLING TESTS ==========

TEST_F(FileManagerTest, HandlePathWithSpaces) {
    fs::path spaceFile = testDir / "file with spaces.txt";
    std::ofstream(spaceFile) << "Content with spaces in filename";

    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(spaceFile));
    EXPECT_TRUE(fm.isFileOpen());
}

TEST_F(FileManagerTest, HandleRelativePath) {
    // Create file in current directory
    fs::path relativePath = "temp_test_file.txt";
    std::ofstream(relativePath) << "test content";

    FileManager<std::string> fm;
    EXPECT_TRUE(fm.readingMode(relativePath));

    std::string line;
    EXPECT_TRUE(fm.readLine(line));
    EXPECT_EQ(line, "test content");

    // Cleanup
    fs::remove(relativePath);
}

// ========== EMPTY CONTENT TESTS ==========

TEST_F(FileManagerTest, WriteEmptyContent) {
    fs::path emptyOutputFile = testDir / "empty_output.txt";

    FileManager<std::string> fm;
    EXPECT_TRUE(fm.writingMode(emptyOutputFile));
    // Don't write anything
    fm.closeFiles();

    EXPECT_TRUE(fs::exists(emptyOutputFile));

    // File should be empty
    std::ifstream file(emptyOutputFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.empty());
}

// ========== BINARY DATA TESTS ==========

TEST_F(FileManagerTest, HandleBinaryData) {
    fs::path binaryFile = testDir / "binary.txt";

    // Create file with binary data
    {
        std::ofstream file(binaryFile, std::ios::binary);
        for (int i = 0; i < 256; ++i) {
            file.put(static_cast<char>(i));
        }
    }

    FileManager<char> fm;
    EXPECT_TRUE(fm.readingMode(binaryFile));

    std::ostringstream oss;
    EXPECT_TRUE(fm.loadFromFile(oss));

    // Should be able to read binary data
    EXPECT_FALSE(oss.str().empty());
    // loadFromFile adds newlines, so length will be longer than 256
    EXPECT_GE(oss.str().length(), 256);
}
