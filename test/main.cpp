#include "../src/io.h"
#include "../src/security.h"
#include "../src/command.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

// IO TESTS

TEST_CASE("Get current directory")
{
    std::string cd = GetCurrentDirectory();
    REQUIRE(cd == std::filesystem::current_path());
}

TEST_CASE("Get current directory fileobject")
{
    std::string cd = GetCurrentDirectory();
    FileObject *cdfo = GetFile(cd);
    REQUIRE(cdfo != nullptr);
    REQUIRE(!cdfo->isfile);
    Folder *collapsedcd = static_cast<Folder *>(cdfo);
    REQUIRE(collapsedcd->name == std::filesystem::current_path());
}

TEST_CASE("List current directory")
{
    FileObject *cd = ListCurrentDirectory();
    REQUIRE(!cd->isfile);
    Folder *collapsedcd = static_cast<Folder *>(cd);
    CHECK(collapsedcd->name == std::filesystem::current_path());
    REQUIRE(collapsedcd->fileamount == 9);
}

TEST_CASE("List test directory")
{
    FileObject *cd = ListDirectory(std::filesystem::absolute("tests"));
    REQUIRE(!cd->isfile);
    Folder *collapsedcd = static_cast<Folder *>(cd);
    CHECK(collapsedcd->name == std::filesystem::absolute("tests"));
    REQUIRE(collapsedcd->fileamount == 2);
    std::vector<std::string> files = {std::filesystem::absolute("tests/lowpermfolder"), std::filesystem::absolute("tests/highpermfolder")};
    for (int i = 0; i < files.size(); i++)
    {
        CHECK(collapsedcd->files[i]->name == files[i]);
    }
}

TEST_CASE("Check listing high perm folder produces correct error")
{
    CHECK_THROWS_AS(ListDirectory(std::filesystem::absolute("tests/highpermfolder")), ExecuteNotPermitted);
}

TEST_CASE("Check reading low perm file in high perm folder produces correct error")
{
    CHECK_THROWS_AS(ReadFile(std::filesystem::absolute("tests/highpermfolder/folder/folder/lowpermfile")), AccessNotPermitted);
}

TEST_CASE("Check listing low perm folder in high perm folder produces correct error")
{
    CHECK_THROWS_AS(ListDirectory(std::filesystem::absolute("tests/highpermfolder/folder")), ExecuteNotPermitted);
}

TEST_CASE("Recursively list with depth 0 returns bare folder without children")
{
    // depth 0 falls through to GetFile(), so no child enumeration occurs
    FileObject *result = ListDirectoryRecursive(std::filesystem::absolute("tests"), 0);
    REQUIRE(result != nullptr);
    REQUIRE(!result->isfile);
    Folder *folder = static_cast<Folder *>(result);
    CHECK(folder->name == std::filesystem::absolute("tests").string());
}

TEST_CASE("Recursively list depth 1 on tests/ returns top-level entries only")
{
    // depth 1 lists tests/ children, then calls depth 0 on each — never tries to enter highpermfolder
    FileObject *result = ListDirectoryRecursive(std::filesystem::absolute("tests"), 1);
    REQUIRE(result != nullptr);
    REQUIRE(!result->isfile);
    Folder *folder = static_cast<Folder *>(result);
    REQUIRE(folder->fileamount == 2);
}

TEST_CASE("Recursively list a regular file at any depth returns the file")
{
    std::string filepath = std::filesystem::absolute("src/io.cpp").string();
    FileObject *result = ListDirectoryRecursive(filepath, 3);
    REQUIRE(result != nullptr);
    REQUIRE(result->isfile);
}

TEST_CASE("ListCurrentDirectoryRecursive depth 0 returns bare current directory")
{
    FileObject *result = ListCurrentDirectoryRecursive(0);
    REQUIRE(result != nullptr);
    REQUIRE(!result->isfile);
    CHECK(result->name == std::filesystem::current_path().string());
}

TEST_CASE("GetAbsoluteDirectory resolves a relative path to an absolute path")
{
    std::string abs = GetAbsoluteDirectory("src");
    CHECK(abs == std::filesystem::absolute("src").string());
    CHECK(abs.find("src") != std::string::npos);
    CHECK(abs.front() == '/');
}

