#include "security.h"
#include "command.h"

namespace tools
{
    namespace security
    {

    using namespace tools::command;
    using namespace tools::exceptions;
    using namespace tools::util;

#ifdef __linux__
        bool AmIRoot()
        {
            return geteuid() == 0;
        }
#endif

        void SetFilePermissions(std::string directory, int perms);
        void SetFileOwner(std::string directory, std::string owner);

        #ifdef __linux__
        bool IsOwner(User user, std::string file)
        {
            struct stat info;
            if (stat(file.c_str(), &info) == 0)
            {
                if (user.uuid == info.st_uid)
                {
                    return true;
                }
            }
            return false;
        }

        bool IsGroupOwner(User user, std::string file)
        {
            struct stat info;
            if (stat(file.c_str(), &info) == 0)
            {
                if (user.guid == info.st_gid)
                {
                    return true;
                }
            }
            return false;
        }
        #elif _WIN32
        bool IsOwner(User user, std::string file)
        {
            return false;
        }

        bool IsGroupOwner(User user, std::string file)
        {
            return false;
        }
        #endif

        bool CanRead(User user, std::string file)
        {
            std::filesystem::perms filep;
            try
            {
                filep = std::filesystem::status(file).permissions();
            }
            catch (const std::exception &e)
            {
                return false;
            }
            if (IsOwner(user, file))
            {
                if ((filep & std::filesystem::perms::owner_read) != std::filesystem::perms::none)
                {
                    return true;
                }
                return false;
            }
            if (IsGroupOwner(user, file))
            {
                if ((filep & std::filesystem::perms::group_read) != std::filesystem::perms::none)
                {
                    return true;
                }
                return false;
            }
            if ((filep & std::filesystem::perms::others_read) != std::filesystem::perms::none)
            {
                return true;
            }
            return false;
        }
        bool CanWrite(User user, std::string file)
        {
            std::filesystem::perms filep;
            try
            {
                filep = std::filesystem::status(file).permissions();
            }
            catch (const std::exception &e)
            {
                return false;
            }
            if (IsOwner(user, file))
            {
                if ((filep & std::filesystem::perms::owner_write) != std::filesystem::perms::none)
                {
                    return true;
                }
                return false;
            }
            if (IsGroupOwner(user, file))
            {
                if ((filep & std::filesystem::perms::group_write) != std::filesystem::perms::none)
                {
                    return true;
                }
                return false;
            }
            if ((filep & std::filesystem::perms::others_write) != std::filesystem::perms::none)
            {
                return true;
            }
            return false;
        }
        bool CanExecute(User user, std::string file)
        {
            std::filesystem::perms filep;
            try
            {
                filep = std::filesystem::status(file).permissions();
            }
            catch (const std::exception &e)
            {
                return false;
            }
            if (IsOwner(user, file))
            {
                if ((filep & std::filesystem::perms::owner_exec) != std::filesystem::perms::none)
                {
                    return true;
                }
                return false;
            }
            if (IsGroupOwner(user, file))
            {
                if ((filep & std::filesystem::perms::group_exec) != std::filesystem::perms::none)
                {
                    return true;
                }
                return false;
            }
            if ((filep & std::filesystem::perms::others_exec) != std::filesystem::perms::none)
            {
                return true;
            }
            return false;
        }

        User GetCurrentUser()
        {
            return GetUser(RecursiveReplace(Split(Execute("whoami"), "\\")[1], "\n", ""));
        }

        #ifdef __linux__
        User GetUser(std::string user)
        {
            std::string output = Execute("id " + user);
            if (output.find("no such user") != std::string::npos)
            {
                throw UserDoesntExist("Given user does not exist");
            }
            output = output.substr(0, output.size() - 2);
            User userobj;
            userobj.uuid = std::stoi(Split(Split(Split(output, " ")[0], "=")[1], "(")[0]);
            userobj.guid = std::stoi(Split(Split(Split(output, " ")[1], "=")[1], "(")[0]);
            userobj.name = Split(Split(Split(output, " ")[0], "(")[1], ")")[0];
            std::string groups = Split(output, "groups=")[1];
            std::vector<std::string> groupsvec = Split(groups, "),");
            userobj.groups = new std::pair<int, std::string>[groupsvec.size()];
            for (int i = 0; i < groupsvec.size(); i++)
            {
                userobj.groups[i] = std::make_pair(std::stoi(Split(groupsvec[i], "(")[0]), Split(groupsvec[i], "(")[1]);
            }
            userobj.groupamount = groupsvec.size();
            return userobj;
        }

