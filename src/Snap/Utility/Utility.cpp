#include "Utility.hpp"

#include <fstream>

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