TEST_CASE("GetAbsoluteDirectory on dot produces a path containing the current directory")
{
    std::string abs = GetAbsoluteDirectory(".");
    std::string cwd = std::filesystem::current_path().string();
    // filesystem::absolute does not normalize trailing /. components
    CHECK(abs.find(cwd) != std::string::npos);
    CHECK(abs.front() == '/');
}

TEST_CASE("GetFile on a regular file returns isfile true with correct name")
{
    std::string filepath = std::filesystem::absolute("src/io.cpp").string();
    FileObject *fo = GetFile(filepath);
    REQUIRE(fo != nullptr);
    CHECK(fo->isfile);
    CHECK(fo->name == filepath);
    CHECK(fo->permissions != 0);
}

TEST_CASE("GetFile on a directory returns isfile false")
{
    std::string dirpath = std::filesystem::absolute("src").string();
    FileObject *fo = GetFile(dirpath);
    REQUIRE(fo != nullptr);
    CHECK(!fo->isfile);
    CHECK(fo->name == dirpath);
}

TEST_CASE("GetFile on a non-existent path throws FileDoesntExist")
{
    CHECK_THROWS_AS(GetFile("/this/path/does/not/exist"), FileDoesntExist);
}

TEST_CASE("GetFiles returns correctly typed entries for a mixed array")
{
    std::string paths[2] = {
        std::filesystem::absolute("src").string(),
        std::filesystem::absolute("src/io.cpp").string()
    };
    FileObject **files = GetFiles(paths, 2);
    REQUIRE(files != nullptr);
    REQUIRE(files[0] != nullptr);
    REQUIRE(files[1] != nullptr);
    CHECK(!files[0]->isfile);   // src/ is a directory
    CHECK(files[1]->isfile);    // src/io.cpp is a file
    CHECK(files[0]->name == paths[0]);
    CHECK(files[1]->name == paths[1]);
}

TEST_CASE("GetFiles with two regular files both return isfile true")
{
    std::string paths[2] = {
        std::filesystem::absolute("src/io.cpp").string(),
        std::filesystem::absolute("src/security.cpp").string()
    };
    FileObject **files = GetFiles(paths, 2);
    REQUIRE(files != nullptr);
    CHECK(files[0]->isfile);
    CHECK(files[1]->isfile);
}

TEST_CASE("ReadFile on a non-existent path throws FileDoesntExist")
{
    CHECK_THROWS_AS(ReadFile("/no/such/file/at/all"), FileDoesntExist);
}

TEST_CASE("ReadFile on a directory throws Unsupported")
{
    CHECK_THROWS_AS(ReadFile(std::filesystem::absolute("src").string()), Unsupported);
}

TEST_CASE("ReadFile on a world-readable file returns a valid file object")
{
    // lowpermfile is chmod 777 so testuser (others) can read it
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/lowpermfile").string();
    FileObject *fo = ReadFile(filepath);
    REQUIRE(fo != nullptr);
    CHECK(fo->isfile);
    CHECK(fo->name == filepath);
}

TEST_CASE("ReadFile on a root-only file throws ReadNotPermitted")
{
    // highpermfile is chmod 700 owned by root, so testuser cannot read it
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/highpermfile").string();
    CHECK_THROWS_AS(ReadFile(filepath), ReadNotPermitted);
}

TEST_CASE("ListDirectory on lowpermfolder returns one entry (the folder/ subdir)")
{
    FileObject *cd = ListDirectory(std::filesystem::absolute("tests/lowpermfolder"));
    REQUIRE(!cd->isfile);
    Folder *folder = static_cast<Folder *>(cd);
    REQUIRE(folder->fileamount == 1);
    CHECK(!folder->files[0]->isfile);
}

TEST_CASE("ListDirectory on a non-existent path throws")
{
    CHECK_THROWS(ListDirectory("/no/such/directory/here"));
}

TEST_CASE("MoveDirectory on a regular file throws InvalidFileType")
{
    CHECK_THROWS_AS(
        MoveDirectory(std::filesystem::absolute("src/io.cpp").string()),
        InvalidFileType
    );
}

TEST_CASE("MoveDirectory on a non-existent path throws FileDoesntExist")
{
    CHECK_THROWS_AS(MoveDirectory("/this/does/not/exist"), FileDoesntExist);
}

