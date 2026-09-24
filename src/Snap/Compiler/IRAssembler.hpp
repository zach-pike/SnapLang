#pragma once

#include <vector>
#include <string>

#include "Types.hpp"

namespace Snap {
    std::vector<Snap::u8> AssembleIR(std::string ir);
}