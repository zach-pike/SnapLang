#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <string_view>
#include <cstdint>
#include <format>
#include <iostream>

namespace Snap {
    enum class TokenType : std::uint16_t {
        LITERAL = 0, // 42, variableName
        STRING_LITERAL = 1, // "hello"

        COLON = 3,
        SEMICOLON = 4,
        PERCENT = 5,
        AT = 6,
        EQUAL = 7,
        COMMA = 8,

        OPEN_PAREN = 9,
        CLOSE_PAREN = 10,
        OPEN_BRACKET = 11,
        CLOSE_BRACKET = 12,
        OPEN_CURLY = 13,
        CLOSE_CURLY = 14,

        FUNCTION_DEF = 15,
        RETURN = 16,
        CALL = 17,
    };

    const std::unordered_map<std::string_view, Snap::TokenType> StringToTokenLookup = {
        { ":", TokenType::COLON },
        { ";", TokenType::SEMICOLON },
        { "%", TokenType::PERCENT },
        { "@", TokenType::AT },
        { "=", TokenType::EQUAL },
        { ",", TokenType::COMMA },

        { "(", TokenType::OPEN_PAREN },
        { ")", TokenType::CLOSE_PAREN },
        { "[", TokenType::OPEN_BRACKET },
        { "]", TokenType::CLOSE_BRACKET },
        { "{", TokenType::OPEN_CURLY },
        { "}", TokenType::CLOSE_CURLY },

        { "fn", TokenType::FUNCTION_DEF },
        { "return", TokenType::RETURN },
        { "call", TokenType::CALL }
    };

    const std::unordered_map<Snap::TokenType, std::string_view> TokenTypeToString = {
        { TokenType::LITERAL, "LITERAL" },
        { TokenType::STRING_LITERAL, "STRING_LITERAL" },

        { TokenType::COLON, "COLON" },
        { TokenType::SEMICOLON, "SEMICOLON" },
        { TokenType::PERCENT, "PERCENT" },
        { TokenType::AT, "AT" },
        { TokenType::EQUAL, "EQUAL" },
        { TokenType::COMMA, "COMMA" },

        { TokenType::OPEN_PAREN, "OPEN_PAREN" },
        { TokenType::CLOSE_PAREN, "CLOSE_PAREN" },

        { TokenType::OPEN_BRACKET, "OPEN_BRACKET" },
        { TokenType::CLOSE_BRACKET, "CLOSE_BRACKET" },

        { TokenType::OPEN_CURLY, "OPEN_CURLY" },
        { TokenType::CLOSE_CURLY, "CLOSE_CURLY" },

        { TokenType::FUNCTION_DEF, "FUNCTION_DEF" },
        { TokenType::RETURN, "RETURN" },
        { TokenType::CALL, "CALL" },
    };

    struct Token {
        TokenType type;
        std::optional<std::string> data;
    };
    
    inline void PrintTokens(const std::vector<Token>& tokens) {
        for (int i=0; i<tokens.size(); i++) {
            auto token = tokens.at(i);

            if (token.type == TokenType::LITERAL || token.type == TokenType::STRING_LITERAL) {
                std::cout << std::format("{} | Token: {} | Data : ({})\n", i, TokenTypeToString.at(token.type), token.data.value());
            } else {
                std::cout << std::format("{} | Token: {}\n", i, TokenTypeToString.at(token.type));
            }
        }
    }
}