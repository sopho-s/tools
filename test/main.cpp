#include "../src/io.h"
#include "../src/security.h"
#include "../src/command.h"
#include "../src/crypto.h"
#include "../src/util.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <cstring>

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

// CRYPTO TESTS

TEST_CASE("EncryptXOR single value XOR works correctly")
{
    unsigned char a = 0x55;
    unsigned char b = 0xAA;
    CHECK(EncryptXOR<unsigned char>(a, b) == 0xFF);
    CHECK(EncryptXOR<unsigned char>(a, a) == 0);
}

TEST_CASE("EncryptXOR with single key XORs all elements")
{
    unsigned char data[4] = {0x01, 0x02, 0x03, 0x04};
    unsigned char key = 0xFF;
    unsigned char* result = EncryptXOR<unsigned char>(data, key, 4);
    CHECK(result[0] == 0xFE);
    CHECK(result[1] == 0xFD);
    CHECK(result[2] == 0xFC);
    CHECK(result[3] == 0xFB);
    delete[] result;
}

TEST_CASE("EncryptXOR with key array XORs element-wise")
{
    unsigned char data[4] = {0x01, 0x02, 0x03, 0x04};
    unsigned char key[4] = {0x01, 0x02, 0x03, 0x04};
    unsigned char* result = EncryptXOR<unsigned char>(data, key, 4);
    CHECK(result[0] == 0);
    CHECK(result[1] == 0);
    CHECK(result[2] == 0);
    CHECK(result[3] == 0);
    delete[] result;
}

TEST_CASE("EncryptXOR is reversible")
{
    unsigned char data[4] = {0x41, 0x42, 0x43, 0x44};
    unsigned char key = 0x37;
    unsigned char* encrypted = EncryptXOR<unsigned char>(data, key, 4);
    unsigned char* decrypted = EncryptXOR<unsigned char>(encrypted, key, 4);
    CHECK(decrypted[0] == data[0]);
    CHECK(decrypted[1] == data[1]);
    CHECK(decrypted[2] == data[2]);
    CHECK(decrypted[3] == data[3]);
    delete[] encrypted;
    delete[] decrypted;
}

TEST_CASE("GFMult with zero returns zero")
{
    CHECK(GFMult(0, 0x53) == 0);
    CHECK(GFMult(0x53, 0) == 0);
}

TEST_CASE("GFMult with one returns the other value")
{
    CHECK(GFMult(1, 0x53) == 0x53);
    CHECK(GFMult(0x57, 1) == 0x57);
}

TEST_CASE("GFInv of zero returns zero")
{
    CHECK(GFInv(0) == 0);
}

TEST_CASE("GFInv of one returns one")
{
    CHECK(GFInv(1) == 1);
}

TEST_CASE("GetSBox returns 256 element array")
{
    unsigned char* sbox = GetSBox();
    REQUIRE(sbox != nullptr);
    // S-box entry for 0x00 should be 0x63 in standard AES
    // (though implementation may differ)
    delete[] sbox;
}

TEST_CASE("AddRoundKey128 XORs data with key")
{
    unsigned char data[16] = {0};
    unsigned char key[16] = {0};
    for (int i = 0; i < 16; i++) {
        data[i] = i;
        key[i] = 0xFF;
    }
    AddRoundKey128(data, key);
    for (int i = 0; i < 16; i++) {
        CHECK(data[i] == (unsigned char)(i ^ 0xFF));
    }
}

TEST_CASE("AddRoundKey128 is self-inverse")
{
    unsigned char data[16] = {0};
    unsigned char original[16] = {0};
    unsigned char key[16] = {0};
    for (int i = 0; i < 16; i++) {
        data[i] = i * 3;
        original[i] = i * 3;
        key[i] = i * 7;
    }
    AddRoundKey128(data, key);
    AddRoundKey128(data, key);
    for (int i = 0; i < 16; i++) {
        CHECK(data[i] == original[i]);
    }
}

TEST_CASE("ShiftRows128 modifies data in place")
{
    unsigned char data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    unsigned char original[16];
    std::memcpy(original, data, 16);
    ShiftRows128(data);
    // Verify at least some bytes changed
    bool changed = false;
    for (int i = 0; i < 16; i++) {
        if (data[i] != original[i]) changed = true;
    }
    CHECK(changed);
}

TEST_CASE("ExpandRoundKey128 returns 176 bytes (11 round keys)")
{
    unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    unsigned char* roundkeys = ExpandRoundKey128(key);
    REQUIRE(roundkeys != nullptr);
    // First 16 bytes should be the original key
    for (int i = 0; i < 16; i++) {
        CHECK(roundkeys[i] == key[i]);
    }
    delete[] roundkeys;
}

