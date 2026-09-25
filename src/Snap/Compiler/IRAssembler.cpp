#include "IRAssembler.hpp"

#include <vector>
#include <functional>
#include <tuple>
#include <unordered_map>

#include "Tokenizer.hpp"
#include "VM/Instruction.hpp"

struct ScopeVariable {
    std::string name;
    Snap::ValueType type;
};

static const std::unordered_map<std::string_view, Snap::Instruction> BinaryMathOps = {
    { "add", Snap::Instruction::ADD },
    { "sub", Snap::Instruction::SUBTRACT },
    { "mult", Snap::Instruction::MULTIPLY },
    { "div", Snap::Instruction::DIVIDE },

    { "lt", Snap::Instruction::CMP_LT },
    { "gt", Snap::Instruction::CMP_GT },
    { "lte", Snap::Instruction::CMP_LTE },
    { "gte", Snap::Instruction::CMP_GTE },
    { "eq", Snap::Instruction::CMP_EQ },
    { "neq", Snap::Instruction::CMP_NEQ },
};

static const std::unordered_map<std::string_view, Snap::Instruction> TernaryOps = {
    { "call", Snap::Instruction::CALL },
};

std::vector<Snap::u8> Snap::AssembleIR(std::string ir) {
    auto tokens = Tokenize(ir);
    PrintTokens(tokens);
    std::size_t tokenIndex = 0;

    std::vector<u8> sectionData;
    std::vector<std::pair<std::string, u64>> sectionLocalOffsets;
    std::vector<std::pair<std::string, u64>> unresolvedSectionAddressValueLocalOffsets;

    auto PeekToken = [&](std::size_t lookAhead = 0) {
        assert((tokenIndex + lookAhead) < tokens.size());
        return tokens[tokenIndex + lookAhead];
    };
    auto CheckToken = [&](TokenType type, std::size_t lookAhead = 0) {
        if((tokenIndex + lookAhead) >= tokens.size()) return false;
        return tokens[tokenIndex + lookAhead].type == type;
    };
    auto ExpectToken = [&](TokenType type) {
        // Try to consume token of desired type or error
        assert(tokenIndex < tokens.size());

        // Get token
        auto token = tokens[tokenIndex];
        if (token.type != type) {
            assert(false);
        }

        tokenIndex += 1;

        return token;
    };
    auto SkipToken = [&]() {
        assert(tokenIndex < tokens.size());
        tokenIndex += 1;
    };
    auto ParseRawAsType = [&](std::string s, ValueType t) {
        switch (t) {
            case ValueType::U64: return Value::U64(std::stoull(s));
            case ValueType::U32: return Value::U32(std::stoul(s));
            case ValueType::U16: return Value::U16(std::stoul(s));
            case ValueType::U8:  return Value::U8 (std::stoul(s));
            case ValueType::I64: return Value::I64(std::stoll(s));
            case ValueType::I32: return Value::I32(std::stol(s));
            case ValueType::I16: return Value::I16(std::stol(s));
            case ValueType::I8:  return Value::I8 (std::stol(s));
            case ValueType::F32: return Value::F32(std::stof(s));
            case ValueType::F64: return Value::F64(std::stod(s));
            default: assert(false);
        }
    };

    auto InsertOpcode = [&](Instruction instr) {
        u8* ptr = reinterpret_cast<u8*>(&instr);
        sectionData.insert(sectionData.end(), ptr, ptr + sizeof(instr));
    };
    auto InsertArg = [&](Value v) {
        std::size_t size = ValueTypeByteWidthLookup.at(v.type);
        const u8* data = reinterpret_cast<const u8*>(&v.data);
        sectionData.insert(sectionData.end(), data, data + size);
    };

    std::function<void(std::vector<ScopeVariable>&)> ParseValue = [&](std::vector<ScopeVariable>& vars) {
        // Load variable
        if (CheckToken(TokenType::PERCENT)) {
            ExpectToken(TokenType::PERCENT);
            Token variableToken = ExpectToken(TokenType::LITERAL);
            std::string varName = variableToken.data.value();

            auto varEntryIt = std::find_if(vars.begin(), vars.end(), [&varName](ScopeVariable v) {
                return v.name == varName;
            });

            assert(varEntryIt != vars.end());
            u8 index = std::distance(vars.begin(), varEntryIt);

            // Load value onto stack
            InsertOpcode(Instruction::LOAD_LOCAL);
            InsertArg(Value::U8(index));
        }
        //function call
        else if (CheckToken(TokenType::CALL)) {
            ExpectToken(TokenType::CALL);
            Token funcNameToken = ExpectToken(TokenType::LITERAL);
            std::string funcName = funcNameToken.data.value();

            ExpectToken(TokenType::OPEN_PAREN);
            u8 argCount = 0;
            while(!CheckToken(TokenType::CLOSE_PAREN)) {
                ParseValue(vars);
                if (CheckToken(TokenType::COMMA)) ExpectToken(TokenType::COMMA);
                argCount ++;
            }
            ExpectToken(TokenType::CLOSE_PAREN);

            // Insert opcode
            InsertOpcode(Instruction::CALL);
            unresolvedSectionAddressValueLocalOffsets.push_back(std::make_pair(funcName, sectionData.size()));
            InsertArg(Value::U64(0));
            InsertArg(Value::U8(argCount));
        } else {
            Token initialToken = ExpectToken(TokenType::LITERAL);
            std::string initialTokString = initialToken.data.value();

            // check if the literal is a datatype
            if (StringToValueTypeLookup.count(initialTokString)) {
                ValueType type = StringToValueTypeLookup.at(initialTokString);

                ExpectToken(TokenType::OPEN_PAREN);
                Token v = ExpectToken(TokenType::LITERAL);
                std::string rawVal = v.data.value();
                ExpectToken(TokenType::CLOSE_PAREN);

                Value val = ParseRawAsType(rawVal, type);

                // Insert opcode and arg to load the immediate value onto stack
                switch(type) {
                    case ValueType::U64: InsertOpcode(Instruction::PUSH_U64); break;
                    case ValueType::U32: InsertOpcode(Instruction::PUSH_U32); break;
                    case ValueType::U16: InsertOpcode(Instruction::PUSH_U16); break;
                    case ValueType::U8:  InsertOpcode(Instruction::PUSH_U8); break;

                    case ValueType::I64: InsertOpcode(Instruction::PUSH_I64); break;
                    case ValueType::I32: InsertOpcode(Instruction::PUSH_I32); break;
                    case ValueType::I16: InsertOpcode(Instruction::PUSH_I16); break;
                    case ValueType::I8:  InsertOpcode(Instruction::PUSH_I8); break;

                    case ValueType::F32: InsertOpcode(Instruction::PUSH_F32); break;
                    case ValueType::F64: InsertOpcode(Instruction::PUSH_F64); break;

                    default: assert(false);
                }
                InsertArg(val);
            }
            // Check if its a binary math op
            else if (BinaryMathOps.count(initialTokString)) {
                ExpectToken(TokenType::OPEN_PAREN);
                // Left hand side
                ParseValue(vars);
                ExpectToken(TokenType::COMMA);
                ParseValue(vars);

                ExpectToken(TokenType::CLOSE_PAREN);

                Instruction in = BinaryMathOps.at(initialTokString);
                InsertOpcode(in);
            } else {
                assert(false);
            }
        }
    };

    auto ParseExpression = [&](std::vector<ScopeVariable>& vars) {
        // Label definition
        if (CheckToken(TokenType::LITERAL) && CheckToken(TokenType::COLON, 1)) {
            Token labelToken = ExpectToken(TokenType::LITERAL);
            std::string labelTokenStr = labelToken.data.value();
            ExpectToken(TokenType::COLON);

            sectionLocalOffsets.push_back(std::make_pair(labelTokenStr, sectionData.size()));
            return;
        }

        // Return
        if (CheckToken(TokenType::RETURN)) {
            ExpectToken(TokenType::RETURN);

            // No return value
            if (CheckToken(TokenType::SEMICOLON, 1)) {
                InsertOpcode(Instruction::RETURN);
            }
            // Return value
            else {
                ParseValue(vars);
                InsertOpcode(Instruction::RETURN_V);
            }
        }
        // Jump
        else if (CheckToken(TokenType::JUMP)) {
            ExpectToken(TokenType::JUMP);

            Token labelNameToken = ExpectToken(TokenType::LITERAL);
            std::string labelName = labelNameToken.data.value();

            InsertOpcode(Instruction::JUMP);
            unresolvedSectionAddressValueLocalOffsets.push_back(std::make_pair(labelName, sectionData.size()));
            InsertArg(Value::U64(0));
        }
        // Jump cond
        else if (CheckToken(TokenType::JUMP_COND)) {
            ExpectToken(TokenType::JUMP_COND);

            Token labelNameToken = ExpectToken(TokenType::LITERAL);
            std::string labelName = labelNameToken.data.value();
            ExpectToken(TokenType::COMMA);

            // Put condition onto stack
            ParseValue(vars);

            // Jump instr
            InsertOpcode(Instruction::JUMP_COND);
            unresolvedSectionAddressValueLocalOffsets.push_back(std::make_pair(labelName, sectionData.size()));
            InsertArg(Value::U64(0));
        }
        // new variable assignment
        else if (CheckToken(TokenType::LITERAL) && CheckToken(TokenType::PERCENT, 1)) {
            Token variableTypeToken = ExpectToken(TokenType::LITERAL);
            ValueType variableType = StringToValueTypeLookup.at(variableTypeToken.data.value());

            ExpectToken(TokenType::PERCENT);

            Token variableNameToken = ExpectToken(TokenType::LITERAL);
            std::string variableName = variableNameToken.data.value();

            u64 variableIndex = vars.size();
            vars.push_back(ScopeVariable{ .name=variableName, .type=variableType });

            ExpectToken(TokenType::EQUAL);

            // We need to have the value on the stack so we can use LOAD_LOCAL
            ParseValue(vars);

            // Insert the instruction for storing the value into local storage
            InsertOpcode(Instruction::STORE_LOCAL);
            InsertArg(Value::U8(variableIndex));
        }
        // Current var assignment
        else if (CheckToken(TokenType::PERCENT) && CheckToken(TokenType::LITERAL, 1) && CheckToken(TokenType::EQUAL, 2)) {
            ExpectToken(TokenType::PERCENT);

            Token variableNameToken = ExpectToken(TokenType::LITERAL);
            std::string variableName = variableNameToken.data.value();
            ExpectToken(TokenType::EQUAL);

            auto varEntryIt = std::find_if(vars.begin(), vars.end(), [&variableName](ScopeVariable v) {
                return v.name == variableName;
            });

            assert(varEntryIt != vars.end());
            u8 index = std::distance(vars.begin(), varEntryIt);

            // Parse bit after the equal sign
            ParseValue(vars);

            // Store into index
            InsertOpcode(Instruction::STORE_LOCAL);
            InsertArg(Value::U8(index));
        }

        ExpectToken(TokenType::SEMICOLON);
    };
    auto ParseFunction = [&]() {
        std::vector<ScopeVariable> vars;

        ExpectToken(TokenType::FUNCTION_DEF);

        auto retValToken = ExpectToken(TokenType::LITERAL);
        ValueType retValType = StringToValueTypeLookup.at(retValToken.data.value());
        
        ExpectToken(TokenType::AT);
        auto functionNameTok = ExpectToken(TokenType::LITERAL);

        // Insert current section offset
        std::string functionName = functionNameTok.data.value();
        sectionLocalOffsets.push_back(std::make_pair(functionName, sectionData.size()));

        ExpectToken(TokenType::OPEN_PAREN);
        while (!CheckToken(TokenType::CLOSE_PAREN)) {
            // Parse arg type
            auto argTypeToken = ExpectToken(TokenType::LITERAL);
            ValueType argType = StringToValueTypeLookup.at(argTypeToken.data.value());

            ExpectToken(TokenType::PERCENT);
            
            // Parse arg name
            auto argNameToken = ExpectToken(TokenType::LITERAL);
            std::string variableName = argNameToken.data.value();

            // Add variable to list
            vars.push_back(ScopeVariable{ .name=variableName, .type=argType });

            if(CheckToken(TokenType::COMMA)) ExpectToken(TokenType::COMMA);
        }
        ExpectToken(TokenType::CLOSE_PAREN);

        ExpectToken(TokenType::OPEN_CURLY);
        while (!CheckToken(TokenType::CLOSE_CURLY)) {
            // Parse all expressions
            ParseExpression(vars);
        }
        ExpectToken(TokenType::CLOSE_CURLY);
    };

    // Parse into unfinished bytecode
    while (tokenIndex < tokens.size()) {
        auto current = tokens.at(tokenIndex);

        if (CheckToken(TokenType::FUNCTION_DEF)) ParseFunction();
    }

    // Make local offset array for section names
    std::vector<u64> sectionNameLocalOffset;
    u64 localNameOffset=0;
    for (const auto& sect : sectionLocalOffsets) {
        sectionNameLocalOffset.push_back(localNameOffset);

        // Name + \0
        localNameOffset += sect.first.size() + 1;
    }

    // Fill in section names now that we have parsed the full file
    for (const auto& sect : unresolvedSectionAddressValueLocalOffsets) {
        auto it = std::find_if(sectionLocalOffsets.begin(), sectionLocalOffsets.end(), [&sect](std::pair<std::string, u64> a) {
            return a.first == sect.first;
        });

        assert(it != sectionLocalOffsets.end());
        *reinterpret_cast<u64*>(&sectionData[sect.second]) = sizeof(SnapCHeader) + it->second;
    }

    // Start assembling SnapC file
    std::vector<u8> assembledFile;

    // Header
    SnapCHeader header;
    header.versionMajor = SNAPC_VERSION_MAJOR;
    header.versionMinor = SNAPC_VERSION_MINOR;
    header.magicBytes = SNAPC_MAGIC_BYTES;

    // Section header comes directly after sections themselves
    header.sectionHeaderOffset = sizeof(SnapCHeader) + sectionData.size();
    header.sectionHeaderCount = sectionLocalOffsets.size();
    
    // Section names come directly after the section headers
    header.sectionNameArrayOffset = 
        header.sectionHeaderOffset + 
        header.sectionHeaderCount * sizeof(SnapCSectionHeader);

    // Insert header
    u8* headerptr = reinterpret_cast<u8*>(&header);
    assembledFile.insert(assembledFile.end(), headerptr, headerptr + sizeof(header));

    // Insert section data
    assembledFile.insert(assembledFile.end(), sectionData.begin(), sectionData.end());

    // Insert section headers
    for (int i=0; i<sectionLocalOffsets.size(); i++) {
        SnapCSectionHeader sectHeader;
        sectHeader.nameOffset = header.sectionNameArrayOffset + sectionNameLocalOffset.at(i);
        sectHeader.sectionOffset = sizeof(SnapCHeader) + sectionLocalOffsets.at(i).second;

        u8* sectHeaderPtr = reinterpret_cast<u8*>(&sectHeader);
        assembledFile.insert(assembledFile.end(), sectHeaderPtr, sectHeaderPtr + sizeof(SnapCSectionHeader));
    }

    // Insert section names
    for (int i=0; i<sectionLocalOffsets.size(); i++) {
        std::string name = sectionLocalOffsets.at(i).first;
        assembledFile.insert(assembledFile.end(), name.begin(), name.end());
        assembledFile.push_back(0); // null terminator
    }

    return assembledFile;
}