        std::vector<Group> GetGroups()
        {
            std::string directory = "/etc/group";
            try
            {
                if (!std::filesystem::exists(directory))
                {
                    throw FileDoesntExist("Specified file or directory does not exist");
                }
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                throw AccessNotPermitted("Invalid permissions to access /etc/group file");
            }
            std::filesystem::directory_entry filetoread = std::filesystem::directory_entry(directory);
            switch (std::filesystem::status(directory).type())
            {
            case std::filesystem::file_type::regular:
            {
                if (!CanRead(GetCurrentUser(), directory))
                {
                    throw ReadNotPermitted("Invalid permissions to read /etc/group file");
                }
                break;
            }
            default:
            {
                throw Unsupported("/etc/group is of a file type that is unsupported");
            }
            }
            std::ifstream file("/etc/group");
            std::string line;
            std::vector<Group> groups;
            while (std::getline(file, line))
            {
                std::vector<std::string> group = Split(line, ":");
                std::vector<User> groupusers;
                for (int i = 3; i < group.size(); i++)
                {
                    if (group[i] == "")
                    {
                        break;
                    }
                    groupusers.push_back(GetUser(group[i]));
                }
                Group groupobj = Group();
                groupobj.groupsize = groupusers.size();
                groupobj.usersingroup = new User[groupusers.size()];
                for (int i = 0; i < groupusers.size(); i++)
                {
                    groupobj.usersingroup[i] = groupusers[i];
                }
                groupobj.groupid = std::stoi(group[2]);
                groupobj.name = group[0];
                groups.push_back(groupobj);
            }
            return groups;
        }

        Group GetGroup(std::string groupstr)
        {
            std::string directory = "/etc/group";
            try
            {
                if (!std::filesystem::exists(directory))
                {
                    throw FileDoesntExist("Specified file or directory does not exist");
                }
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                throw AccessNotPermitted("Invalid permissions to access /etc/group file");
            }
            std::filesystem::directory_entry filetoread = std::filesystem::directory_entry(directory);
            switch (std::filesystem::status(directory).type())
            {
            case std::filesystem::file_type::regular:
            {
                if (!CanRead(GetCurrentUser(), directory))
                {
                    throw ReadNotPermitted("Invalid permissions to read /etc/group file");
                }
                break;
            }
            default:
            {
                throw Unsupported("/etc/group is of a file type that is unsupported");
            }
            }
            std::ifstream file("/etc/group");
            std::string line;
            while (std::getline(file, line))
            {
                std::vector<std::string> group = Split(line, ":");
                if (group[0] == groupstr)
                {
                    std::vector<User> groupusers;
                    for (int i = 3; i < group.size(); i++)
                    {
                        if (group[i] == "")
                        {
                            break;
                        }
                        groupusers.push_back(GetUser(group[i]));
                    }
                    Group groupobj = Group();
                    groupobj.groupsize = groupusers.size();
                    groupobj.usersingroup = new User[groupusers.size()];
                    for (int i = 0; i < groupusers.size(); i++)
                    {
                        groupobj.usersingroup[i] = groupusers[i];
                    }
                    groupobj.groupid = std::stoi(group[2]);
                    groupobj.name = group[0];
                    return groupobj;
                }
            }
            throw PermissionObjectDoesntExist("Specified group does not exist");
        }
        #elif _WIN32
        User GetUser(std::string user)
        {
            std::string output = Execute("net USER " + RecursiveReplace(user, "\n", ""));
            if (output.find("The user name could not be found.") != std::string::npos)
            {
                throw UserDoesntExist("Given user does not exist");
            }
            User userobj;
            std::vector<std::string> splits = Split(RecursiveReplace(RecursiveReplace(output, "  ", " "), "-", ""), "\n");
            userobj.name = user;
            for (int i = 0; i < Split(splits[20], " ").size() - 1; i++) {
                userobj.groups.push_back(Split(splits[20], " ")[i+1].substr(i+1, Split(splits[20], " ")[i+1].size() - 1));
            }
            return userobj;
        }

        Group GetGroup(std::string groupstr)
        {
            std::string output = Execute("net LOCALGROUP " + groupstr);
            if (output.find("The specified local group does not exist.") != std::string::npos)
            {
                throw PermissionObjectDoesntExist("Specified group does not exist");
            }
            Group groupobj;
            std::vector<std::string> splits = Split(RecursiveReplace(RecursiveReplace(output, "  ", " "), "-", ""), "\n");
            groupobj.name = groupstr;
            groupobj.groupsize = splits.size() - 7;
            groupobj.usersingroup = new User[groupobj.groupsize];
            for (int i = 6; i < splits.size() - 1; i++) {
                User user = GetUser(splits[i]);
                groupobj.usersingroup[i-6] = user;
            }
            return groupobj;
        }

        std::vector<Group> GetGroups()
        {
            std::string output = Execute("net LOCALGROUP");
            std::vector<std::string> splits = Split(RecursiveReplace(RecursiveReplace(output, "-", ""), "*", ""), "\n");
            std::vector<Group> groups;
            for (int i = 4; i < splits.size() - 1; i++) {
                groups.push_back(GetGroup(splits[i]));
            }
            return groups;
        }
        #endif
    }
}