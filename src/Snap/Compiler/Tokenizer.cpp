#include "Tokenizer.hpp"

bool isToken(const std::string& s, std::size_t pos) {
    for (const auto& checkToken : Snap::StringToTokenLookup) {
        // If we are at end of file, dont look for tokens that are longer than the input length
        if ((pos + checkToken.first.length()) > s.length()) continue;
        if (s.compare(pos, checkToken.first.length(), checkToken.first) == 0) return true;
    }
    return false;
}

std::vector<Snap::Token> Snap::Tokenize(const std::string& input) {
    std::vector<Token> tokens;

    int i = 0;
    while (i < input.length()) {
        // Start string literal mode
        if (input.at(i) == '\"') {
            // Skips end quotes that have backslash before
            // Start one past the start quote
            int j = 1;
            while(
                (i+j) < input.length() && 
                !(
                    input.at(i+j) == '\"' &&
                    input.at(i+j-1) != '\\'
                )
            ) j++;

            tokens.push_back(Token{ .type=TokenType::STRING_LITERAL, .data=input.substr(i + 1, j - 1) });
            i = i + j + 1;
        }

        // Skip whitespaces
        if (isspace(input.at(i))) {
            // keep track of newline though
            // if (input.at(i) == '\n') tokens.push_back(Token{ .type=TokenType::NEWLINE });
            i++;
            continue;
        }

        // Start at current position, loop thru all tokens on file. if no token is found, keep going till token is found and move anything extra into accumulator
        bool foundToken = false;
        for (const auto& checkToken : StringToTokenLookup) {
            // If we are at end of file, dont look for tokens that are longer than the input length
            if ((i + checkToken.first.length()) > input.length()) continue;

            if (input.compare(i, checkToken.first.length(), checkToken.first) == 0) {
                // Found a token.
                tokens.push_back(Token{ .type=checkToken.second });

                // Move cursor up to the end of where we found token
                i = i + checkToken.first.length();
                foundToken = true;
                break;
            }
        }
        // allow for tokens that come after tokens "()" to not come out as a token and literal
        if (foundToken == true) continue;


        // If we got here, we need to pull a literal stop at tokens or whitespace
        int j = 0;
        while(i+j < input.length() && !isToken(input, i+j) && !isspace(input.at(i+j))) j++;
        tokens.push_back(Token{ .type=TokenType::LITERAL, .data=input.substr(i, j) });
        i = i + j;
    }

    return tokens;
}