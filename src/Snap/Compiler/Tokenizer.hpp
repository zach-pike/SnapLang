#pragma once

#include <vector>
#include <string>

#include "Token.hpp"


namespace Snap {
    std::vector<Token> Tokenize(const std::string& input);
}
