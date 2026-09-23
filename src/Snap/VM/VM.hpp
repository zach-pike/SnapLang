#pragma once

#include "CallStack/CallStack.hpp"
#include "Types.hpp"

#include <cstdint>
#include <vector>
#include <optional>
#include <unordered_map>

namespace Snap {
    class VM {
    private:
        CallStack callStack;
        
        // Bytecode information
        SnapCHeader header;
        std::unordered_map<std::string, SnapCSectionHeader> sections;
        std::vector<u8> bytecode;

        bool systemHalted = false;

        std::optional<Value> stepExecution();
        bool isHalted() const;
        void decodeSnapCHeader();
    public:
        VM();
        ~VM();

        void loadSnapCBinary(const std::vector<std::uint8_t>& code);
        std::optional<Snap::Value> call(std::string functionName, std::vector<Snap::Value> args = {});
        void reset();
        
        void dump() const;
    };
}