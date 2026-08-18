#pragma once

namespace bl
{


class File
{
public:
    File(const std::string& path);
    ~File();

    File LoadFile();

    static bool Exists(const std::string& path);
    static void SetFilesystemType();


};

}