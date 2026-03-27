#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "exceptions.h"
#pragma once

/// @brief Base structure representing a filesystem entry (file or folder)
struct FileObject {
    bool isfile;          ///< true if this is a regular file, false if a directory
    std::string name;     ///< Name of the file or folder
    int permissions;      ///< Permission bits (e.g. Unix-style octal)
    std::string owner;    ///< Name of the owning user
};

/// @brief Represents a directory, extending FileObject with its contents
struct Folder : public FileObject {
    FileObject** files;  ///< Array of pointers to the folder's contents
    int fileamount;      ///< Number of entries in the files array

    /// @brief Returns a human-readable summary of the folder's metadata
    /// @return Formatted string with name, permissions, owner, and file count
    std::string ToString() {
        std::string folderrep = "";
        folderrep += "Foldername: " + name;
        folderrep += "\nPermissions: " + std::to_string(permissions);
        folderrep += "\nOwner: " + owner;
        folderrep += "\nFile amount: " + std::to_string(fileamount);
        return folderrep;
    }
};


/// @brief Represents a regular file, extending FileObject with its contents
struct File : public FileObject {
    char* contents;          ///< Raw file contents buffer
    int contentssize;        ///< Size of the contents buffer in bytes
    bool retrievedcontent;   ///< true if the file contents have been loaded

    /// @brief Returns a human-readable summary of the file's metadata
    /// @return Formatted string with name, permissions, owner, and size
    std::string ToString() {
        std::string filerep = "";
        filerep += "Filename: " + name;
        filerep += "\nPermissions: " + std::to_string(permissions);
        filerep += "\nOwner: " + owner;
        filerep += "\nSize: " + std::to_string(contentssize);
        return filerep;
    }
};

/// @brief Checks whether the current user has the specified permissions on a directory
/// @param directory Path to the directory to check
/// @param read Whether to check for read permission
/// @param write Whether to check for write permission
/// @param execute Whether to check for execute permission
/// @return true if all requested permissions are granted, false otherwise
bool DoIHaveValidPermissions(std::string directory, bool read = false, bool write = false, bool execute = false);

/// @brief Returns the current working directory path
/// @return Absolute path of the current directory
std::string GetCurrentDirectory();

/// @brief Resolves a relative path to an absolute path
/// @param directory The relative or partial directory path
/// @return The absolute path string
std::string GetAbsoluteDirectory(std::string directory);

/// @brief Lists entries in the current working directory (non-recursive)
/// @return Pointer to a FileObject representing the current directory
/// @throws ExecuteNotPermitted if the current user lacks execute permission on the directory
/// @throws FileDoesntExist if the directory does not exist
/// @throws AccessNotPermitted if a permission error prevents checking existence
/// @throws Unsupported if an entry's file type is not a regular file or directory
FileObject* ListCurrentDirectory();

/// @brief Lists entries in the current working directory up to a given depth
/// @param depth Maximum recursion depth
/// @return Pointer to a FileObject representing the directory tree
FileObject* ListCurrentDirectoryRecursive(int depth);

/// @brief Lists entries in the specified directory (non-recursive)
/// @param directory Path to the directory to list
/// @return Pointer to a FileObject representing the directory
/// @throws ExecuteNotPermitted if the current user lacks execute permission on the directory
/// @throws FileDoesntExist if the directory does not exist
/// @throws AccessNotPermitted if a permission error prevents checking existence
/// @throws Unsupported if an entry's file type is not a regular file or directory
FileObject* ListDirectory(std::string directory);

/// @brief Lists entries in the specified directory up to a given depth
/// @param directory Path to the directory to list
/// @param depth Maximum recursion depth
/// @return Pointer to a FileObject representing the directory tree
FileObject* ListDirectoryRecursive(std::string directory, int depth);

/// @brief Retrieves metadata for a single file without reading its contents
/// @param directory Path to the file
/// @return Pointer to a FileObject with metadata populated
/// @throws FileDoesntExist if the path does not exist
/// @throws AccessNotPermitted if a permission error prevents checking existence
/// @throws Unsupported if the file type is not a regular file or directory
FileObject* GetFile(std::string directory);

/// @brief Retrieves metadata for multiple files without reading their contents
/// @param directories Array of file paths
/// @param amount Number of paths in the array
/// @return Array of FileObject pointers with metadata populated
FileObject** GetFiles(std::string* directories, int amount);

/// @brief Reads a file and populates its contents
/// @param directory Path to the file
/// @return Pointer to a FileObject with contents loaded
/// @throws FileDoesntExist if the path does not exist
/// @throws AccessNotPermitted if a permission error prevents checking existence
/// @throws ReadNotPermitted if the current user lacks read permission on the file
/// @throws Unsupported if the file type is not a regular file
FileObject* ReadFile(std::string directory);

/// @brief Reads multiple files and populates their contents
/// @param directories Array of file paths
/// @param amount Number of paths in the array
/// @return Array of FileObject pointers with contents loaded
FileObject** ReadFiles(std::string* directories, int amount);

/// @brief Removes a file from the filesystem
/// @param directory Path to the file to delete
void RemoveFile(std::string directory);

/// @brief Creates a new file at the specified path
/// @param directory Path where the file should be created
/// @param file FileObject describing the file to add
void AddFile(std::string directory, FileObject file);

/// @brief Overwrites an existing file with new data
/// @param directory Path to the file to modify
/// @param file FileObject containing the updated data
void ModifyFile(std::string directory, FileObject file);

/// @brief Moves a directory to a new location
/// @param directory Destination path for the directory
/// @throws InvalidFileType if the path points to a regular file rather than a directory
/// @throws FileDoesntExist if the path does not exist
/// @throws AccessNotPermitted if a permission error prevents checking existence
/// @throws Unsupported if the file type is not a regular file or directory
void MoveDirectory(std::string directory);
