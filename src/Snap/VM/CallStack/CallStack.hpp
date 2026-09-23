#pragma once

#include <array>
#include <optional>

#include "OperandStack/OperandStack.hpp"

#include "Types.hpp"

namespace Snap {
    struct CallStackFrame {
        u64 ip;
        std::array<std::optional<Value>, 128> locals;
        OperandStack opstack;
    };

    class CallStack {
    private:
        std::vector<CallStackFrame> stack;
    public:
        CallStack();
        ~CallStack();

        void push(CallStackFrame);
        CallStackFrame pop();
        CallStackFrame& back();
        std::size_t size();

        void reset();

        void dump() const;
    };
}