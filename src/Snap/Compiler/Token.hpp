#pragma once

#include <optional>
#include <string>
#include <cstdint>

namespace Snap {
    enum class TokenType : std::uint16_t {
        LITERAL = 0, // 42, variableName
        STRING_LITERAL = 1, // "hello"
        NEWLINE = 2,

        COLON = 3
    };

    struct Token {
        TokenType type;
        std::optional<std::string> data;
    };
}