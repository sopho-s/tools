#include <bits/stdc++.h>
#pragma once

/// @brief Thrown when a type is either intentionally or unintentionally unsupported
class Unsupported : public std::exception {
    private:
        std::string message;
    public:
        Unsupported(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};

/// @brief Thrown when a file given does not exist
class FileDoesntExist : public std::exception {
    private:
        std::string message;
    public:
        FileDoesntExist(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};

/// @brief Thrown when a file (directory, regular, block, etc) is not of the correct type
class InvalidFileType : public std::exception {
    private:
        std::string message;
    public:
        InvalidFileType(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};

/// @brief Thrown when trying to read a file without the correct permissions
class ReadNotPermitted : public std::exception {
    private:
        std::string message;
    public:
        ReadNotPermitted(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};

/// @brief Thrown when trying to execute a file without the correct permissions
class ExecuteNotPermitted : public std::exception {
    private:
        std::string message;
    public:
        ExecuteNotPermitted(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};

/// @brief Thrown when trying to access a file but it was unknown where in the folder chain it was that blocked you from accessing the file
class AccessNotPermitted : public std::exception {
    private:
        std::string message;
    public:
        AccessNotPermitted(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};


class PermissionObjectDoesntExist : public std::exception {
    private:
        std::string message;
    public:
        PermissionObjectDoesntExist(const char* msg) : message(msg) {}

        const char* what() const noexcept {
            return message.c_str();
        }
};