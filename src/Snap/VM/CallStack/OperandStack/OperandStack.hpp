#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "Types.hpp"

namespace Snap {
    class OperandStack {
    private:
        std::vector<Value> stack;
    public:
        static std::string ValueTypeToString(ValueType);
        static std::string ValueToString(Value v);

        OperandStack();
        ~OperandStack();

        void push(Value v);
        Value pop(std::size_t n = 1);
        Value& back();
        std::size_t size() const;

        std::vector<Value> getLastNElems(std::size_t n);

        void dump() const;
    };
}