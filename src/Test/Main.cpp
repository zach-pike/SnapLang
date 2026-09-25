#include <bits/stdc++.h>

#include "Compiler/Tokenizer.hpp"
#include "Snap/VM/Instruction.hpp"
#include "Snap/Types.hpp"
#include "Snap/VM/VM.hpp"
#include "Snap/Assembler/Assembler.hpp"
#include "Utility/Utility.hpp"
#include "Snap/Compiler/IRAssembler.hpp"

int main(int argc, char** argv) {
    // AssembleAndSave();
    // LoadAndRun();

    std::string text = Snap::Utility::ReadTextFile("SnapCExamples/fibonacci.SnapIR").value();
    std::vector<Snap::u8> file = Snap::AssembleIR(text);

    Snap::VM vm;
    vm.loadSnapCBinary(file);
    auto ret = vm.call("Fibonacci", { Snap::Value::U32(10) });
    std::cout << ret.value().asU32() << '\n';
    return 0;
}