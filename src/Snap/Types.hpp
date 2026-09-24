#pragma once

#include <cstdint>
#include <assert.h>
#include <string_view>
#include <unordered_map>

#define SNAPC_MAGIC_BYTES 0x420

#define SNAPC_VERSION_MAJOR 0
#define SNAPC_VERSION_MINOR 1



namespace Snap {
    using f64 = double;
    using f32 = float;

    using u64 = std::uint64_t;
    using u32 = std::uint32_t;
    using u16 = std::uint16_t;
    using u8  = std::uint8_t;

    using i64 = std::int64_t;
    using i32 = std::int32_t;
    using i16 = std::int16_t;
    using i8  = std::int8_t;

    struct SnapCHeader {
        u8 versionMajor;
        u8 versionMinor;

        u16 magicBytes;

        u64 sectionHeaderOffset;
        u16 sectionHeaderCount;
        u64 sectionNameArrayOffset;
    } __attribute__((packed));

    struct SnapCSectionHeader {
        u64 nameOffset;
        u64 sectionOffset;
    } __attribute__((packed));

    enum class ValueType : std::uint8_t {
        VOID = 0,
        U64 = 1,
        U32 = 2,
        U16 = 3,
        U8 = 4,

        I64 = 5,
        I32 = 6,
        I16 = 7,
        I8 = 8,

        F64 = 9,
        F32 = 10
    };

    const std::unordered_map<ValueType, std::string_view> ValueTypeToStringLookup = {
        { ValueType::VOID, "void" },
        { ValueType::U64, "u64" },
        { ValueType::U32, "u32" },
        { ValueType::U16, "u16" },
        { ValueType::U8,  "u8" },

        { ValueType::I64, "i64" },
        { ValueType::I32, "i32" },
        { ValueType::I16, "i16" },
        { ValueType::I8,  "i8" },

        { ValueType::F32, "f32" },
        { ValueType::F64, "f64" },
    };

    const std::unordered_map<std::string_view, ValueType> StringToValueTypeLookup = {
        { "void", ValueType::VOID },
        {  "u64", ValueType::U64 },
        {  "u32", ValueType::U32 },
        {  "u16", ValueType::U16 },
        {   "u8", ValueType::U8 },

        {  "i64", ValueType::I64 },
        {  "i32", ValueType::I32 },
        {  "i16", ValueType::I16 },
        {   "i8", ValueType::I8 },

        {  "f32", ValueType::F32 },
        {  "f64", ValueType::F64 }
    };

    const std::unordered_map<ValueType, std::size_t> ValueTypeByteWidthLookup = {
        { ValueType::VOID, 0 },
        { ValueType::U64, 8 },
        { ValueType::U32, 4 },
        { ValueType::U16, 2 },
        { ValueType::U8, 1 },
        { ValueType::I64, 8 },
        { ValueType::I32, 4 },
        { ValueType::I16, 2 },
        { ValueType::I8, 1 },
        { ValueType::F32, 4 },
        { ValueType::F64, 8 },
    };

    union ValueData {
        u64 u64;
        u32 u32;
        u16 u16;
        u8  u8;

        i64 i64;
        i32 i32;
        i16 i16;
        i8  i8;

        f32 f32;
        f64 f64;
    };

    struct Value {
        ValueType type;
        ValueData data;

        static inline Value U64(u64 val) {
            Value sv;
            sv.type = ValueType::U64;
            sv.data.u64 = (u64)val;
            return sv;
        }
        static inline Value U32(u32 val) {
            Value sv;
            sv.type = ValueType::U32;
            sv.data.u32 = (u32)val;
            return sv;
        }
        static inline Value U16(u16 val) {
            Value sv;
            sv.type = ValueType::U16;
            sv.data.u16 = (u16)val;
            return sv;
        }
        static inline Value U8(u8 val) {
            Value sv;
            sv.type = ValueType::U8;
            sv.data.u8 = (u8)val;
            return sv;
        }
        static inline Value I64(i64 val) {
            Value sv;
            sv.type = ValueType::I64;
            sv.data.i64 = (i64)val;
            return sv;
        }
        static inline Value I32(i32 val) {
            Value sv;
            sv.type = ValueType::I32;
            sv.data.i32 = (i32)val;
            return sv;
        }
        static inline Value I16(i16 val) {
            Value sv;
            sv.type = ValueType::I16;
            sv.data.i16 = (i16)val;
            return sv;
        }
        static inline Value I8(i8 val) {
            Value sv;
            sv.type = ValueType::I8;
            sv.data.i8 = (i8)val;
            return sv;
        }
        static inline Value F32(f32 val) {
            Value sv;
            sv.type = ValueType::F32;
            sv.data.f32 = (f32)val;
            return sv;
        }
        static inline Value F64(f64 val) {
            Value sv;
            sv.type = ValueType::F64;
            sv.data.f64 = (f64)val;
            return sv;
        }

        inline u64 asU64() {
            assert(type == ValueType::U64);
            return data.u64;
        }
        inline u32 asU32() {
            assert(type == ValueType::U32);
            return data.u32;
        }
        inline u16 asU16() {
            assert(type == ValueType::U16);
            return data.u16;
        }
        inline u8 asU8() {
            assert(type == ValueType::U8);
            return data.u8;
        }
        inline i64 asI64() {
            assert(type == ValueType::I64);
            return data.i64;
        }
        inline i32 asI32() {
            assert(type == ValueType::I32);
            return data.i32;
        }
        inline i16 asI16() {
            assert(type == ValueType::I16);
            return data.i16;
        }
        inline i8 asI8() {
            assert(type == ValueType::I8);
            return data.i8;
        }
        inline f32 asF32() {
            assert(type == ValueType::F32);
            return data.f32;
        }
        inline f64 asF64() {
            assert(type == ValueType::F64);
            return data.f64;
        }
    };
}