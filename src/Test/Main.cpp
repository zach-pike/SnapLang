#include <bits/stdc++.h>

#include "Compiler/Tokenizer.hpp"
#include "Snap/VM/Instruction.hpp"
#include "Snap/Types.hpp"
#include "Snap/VM/VM.hpp"
#include "Snap/Assembler/Assembler.hpp"


int main(int argc, char** argv) {
    auto binary = Snap::Assemble("fib.txt");

    Snap::VM vm;
    vm.loadSnapCBinary(binary);

    auto ret = vm.call("Fibonacci", { Snap::Value::U64(50) });
    std::cout << ret.value().asU64() << '\n';

    return 0;
}