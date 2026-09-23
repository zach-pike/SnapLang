#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <string_view>

#include "Types.hpp"
#include "VM/Instruction.hpp"

namespace Snap {
    enum class AssemblerType : std::uint8_t {
        U64 = 0,
        U32 = 1,
        U16 = 2,
        U8  = 3,

        I64 = 4,
        I32 = 5,
        I16 = 6,
        I8  = 7,

        F32 = 8,
        F64 = 9
    };

    const std::unordered_map<std::string_view, Instruction> StringToInstruction = {
        { "PUSH_U64", Instruction::PUSH_U64 },
        { "PUSH_U32", Instruction::PUSH_U32 },
        { "PUSH_U16", Instruction::PUSH_U16 },
        { "PUSH_U8",  Instruction::PUSH_U8  },

        { "PUSH_I64", Instruction::PUSH_I64 },
        { "PUSH_I32", Instruction::PUSH_I32 },
        { "PUSH_I16", Instruction::PUSH_I16 },
        { "PUSH_I8",  Instruction::PUSH_I8  },

        { "PUSH_F32",  Instruction::PUSH_F32 },
        { "PUSH_F64",  Instruction::PUSH_F64 },

        { "ADD",  Instruction::ADD },
        { "SUB",  Instruction::SUBTRACT },
        { "MULT", Instruction::MULTIPLY },
        { "DIV",  Instruction::DIVIDE },

        { "HALT",  Instruction::HALT },

        { "RETURN", Instruction::RETURN },
        { "RETURN_V", Instruction::RETURN_V },
        { "CALL", Instruction::CALL },

        { "DUMP", Instruction::DUMP },

        { "LOAD_LOCAL", Instruction::LOAD_LOCAL },
        { "STORE_LOCAL", Instruction::STORE_LOCAL },
        { "STORE_LOCAL_NOPOP", Instruction::STORE_LOCAL_NOPOP },

        { "CMP_EQ", Instruction::CMP_EQ },
        { "CMP_NEQ", Instruction::CMP_NEQ },
        { "CMP_LT", Instruction::CMP_LT },
        { "CMP_GT", Instruction::CMP_GT },
        { "CMP_LTE", Instruction::CMP_LTE },
        { "CMP_GTE", Instruction::CMP_GTE },

        { "JUMP", Instruction::JUMP },
        { "JUMP_COND", Instruction::JUMP_COND }
    };

    const std::unordered_map<Instruction, std::vector<AssemblerType>> InstructionArgTypes = {
        { Instruction::PUSH_U64, { AssemblerType::U64 } },
        { Instruction::PUSH_U32, { AssemblerType::U32 } },
        { Instruction::PUSH_U16, { AssemblerType::U16 } },
        { Instruction::PUSH_U8,  { AssemblerType::U8 } },

        { Instruction::PUSH_I64, { AssemblerType::I64 } },
        { Instruction::PUSH_I32, { AssemblerType::I32 } },
        { Instruction::PUSH_I16, { AssemblerType::I16 } },
        { Instruction::PUSH_I8,  { AssemblerType::I8 } },

        { Instruction::PUSH_F32, { AssemblerType::F32 } },
        { Instruction::PUSH_F64, { AssemblerType::F64 } },

        { Instruction::CALL, { AssemblerType::U64, AssemblerType::U8 } },
        { Instruction::JUMP, { AssemblerType::U64 } },
        { Instruction::JUMP_COND, { AssemblerType::U64 } },
        
        { Instruction::LOAD_LOCAL, { AssemblerType::U8 } },
        { Instruction::STORE_LOCAL, { AssemblerType::U8 } },
        { Instruction::STORE_LOCAL_NOPOP, { AssemblerType::U8 } },
    };

    std::vector<u8> Assemble(std::string);
}