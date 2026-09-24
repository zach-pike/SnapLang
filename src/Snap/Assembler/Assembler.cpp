#include "Assembler.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "assert.h"

#include "VM/Instruction.hpp"
#include "Compiler/Tokenizer.hpp"

void InsertArgBytes(std::vector<Snap::u8>& bytes, Snap::AssemblerType t, std::string raw) {
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

bool PeekToken(const std::vector<Snap::Token>& tokens, std::size_t i, Snap::TokenType ty) {
    return tokens.at(i).type == ty;
}

Snap::Token ExpectToken(const std::vector<Snap::Token>& tokens, std::size_t& i, Snap::TokenType ty) {
    Snap::Token tok = tokens.at(i);
    if (tok.type != ty) {
        std::cout << "Expected: " << (int)ty << " Got: " << (int)tok.type << '\n';
        assert(false);
    }
    i++;
    return tok;
}

std::vector<Snap::u8> Snap::Assemble(std::string filepath) {
    std::ifstream f(filepath);
    std::stringstream ss;
    ss << f.rdbuf();

    // offsets local to just the sections (ignoring everything else esentially)
    std::vector<Token> tokens = Tokenize(ss.str());
    std::vector<u8> sectionData;
    std::vector<std::pair<std::string, u64>> sectionLocalOffsets;
    std::vector<std::pair<std::string, u64>> unresolvedSectionAddressValueLocalOffsets;

    auto PushOpcodeBytes = [&](Instruction instr) {
        u8* ptr = reinterpret_cast<u8*>(&instr);
        sectionData.insert(sectionData.end(), ptr, ptr + sizeof(instr));
    };

    std::size_t i=0;
    while (i < tokens.size()) {
        // Section definition
        if (
            (i+1) < tokens.size() &&
            PeekToken(tokens, i, TokenType::LITERAL) && 
            PeekToken(tokens, i+1, TokenType::COLON) && 
            ((i+2) == tokens.size() || PeekToken(tokens, i+2, TokenType::SEMICOLON))
        ) {
            sectionLocalOffsets.push_back(
                std::make_pair(
                    tokens.at(i).data.value(),
                    (u64)sectionData.size()
                )
            );

            i += 3;
            continue;
        } else if (PeekToken(tokens, i, TokenType::LITERAL)) {
            std::string firstLiteral = ExpectToken(tokens, i, TokenType::LITERAL).data.value();
            Instruction in = StringToInstruction.at(firstLiteral);
            PushOpcodeBytes(in);

            // Get args if the instruction needs it
            if (InstructionArgTypes.count(in)) {
                auto args = InstructionArgTypes.at(in);

                for (int j=0; j<args.size(); j++) {
                    std::string rawArg = ExpectToken(tokens, i, TokenType::LITERAL).data.value();

                    if (args.at(j) == AssemblerType::SECTION_OFFSET) {
                        // Note position of current read head and fill in zero and
                        // Code will come back and write in the addresses
                        unresolvedSectionAddressValueLocalOffsets.push_back(std::make_pair(rawArg, sectionData.size()));
                        InsertArgBytes(sectionData, AssemblerType::U64, "0");
                    } else {
                        InsertArgBytes(sectionData, args[j], rawArg);
                    }
                }
            }
            
            if (i < tokens.size() && PeekToken(tokens, i, TokenType::SEMICOLON)) i += 1;
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