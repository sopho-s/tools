#include "../src/io.h"
#include "../src/security.h"
#include "../src/command.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

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

TEST_CASE("Check permission on high perms folder")
{
    CHECK(!CanRead(GetCurrentUser(), std::filesystem::absolute("tests/highpermfolder")));
    CHECK(!CanWrite(GetCurrentUser(), std::filesystem::absolute("tests/highpermfolder")));
    CHECK(!CanExecute(GetCurrentUser(), std::filesystem::absolute("tests/highpermfolder")));
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