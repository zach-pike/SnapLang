#include "CallStack.hpp"

#include <iostream>
#include <format>

Snap::CallStack::CallStack() {}
Snap::CallStack::~CallStack() {}

void Snap::CallStack::push(CallStackFrame v) {
    stack.push_back(v);
}

Snap::CallStackFrame Snap::CallStack::pop() {
    CallStackFrame v = stack.back();
    stack.pop_back();
    return v;
}

Snap::CallStackFrame& Snap::CallStack::back() {
    return stack.back();
}

std::size_t Snap::CallStack::size() {
    return stack.size();
}

void Snap::CallStack::reset() {
    stack.clear();
}

void Snap::CallStack::dump() const {
    std::cout << "Call Stack Dump\n";
    for (int i=stack.size()-1; i >= 0; i--) {
        auto v = stack.at(i);

        std::cout << std::format("-------- (Frame {:05}) --------\n", i);
        std::cout << std::format("IP: {} ", v.ip) << std::endl;
        v.opstack.dump();
        std::cout << std::format("------ (Frame {:05} END) ------\n\n", i);

    }
}