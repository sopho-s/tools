#include "io.h"
#include "security.h"

std::string GetCurrentDirectory()
{
    std::string currentdir = "";
    currentdir = std::filesystem::current_path().string();
    return currentdir;
}
std::string GetAbsoluteDirectory(std::string directory)
{
    std::string absolutedir = "";
    absolutedir = std::filesystem::absolute(directory).string();
    return absolutedir;
}

FileObject *GetFile(std::string directory)
{
    try {
        if (!std::filesystem::exists(directory))
        {
            throw FileDoesntExist("Specified file or directory does not exist");
        }
    } catch (const std::filesystem::filesystem_error &e) {
        throw AccessNotPermitted("Invalid permissions to access file");
    }
    std::filesystem::directory_entry dirtocheck = std::filesystem::directory_entry(directory);
    switch (std::filesystem::status(directory).type())
    {
    case std::filesystem::file_type::directory:
    {
        Folder *dir = new Folder();
        dir->name = directory;
        dir->permissions = static_cast<int>(std::filesystem::status(directory).permissions());
        return dir;
    }
    case std::filesystem::file_type::regular:
    {
        File *file = new File();
        file->isfile = true;
        file->name = directory;
        file->permissions = static_cast<int>(std::filesystem::status(directory).permissions());
        return file;
    }
    default:
    {
        throw Unsupported("Specified file type is unsupported");
    }
    }
    return nullptr;
}
FileObject **GetFiles(std::string *directories, int amount);
FileObject *ReadFile(std::string directory) {
    try {
        if (!std::filesystem::exists(directory))
        {
            throw FileDoesntExist("Specified file or directory does not exist");
        }
    } catch (const std::filesystem::filesystem_error &e) {
        throw AccessNotPermitted("Invalid permissions to access file");
    }
    std::filesystem::directory_entry filetoread = std::filesystem::directory_entry(directory);
    switch (std::filesystem::status(directory).type())
    {
    case std::filesystem::file_type::regular:
    {
        File *file = new File();
        file->isfile = true;
        file->name = directory;
        file->permissions = static_cast<int>(std::filesystem::status(directory).permissions());
        if (!CanRead(GetCurrentUser(), directory)) {
            throw ReadNotPermitted("Invalid permissions to read file");
        }
        return file;
    }
    default:
    {
        throw Unsupported("Specified file type is unsupported");
    }
    }
}
FileObject **ReadFiles(std::string *directories, int amount);

FileObject *ListDirectory(std::string directory)
{
    if (!CanExecute(GetCurrentUser(), directory)) {
        throw ExecuteNotPermitted("Invalid permissions to list directory");
    }
    FileObject *currentdir = GetFile(directory);
    std::vector<FileObject *> files;
    for (std::filesystem::directory_entry const &dir : std::filesystem::directory_iterator(directory))
    {
        std::string path = dir.path().string();
        FileObject *currentfile = GetFile(path);
        files.push_back(currentfile);
    }
    Folder *collapseddir = new Folder();
    collapseddir = static_cast<Folder *>(currentdir);
    collapseddir->files = new FileObject *[files.size()];
    for (int i = 0; i < files.size(); i++)
    {
        collapseddir->files[i] = files[i];
    }
    collapseddir->fileamount = files.size();
    currentdir = collapseddir;
    return currentdir;
}
FileObject *ListDirectoryRecursive(std::string directory, int depth);
FileObject *ListCurrentDirectory()
{
    std::string currentdirname = GetCurrentDirectory();
    return ListDirectory(currentdirname);
}
FileObject *ListCurrentDirectoryRecursive(std::string directory, int depth);

void RemoveFile(std::string directory);
void AddFile(std::string directory, FileObject file);
void ModifyFile(std::string directory, FileObject file);

void MoveDirectory(std::string directory)
{
    FileObject *folder = GetFile(directory);
    if (folder->isfile)
    {
        throw InvalidFileType("Specified directory is not a folder");
    }
    Folder *collapsedfolder = static_cast<Folder *>(folder);
    std::filesystem::current_path(directory);
}