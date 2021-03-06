#pragma once

#include <fstream>
#include <string>

#include "cg_exception.hpp"

class FileManager {
public:
    const static FileManager shared;
    bool FileExistsAt(const std::string &) const;
    void CreateFileAt(const std::string &) const;
    uint64_t FileSizeAt(const std::string &) const;
    void RemoveFileAt(const std::string &) const;
    const std::string FileContentAt(const std::string &) const;
    
};

const FileManager FileManager::shared = FileManager();

bool FileManager::FileExistsAt(const std::string &path) const {
    std::ifstream is(path);
    return is.is_open();
}

void FileManager::CreateFileAt(const std::string &path) const {
    std::ofstream os(path);
}

uint64_t FileManager::FileSizeAt(const std::string &path) const {
    using namespace std;
    fstream fs;
    fs.open(path, ios::in | ios::binary);
    fs.seekg(0, ios::end);
    return (uint64_t) fs.tellg();
}

void FileManager::RemoveFileAt(const std::string &path) const {
    remove(path.c_str());
}

const std::string FileManager::FileContentAt(const std::string &path) const {
    using namespace std;
    if (!FileExistsAt(path)) throw FileNotExistsError(path);
    ifstream in(path);
    istreambuf_iterator<char> begin(in);
    istreambuf_iterator<char> end;
    string res(begin, end);
    return res;
}

