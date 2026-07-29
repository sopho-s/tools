#include <string>
#include <cstring>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include "exceptions.h"
#include "command.h"
#include "util.h"
#include <vector>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#ifdef __linux__
#include <pwd.h>
#include <pwd.h>
#include <unistd.h>
#endif
#pragma once

namespace tools
{
    namespace security
    {
        #ifdef _WIN32
        #define WELL_KNOWN_GROUP 0b1
        #define ALIAS 0b10
        #define LABEL 0b100
        #endif
        /// @brief Represents a system user with identity and group membership information
        struct User
        {
            #ifdef __linux__
            unsigned int guid;                   ///< Group ID of the user's primary group
            std::pair<int, std::string> *groups; ///< Array of (gid, name) pairs for all groups the user belongs to
            unsigned int uuid;                   ///< User ID
            #elif _WIN32
            std::vector<std::string> groups;
            #endif
            int groupamount;                     ///< Number of entries in the groups array
            std::string name;                    ///< Username
            ~User()
            {
                ;
            }
        };

        /// @brief Represents a system group and its member users
        struct Group
        {
            #ifdef __linux__
            int groupid;
            #endif
            std::string name;   ///< Group name
            User *usersingroup; ///< Array of users belonging to this group
            int groupsize;      ///< Number of users in the group
            ~Group()
            {
                // delete usersingroup;
            }
        };

#ifdef __linux__
        bool AmIRoot();
#endif

        void SetFilePermissions(std::string directory, int perms);
        void SetFileOwner(std::string directory, std::string owner);

        /// @brief Checks whether the given user is the owner of a file
        /// @param user The user to check
        /// @param file Path to the file
        /// @return true if the user owns the file, false otherwise
        bool IsOwner(User user, std::string file);

        /// @brief Checks whether the given user belongs to the group that owns a file
        /// @param user The user to check
        /// @param file Path to the file
        /// @return true if the user is in the file's owning group, false otherwise
        bool IsGroupOwner(User user, std::string file);

        /// @brief Checks whether the given user has read permission on a file
        /// @param user The user to check
        /// @param file Path to the file
        /// @return true if read is permitted, false otherwise
        bool CanRead(User user, std::string file);

        /// @brief Checks whether the given user has write permission on a file
        /// @param user The user to check
        /// @param file Path to the file
        /// @return true if write is permitted, false otherwise
        bool CanWrite(User user, std::string file);

        /// @brief Checks whether the given user has execute permission on a file
        /// @param user The user to check
        /// @param file Path to the file
        /// @return true if execute is permitted, false otherwise
        bool CanExecute(User user, std::string file);

        /// @brief Returns the User representing the currently running process's user
        /// @return The current user
        User GetCurrentUser();

        /// @brief Looks up a user by name
        /// @param name The username to search for
        /// @return The matching User
        User GetUser(std::string name);

        /// @brief Returns all groups on the system
        /// @return A vector of all Group objects
        std::vector<Group> GetGroups();

        /// @brief Looks up a group by name
        /// @param group The group name to search for
        /// @return The matching Group
        Group GetGroup(std::string group);

    }
}