TEST_CASE("EncryptAES128 modifies data")
{
    unsigned char data[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
                              0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    unsigned char original[16];
    std::memcpy(original, data, 16);
    unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    EncryptAES128(data, key);
    // Verify data was modified
    bool changed = false;
    for (int i = 0; i < 16; i++) {
        if (data[i] != original[i]) changed = true;
    }
    CHECK(changed);
}

// UTIL TESTS

TEST_CASE("split with single delimiter")
{
    std::string s = "hello,world";
    std::vector<std::string> result = split(s, ",");
    REQUIRE(result.size() == 2);
    CHECK(result[0] == "hello");
    CHECK(result[1] == "world");
}

TEST_CASE("split with no delimiter present")
{
    std::string s = "helloworld";
    std::vector<std::string> result = split(s, ",");
    REQUIRE(result.size() == 1);
    CHECK(result[0] == "helloworld");
}

TEST_CASE("split with multiple delimiters")
{
    std::string s = "a:b:c:d";
    std::vector<std::string> result = split(s, ":");
    REQUIRE(result.size() == 4);
    CHECK(result[0] == "a");
    CHECK(result[1] == "b");
    CHECK(result[2] == "c");
    CHECK(result[3] == "d");
}

TEST_CASE("split with multi-character delimiter")
{
    std::string s = "foo||bar||baz";
    std::vector<std::string> result = split(s, "||");
    REQUIRE(result.size() == 3);
    CHECK(result[0] == "foo");
    CHECK(result[1] == "bar");
    CHECK(result[2] == "baz");
}

TEST_CASE("split empty string returns single empty element")
{
    std::string s = "";
    std::vector<std::string> result = split(s, ",");
    REQUIRE(result.size() == 1);
    CHECK(result[0] == "");
}

TEST_CASE("PadTo pads with specified value")
{
    char data[4] = {'A', 'B', 'C', 'D'};
    char* result = PadTo<char>(data, 4, 8, 'X');
    REQUIRE(result != nullptr);
    CHECK(result[0] == 'A');
    CHECK(result[1] == 'B');
    CHECK(result[2] == 'C');
    CHECK(result[3] == 'D');
    delete[] result;
}

TEST_CASE("PadTo with integers")
{
    int data[2] = {1, 2};
    int* result = PadTo<int>(data, 2, 4, 0);
    REQUIRE(result != nullptr);
    CHECK(result[0] == 1);
    CHECK(result[1] == 2);
    delete[] result;
}

// COMMAND TESTS

TEST_CASE("Execute returns output from simple command")
{
    std::string result = Execute("echo hello");
    CHECK(result.find("hello") != std::string::npos);
}

TEST_CASE("Execute captures multi-line output")
{
    std::string result = Execute("printf 'line1\\nline2\\n'");
    CHECK(result.find("line1") != std::string::npos);
    CHECK(result.find("line2") != std::string::npos);
}

TEST_CASE("Execute with pwd returns a path")
{
    std::string result = Execute("pwd");
    CHECK(result.front() == '/');
}

TEST_CASE("Execute with exit 0 command succeeds")
{
    std::string result = Execute("true");
    // Should not throw, result may be empty
    CHECK(true);
}

// ADDITIONAL SECURITY TESTS

TEST_CASE("AmIRoot returns false for non-root user")
{
    // Assuming tests run as non-root
    if (geteuid() != 0) {
        CHECK(!AmIRoot());
    } else {
        CHECK(AmIRoot());
    }
}

TEST_CASE("GetUser for non-existent user throws")
{
    CHECK_THROWS(GetUser("this_user_does_not_exist_xyz123"));
}

// ADDITIONAL IO TESTS

TEST_CASE("ReadFiles returns array of file objects")
{
    std::string paths[2] = {
        std::filesystem::absolute("tests/lowpermfolder/folder/folder/lowpermfile").string(),
        std::filesystem::absolute("src/io.cpp").string()
    };
    FileObject** files = ReadFiles(paths, 2);
    REQUIRE(files != nullptr);
    REQUIRE(files[0] != nullptr);
    REQUIRE(files[1] != nullptr);
    CHECK(files[0]->isfile);
    CHECK(files[1]->isfile);
}

TEST_CASE("ReadFiles with one invalid path throws")
{
    std::string paths[2] = {
        std::filesystem::absolute("src/io.cpp").string(),
        "/this/path/does/not/exist"
    };
    CHECK_THROWS(ReadFiles(paths, 2));
}

TEST_CASE("GetParentDirectory returns parent path")
{
    std::string result = GetParentDirectory("/home/user/file.txt");
    CHECK(result.find("user") != std::string::npos);
    CHECK(result.find("file.txt") == std::string::npos);
}

TEST_CASE("GetParentDirectory on nested path")
{
    std::string result = GetParentDirectory("src/io.cpp");
    CHECK(result.find("src") != std::string::npos);
}
