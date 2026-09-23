#include "Assembler.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "assert.h"

#include "VM/Instruction.hpp"
#include "Compiler/Tokenizer.hpp"

void insertBytesBasedOnType(std::vector<Snap::u8>& bytes, Snap::AssemblerType t, std::string raw) {
    using namespace Snap;
    switch(t) {
        case Snap::AssemblerType::U64: {
            u64 value = std::stoull(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::U32: {
            u32 value = std::stoul(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::U16: {
            u16 value = std::stoul(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::U8: {
            u8 value = std::stoul(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::I64: {
            i64 value = std::stoll(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::I32: {
            i32 value = std::stol(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::I16: {
            i16 value = std::stol(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::I8: {
            i8 value = std::stol(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::F32: {
            f32 value = std::stof(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
        case Snap::AssemblerType::F64: {
            f64 value = std::stod(raw);
            u8* valptr = reinterpret_cast<u8*>(&value);
            bytes.insert(bytes.end(), valptr, valptr + sizeof(value));
        } break;
    }
}

std::vector<Snap::u8> Snap::Assemble(std::string filepath) {
    std::ifstream f(filepath);
    std::stringstream ss;
    ss << f.rdbuf();

    // offsets local to just the sections (ignoring everything else esentially)
    std::vector<std::pair<std::string, u64>> sectionLocalOffsets;
    std::vector<Token> tokens = Tokenize(ss.str());
    std::vector<u8> sectionData;

    std::vector<std::pair<std::string, u64>> sectionFillIn;

    auto PeekToken = [&](std::size_t i, TokenType t) {
        return tokens.at(i).type == t;
    };

    auto PushOpcodeBytes = [&](Instruction instr) {
        u8* ptr = reinterpret_cast<u8*>(&instr);
        sectionData.insert(sectionData.end(), ptr, ptr + sizeof(instr));
    };

    int i=0;
    while (i < tokens.size()) {
        // Section definition
        if (
            (i+1) < tokens.size() &&
            PeekToken(i, TokenType::LITERAL) && 
            PeekToken(i+1, TokenType::COLON) && 
            ((i+2) == tokens.size() || PeekToken(i+2, TokenType::NEWLINE))
        ) {
            sectionLocalOffsets.push_back(
                std::make_pair(
                    tokens.at(i).data.value(),
                    (u64)sectionData.size()
                )
            );

            i += 3;
            continue;
        }

        // Instruction with no arg
        if (
            PeekToken(i, TokenType::LITERAL) &&
            ((i+1) == tokens.size() || PeekToken(i+1, TokenType::NEWLINE))
        ) {
            Instruction in = StringToInstruction.at(tokens.at(i).data.value());
            PushOpcodeBytes(in);

            i += 2;
            continue;
        }

        // Instruction with one arg
        if (
            (i+1) < tokens.size() &&
            PeekToken(i, TokenType::LITERAL) &&
            PeekToken(i+1, TokenType::LITERAL) &&
            ((i+2) == tokens.size() || PeekToken(i+2, TokenType::NEWLINE))
        ) {
            Instruction in = StringToInstruction.at(tokens.at(i).data.value());
            PushOpcodeBytes(in);

            if (in == Instruction::JUMP || in == Instruction::JUMP_COND) {
                std::string rawArg1 = tokens.at(i+1).data.value();

                // Push current size to this list so we can go back and put the proper address in once
                // all sections have been parsed
                sectionFillIn.push_back(std::make_pair(rawArg1, sectionData.size()));
                // Fill in zero for now
                insertBytesBasedOnType(sectionData, AssemblerType::U64, "0");
            } else {
                std::string rawArg = tokens.at(i+1).data.value();
                AssemblerType expectedType = InstructionArgTypes.at(in)[0];
                insertBytesBasedOnType(sectionData, expectedType, rawArg);
            }
            

            i += 3;
            continue;
        }

        //
        if (
            (i+2) < tokens.size() &&
            PeekToken(i, TokenType::LITERAL) &&
            PeekToken(i+1, TokenType::LITERAL) &&
            PeekToken(i+2, TokenType::LITERAL) &&
            ((i+3) == tokens.size() || PeekToken(i+3, TokenType::NEWLINE))
        ) {
            Instruction in = StringToInstruction.at(tokens.at(i).data.value());
            PushOpcodeBytes(in);

            if (in == Instruction::CALL) {
                std::string rawArg1 = tokens.at(i+1).data.value();
                std::string rawArg2 = tokens.at(i+2).data.value();

                // Push current size to this list so we can go back and put the proper address in once
                // all sections have been parsed
                sectionFillIn.push_back(std::make_pair(rawArg1, sectionData.size()));
                // Fill in zero for now
                insertBytesBasedOnType(sectionData, AssemblerType::U64, "0");

                auto argTypes = InstructionArgTypes.at(in);
                insertBytesBasedOnType(sectionData, argTypes[1], rawArg2);
            } else {
                std::string rawArg = tokens.at(i+1).data.value();
                AssemblerType expectedType = InstructionArgTypes.at(in)[0];

                insertBytesBasedOnType(sectionData, expectedType, rawArg);
            }

            i += 3;
            continue;
        }

        i += 1;
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
    for (const auto& sect : sectionFillIn) {
        auto it = std::find_if(sectionLocalOffsets.begin(), sectionLocalOffsets.end(), [&sect](std::pair<std::string, u64> a) {
            return a.first == sect.first;
        });

        if (it == sectionLocalOffsets.end()) assert(false);
        *reinterpret_cast<u64*>(&sectionData[sect.second]) = sizeof(SnapCHeader) + it->second;
    }

    // Header
    SnapCHeader header;
    header.versionMajor = SNAPC_VERSION_MAJOR;
    header.versionMinor = SNAPC_VERSION_MINOR;

    // Magic bytes
    header.magicBytes = SNAPC_MAGIC_BYTES;

    // Section header comes directly after sections themselves
    header.sectionHeaderOffset = sizeof(SnapCHeader) + sectionData.size();
    header.sectionHeaderCount = sectionLocalOffsets.size();
    
    // Section names come directly after the section headers
    header.sectionNameArrayOffset = 
        header.sectionHeaderOffset + 
        sectionLocalOffsets.size() * sizeof(SnapCSectionHeader);

    std::vector<u8> assembledFile;

    // Insert file header
    u8* headerptr = reinterpret_cast<u8*>(&header);
    assembledFile.insert(assembledFile.end(), headerptr, headerptr + sizeof(header));

    // Insert sections
    assembledFile.insert(assembledFile.end(), sectionData.begin(), sectionData.end());

    // Insert section headers
    for (int i=0; i<sectionLocalOffsets.size(); i++) {
        SnapCSectionHeader sectHeader;
        sectHeader.nameOffset = header.sectionNameArrayOffset + sectionNameLocalOffset.at(i);
        sectHeader.sectionOffset = sizeof(SnapCHeader) + sectionLocalOffsets.at(i).second;

        u8* sectHeaderPtr = reinterpret_cast<u8*>(&sectHeader);
        assembledFile.insert(assembledFile.end(), sectHeaderPtr, sectHeaderPtr + sizeof(SnapCSectionHeader));
    }

    // Insert section name
    for (int i=0; i<sectionLocalOffsets.size(); i++) {
        std::string name = sectionLocalOffsets.at(i).first;
        assembledFile.insert(assembledFile.end(), name.begin(), name.end());
        assembledFile.push_back(0); // null terminator
    }

    return assembledFile;
}