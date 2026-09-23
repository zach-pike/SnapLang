#pragma once

#include <cstdint>

namespace Snap {
    enum class Instruction : std::uint16_t {
        PUSH_U64 = 0,
        PUSH_U32 = 1,
        PUSH_U16 = 2,
        PUSH_U8  = 3,

        PUSH_I64 = 4,
        PUSH_I32 = 5,
        PUSH_I16 = 6,
        PUSH_I8  = 7,

        PUSH_F32 = 8,
        PUSH_F64 = 9,

        ADD      = 10,
        SUBTRACT = 11,
        MULTIPLY = 12,
        DIVIDE   = 13,

        HALT = 14,
        RETURN = 15,
        RETURN_V = 16,
        CALL = 17,
        DUMP = 18,

        LOAD_LOCAL = 19,
        STORE_LOCAL = 20,
        STORE_LOCAL_NOPOP = 21,

        CMP_EQ = 22,
        CMP_NEQ = 23,
        CMP_LT = 24,
        CMP_GT = 25,
        CMP_LTE = 26,
        CMP_GTE = 27,

        JUMP = 28,
        JUMP_COND = 29
    };
}