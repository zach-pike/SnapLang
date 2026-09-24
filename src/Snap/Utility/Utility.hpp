#pragma once

#include <string>
#include <vector>
#include <optional>

#include "Types.hpp"

namespace Snap {
    namespace Utility {
        void WriteBinaryFile(std::string path, const std::vector<u8>& data);
        std::optional<std::vector<u8>> ReadBinaryFile(std::string path);
    }
}