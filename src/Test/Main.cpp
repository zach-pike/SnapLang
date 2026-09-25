#include <bits/stdc++.h>

#include "Compiler/Tokenizer.hpp"
#include "Snap/VM/Instruction.hpp"
#include "Snap/Types.hpp"
#include "Snap/VM/VM.hpp"
#include "Snap/Assembler/Assembler.hpp"
#include "Utility/Utility.hpp"
#include "Snap/Compiler/IRAssembler.hpp"

void AssembleAndSave() {
    auto binary = Snap::Assemble("fact.txt");
    Snap::Utility::WriteBinaryFile("fact.snapc", binary);
}

void LoadAndRun() {
    Snap::VM vm;
    vm.loadSnapCFile("fact.snapc");

    auto ret = vm.call("Factorial", { Snap::Value::U64(10) });
    std::cout << ret.value().asU64() << '\n';
}

int main(int argc, char** argv) {
    // AssembleAndSave();
    // LoadAndRun();

    std::string text = Snap::Utility::ReadTextFile("test.snapir").value();
    std::vector<Snap::u8> file = Snap::AssembleIR(text);

    Snap::VM vm;
    vm.loadSnapCBinary(file);
    auto ret = vm.call("Main", { });
    std::cout << ret.value().asU32() << '\n';



    // auto tokens = Snap::Tokenize(text);

    return 0;
}