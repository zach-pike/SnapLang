#include "OperandStack.hpp"

#include <iostream>
#include <format>

#include <assert.h>

std::string Snap::OperandStack::ValueTypeToString(ValueType v) {
    switch(v) {
        case ValueType::U64: return "U64";
        case ValueType::U32: return "U32";
        case ValueType::U16: return "U16";
        case ValueType::U8:  return "U8";

        case ValueType::I64: return "I64";
        case ValueType::I32: return "I32";
        case ValueType::I16: return "I16";
        case ValueType::I8:  return "I8";

        case ValueType::F32: return "F32";
        case ValueType::F64: return "F64";
        default: return "UNKNOWN";
    }
}

std::string Snap::OperandStack::ValueToString(Value v) {
    std::string s = ValueTypeToString(v.type) + '(';

    switch (v.type) {
        case ValueType::U64: s += std::to_string(v.data.u64); break;
        case ValueType::U32: s += std::to_string(v.data.u32); break;
        case ValueType::U16: s += std::to_string(v.data.u16); break;
        case ValueType::U8:  s += std::to_string(v.data.u8);  break;

        case ValueType::I64: s += std::to_string(v.data.i64); break;
        case ValueType::I32: s += std::to_string(v.data.i32); break;
        case ValueType::I16: s += std::to_string(v.data.i16); break;
        case ValueType::I8:  s += std::to_string(v.data.i8);  break;

        case ValueType::F32: s += std::to_string(v.data.f32); break;
        case ValueType::F64: s += std::to_string(v.data.f64); break;
    }

    s += ')';
    return s;
}

Snap::OperandStack::OperandStack() {}
Snap::OperandStack::~OperandStack() {}

void Snap::OperandStack::push(Value v) {
    stack.push_back(v);
}

Snap::Value Snap::OperandStack::pop(std::size_t n) {
    if (stack.size() == 0) {
        assert(false);
    }

    Value a = stack.back();
    for (int i=0; i<n; i++) {
        stack.pop_back();
    }
    return a;
}

Snap::Value& Snap::OperandStack::back() {
    if (stack.size() == 0) assert(false);

    return stack.back();
}

std::size_t Snap::OperandStack::size() const {
    return stack.size();
}

void Snap::OperandStack::dump() const {
    std::cout << "---- (Operand Stack Dump) -----\n";
    for (int i=stack.size()-1; i >= 0; i--) {
        auto v = stack.at(i);
        std::cout << std::format("{:05} | {}", i, ValueToString(v)) << std::endl;
    }
}

std::vector<Snap::Value> Snap::OperandStack::getLastNElems(std::size_t n) {
    return std::vector<Snap::Value>(
        stack.end() - n,
        stack.end()
    );
}