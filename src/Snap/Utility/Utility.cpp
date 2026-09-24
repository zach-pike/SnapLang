#include "Utility.hpp"

#include <fstream>
#include <sstream>

void Snap::Utility::WriteBinaryFile(std::string path, const std::vector<u8>& data) {
    std::ofstream ofile(path, std::ios::binary);
    ofile.write((char*)data.data(), data.size());
}

std::optional<std::vector<Snap::u8>> Snap::Utility::ReadBinaryFile(std::string path) {
    std::ifstream ifile(path, std::ios::binary | std::ios::ate);
    if (!ifile.is_open()) return std::nullopt;

    std::size_t size = ifile.tellg();
    ifile.seekg(0, std::ios::beg);

    std::vector<u8> data(size);
    if (!ifile.read((char*)data.data(), size)) return std::nullopt;

    return data;
}

void Snap::Utility::WriteTextFile(std::string path, const std::string& data) {
    std::ofstream ofile(path);
    ofile.write((char*)data.data(), data.size());
}

std::optional<std::string> Snap::Utility::ReadTextFile(std::string path) {
    std::ifstream ifile(path);
    if (!ifile.is_open()) return std::nullopt;

    std::stringstream ss;
    ss << ifile.rdbuf();

    return ss.str();
}