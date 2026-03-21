#include "security.h"
#include "command.h"
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <pwd.h>

std::vector<std::string> split(std::string s, std::string delim)
{
    std::vector<std::string> res;
    int pos = 0;
    while (true)
    {
        pos = s.find(delim);
        if (pos == -1) {
            res.push_back(s);
            break;
        }
        res.push_back(s.substr(0, pos));
        s.erase(0, pos + delim.size());
    }
    return res;
}

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

bool CanRead(User user, std::string file)
{
    std::filesystem::perms filep;
    try {
        filep = std::filesystem::status(file).permissions();
    } catch (const std::exception &e) {
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
    try {
        filep = std::filesystem::status(file).permissions();
    } catch (const std::exception &e) {
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
    try {
        filep = std::filesystem::status(file).permissions();
    } catch (const std::exception &e) {
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
    return GetUser(Execute("whoami"));
}
User GetUser(std::string user)
{
    std::string output = Execute("id " + user);
    output = output.substr(0, output.size() - 2);
    User userobj;
    userobj.uuid = std::stoi(split(split(split(output, " ")[0], "=")[1], "(")[0]);
    userobj.guid = std::stoi(split(split(split(output, " ")[1], "=")[1], "(")[0]);
    userobj.name = split(split(split(output, " ")[0], "(")[1], ")")[0];
    std::string groups = split(output, "groups=")[1];
    std::vector<std::string> groupsvec = split(groups, "),");
    userobj.groups = new std::pair<int, std::string>[groups.size()];
    for (int i = 0; i < groupsvec.size(); i++)
    {
        userobj.groups[i] = std::make_pair(std::stoi(split(groupsvec[i], "(")[0]), split(groupsvec[i], "(")[1]);
    }
    userobj.groupamount = groupsvec.size();
    return userobj;
}
std::vector<Group> GetGroups();
Group GetGroup(std::string group);