TEST_CASE("Folder ToString contains name, permissions, owner, and file count")
{
    Folder f;
    f.isfile = false;
    f.name = "testfolder";
    f.permissions = 755;
    f.owner = "alice";
    f.fileamount = 3;
    f.files = nullptr;
    std::string s = f.ToString();
    CHECK(s.find("testfolder") != std::string::npos);
    CHECK(s.find("755") != std::string::npos);
    CHECK(s.find("alice") != std::string::npos);
    CHECK(s.find("3") != std::string::npos);
}

TEST_CASE("File ToString contains name, permissions, owner, and size")
{
    File f;
    f.isfile = true;
    f.name = "myfile.txt";
    f.permissions = 644;
    f.owner = "bob";
    f.contentssize = 1024;
    f.contents = nullptr;
    f.retrievedcontent = false;
    std::string s = f.ToString();
    CHECK(s.find("myfile.txt") != std::string::npos);
    CHECK(s.find("644") != std::string::npos);
    CHECK(s.find("bob") != std::string::npos);
    CHECK(s.find("1024") != std::string::npos);
}

// SECURITY TESTS

TEST_CASE("Check permission on high perms folder")
{
    CHECK(!CanRead(GetCurrentUser(), std::filesystem::absolute("tests/highpermfolder")));
    CHECK(!CanWrite(GetCurrentUser(), std::filesystem::absolute("tests/highpermfolder")));
    CHECK(!CanExecute(GetCurrentUser(), std::filesystem::absolute("tests/highpermfolder")));
}

TEST_CASE("Checks if you can grab group file")
{
    CHECK_NOTHROW(GetGroups());
}

TEST_CASE("Checks if root group exists")
{
    CHECK(GetGroup("root").groupid == 0);
    CHECK(GetGroup("root").name == "root");
    CHECK(GetGroup("root").groupsize == 0);
}

TEST_CASE("CanRead returns true for a world-readable file")
{
    // lowpermfile is chmod 777 — others_read is set
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/lowpermfile").string();
    CHECK(CanRead(GetCurrentUser(), filepath));
}

TEST_CASE("CanRead returns false for a root-only file")
{
    // highpermfile is chmod 700 — others have no permissions
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/highpermfile").string();
    CHECK(!CanRead(GetCurrentUser(), filepath));
}

TEST_CASE("CanWrite returns true for a world-writable file")
{
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/lowpermfile").string();
    CHECK(CanWrite(GetCurrentUser(), filepath));
}

TEST_CASE("CanWrite returns false for a root-only file")
{
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/highpermfile").string();
    CHECK(!CanWrite(GetCurrentUser(), filepath));
}

TEST_CASE("CanExecute returns true for a world-executable file")
{
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/lowpermfile").string();
    CHECK(CanExecute(GetCurrentUser(), filepath));
}

TEST_CASE("CanExecute returns false for a root-only file")
{
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/highpermfile").string();
    CHECK(!CanExecute(GetCurrentUser(), filepath));
}


TEST_CASE("IsOwner returns false for a root-owned file")
{
    // all test fixture files are owned by root
    std::string filepath = std::filesystem::absolute("tests/lowpermfolder/folder/folder/lowpermfile").string();
    CHECK(!IsOwner(GetCurrentUser(), filepath));
}

TEST_CASE("IsOwner returns false for /etc/passwd")
{
    CHECK(!IsOwner(GetCurrentUser(), "/etc/passwd"));
}

TEST_CASE("IsGroupOwner returns false for root-owned files when not in root group")
{
    CHECK(!IsGroupOwner(GetCurrentUser(), "/etc/passwd"));
}

TEST_CASE("GetCurrentUser returns a valid user with a non-empty name and at least one group")
{
    User u = GetCurrentUser();
    CHECK(!u.name.empty());
    CHECK(u.groupamount > 0);
}

TEST_CASE("GetUser for root has uuid 0 and name root")
{
    User root = GetUser("root");
    CHECK(root.uuid == 0);
    CHECK(root.name == "root");
}

TEST_CASE("GetGroup for non-existent group throws PermissionObjectDoesntExist")
{
    CHECK_THROWS_AS(GetGroup("this_group_does_not_exist_xyz"), PermissionObjectDoesntExist);
}

TEST_CASE("GetGroups returns a non-empty list")
{
    std::vector<Group> groups = GetGroups();
    CHECK(groups.size() > 0);
}
