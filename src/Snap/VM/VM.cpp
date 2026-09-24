#include "VM.hpp"

#include <cassert>
#include <iostream>

#include "Types.hpp"
#include "Instruction.hpp"
#include "CallStack/CallStack.hpp"
#include "Utility/Utility.hpp"

Snap::VM::VM() {}
Snap::VM::~VM() {}

void Snap::VM::decodeSnapCHeader() {
    // Read beginning of file as header
    header = *reinterpret_cast<const SnapCHeader*>(bytecode.data());

    // File checks
    assert(header.magicBytes == SNAPC_MAGIC_BYTES);
    assert(header.versionMajor == SNAPC_VERSION_MAJOR);
    assert(header.versionMinor == SNAPC_VERSION_MINOR);

    // Read the section header pointer
    const SnapCSectionHeader* sectionPtr = reinterpret_cast<const SnapCSectionHeader*>(&bytecode[header.sectionHeaderOffset]);

    // Load the sections by name
    for (int i=0; i<header.sectionHeaderCount; i++) {
        SnapCSectionHeader s = sectionPtr[i];
        const char* name = reinterpret_cast<const char*>(&bytecode[s.nameOffset]);
        sections[name] = s;
    }
}

void Snap::VM::reset() {
    callStack.reset();
    systemHalted = false;
}

void Snap::VM::loadSnapCBinary(const std::vector<std::uint8_t>& code) {
    bytecode = code;
    decodeSnapCHeader();
    reset();
}

void Snap::VM::loadSnapCFile(std::string filepath) {
    auto a = Utility::ReadBinaryFile(filepath);
    assert(a.has_value());
    loadSnapCBinary(a.value());
}

std::optional<Snap::Value> Snap::VM::call(std::string s, std::vector<Snap::Value> args) {
    // Create a inital call frame
    CallStackFrame root;
    root.ip = sections.at(s).sectionOffset;
    
    // Add args
    for (int i=0; i<args.size(); i++) {
        root.locals[i] = args[i];
    }
    
    callStack.push(root);

    std::optional<Value> returnValue = std::nullopt;

    while (callStack.size() > 0) {
        if (isHalted()) {
            std::cout << "[INFO] VM Halted early!\n";
            break;
        }
        // Step execution
        returnValue = stepExecution();
    }

    return returnValue;
}

template <typename Op>
Snap::Value binaryOp(Snap::Value lhs, Snap::Value rhs, Op op) {
    assert(lhs.type == rhs.type);

    switch (lhs.type) {
        case Snap::ValueType::U64:
            return Snap::Value::U64(op(lhs.data.u64, rhs.data.u64));

        case Snap::ValueType::U32:
            return Snap::Value::U32(op(lhs.data.u32, rhs.data.u32));

        case Snap::ValueType::U16:
            return Snap::Value::U16(op(lhs.data.u16, rhs.data.u16));

        case Snap::ValueType::U8:
            return Snap::Value::U8(op(lhs.data.u8, rhs.data.u8));

        case Snap::ValueType::I64:
            return Snap::Value::I64(op(lhs.data.i64, rhs.data.i64));

        case Snap::ValueType::I32:
            return Snap::Value::I32(op(lhs.data.i32, rhs.data.i32));

        case Snap::ValueType::I16:
            return Snap::Value::I16(op(lhs.data.i16, rhs.data.i16));

        case Snap::ValueType::I8:
            return Snap::Value::I8(op(lhs.data.i8, rhs.data.i8));

        case Snap::ValueType::F32:
            return Snap::Value::F32(op(lhs.data.f32, rhs.data.f32));

        case Snap::ValueType::F64:
            return Snap::Value::F64(op(lhs.data.f64, rhs.data.f64));

        default: {
            assert(false);
        }
    }
}

template <typename Op>
Snap::Value compareOp(Snap::Value lhs, Snap::Value rhs, Op op) {
    assert(lhs.type == rhs.type);
    return Snap::Value::U8(op(lhs.data.u8, rhs.data.u8));
}

std::optional<Snap::Value> Snap::VM::stepExecution() {
    u64& instructionPointer = callStack.back().ip;
    OperandStack& operandStack = callStack.back().opstack;
    auto& locals = callStack.back().locals;

    if (instructionPointer >= bytecode.size()) {
        systemHalted = true;
        return std::nullopt;
    }

    Instruction instr = *(Instruction*)&bytecode[instructionPointer];

    switch(instr) {
        case Instruction::PUSH_U64: {
            u64 value = *(u64*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::U64, .data = ValueData{ .u64=value }});
            instructionPointer += sizeof(Instruction) + sizeof(u64);
        } break;
        case Instruction::PUSH_U32: {
            u32 value = *(u32*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::U32, .data = ValueData{ .u32=value }});
            instructionPointer += sizeof(Instruction) + sizeof(u32);
        } break;
        case Instruction::PUSH_U16: {
            u16 value = *(u16*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::U16, .data = ValueData{ .u16=value }});
            instructionPointer += sizeof(Instruction) + sizeof(u16);
        } break;
        case Instruction::PUSH_U8: {
            u8 value = *(u8*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::U8, .data = ValueData{ .u8=value }});
            instructionPointer += sizeof(Instruction) + sizeof(u8);
        } break;
        case Instruction::PUSH_I64: {
            i64 value = *(i64*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::I64, .data = ValueData{ .i64=value }});
            instructionPointer += sizeof(Instruction) + sizeof(i64);
        } break;
        case Instruction::PUSH_I32: {
            i32 value = *(i32*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::I32, .data = ValueData{ .i32=value }});
            instructionPointer += sizeof(Instruction) + sizeof(i32);
        } break;
        case Instruction::PUSH_I16: {
            i16 value = *(i16*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::I16, .data = ValueData{ .i16=value }});
            instructionPointer += sizeof(Instruction) + sizeof(i16);
        } break;
        case Instruction::PUSH_I8: {
            i8 value = *(i8*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::I8, .data = ValueData{ .i8=value }});
            instructionPointer += sizeof(Instruction) + sizeof(i8);
        } break;
        case Instruction::PUSH_F32: {
            f32 value = *(f32*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::F32, .data = ValueData{ .f32=value }});
            instructionPointer += sizeof(Instruction) + sizeof(f32);
        } break;
        case Instruction::PUSH_F64: {
            f64 value = *(f64*)&bytecode[instructionPointer + sizeof(Instruction)];
            operandStack.push(Value{ .type=ValueType::F64, .data = ValueData{ .f64=value }});
            instructionPointer += sizeof(Instruction) + sizeof(f64);
        } break;
        case Instruction::ADD: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(binaryOp(lhs, rhs, [](auto a, auto b) {
                return a + b;
            }));

            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::SUBTRACT: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(binaryOp(lhs, rhs, [](auto a, auto b) {
                return a - b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::MULTIPLY: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(binaryOp(lhs, rhs, [](auto a, auto b) {
                return a * b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::DIVIDE: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(binaryOp(lhs, rhs, [](auto a, auto b) {
                return a / b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::HALT: {
            // Halted
            systemHalted = true;
        } break;

        case Instruction::RETURN: {
            callStack.pop();
        } break;

        case Instruction::RETURN_V: {
            auto retval = callStack.back().opstack.pop();

            callStack.pop();

            if (callStack.size() > 0) {
                callStack.back().opstack.push(retval);
            } else {
                // Returning value to host
                return retval;
            }
        } break;

        case Instruction::CALL: {
            u64 jumpOffset = *(u64*)&bytecode[instructionPointer + sizeof(Instruction)];
            u8 argCount = *(u8*)&bytecode[instructionPointer + sizeof(Instruction) + sizeof(u64)];

            // advance ip in callee frame so when called frame is popped the ip is on the
            // instruction directly after the call
            instructionPointer += sizeof(Instruction) + sizeof(u64) + sizeof(u8);

            CallStackFrame frame;
            assert(argCount < frame.locals.size());
            for (int i=0; i<argCount; i++) {
                auto val = operandStack.pop();
                frame.locals[i] = val;
            }

            frame.ip = jumpOffset;
            callStack.push(frame);
        } break;

        case Instruction::DUMP: {
            callStack.dump();
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::LOAD_LOCAL: {
            u8 localIndex = *(u8*)&bytecode[instructionPointer + sizeof(Instruction)];

            auto local = locals.at(localIndex);
            assert(local.has_value());

            operandStack.push(local.value());
            instructionPointer += sizeof(Instruction) + sizeof(u8);
        } break;

        case Instruction::STORE_LOCAL: {
            u8 localIndex = *(u8*)&bytecode[instructionPointer + sizeof(Instruction)];
            locals.at(localIndex) = operandStack.pop();
            instructionPointer += sizeof(Instruction) + sizeof(u8);
        } break;

        case Instruction::STORE_LOCAL_NOPOP: {
            u8 localIndex = *(u8*)&bytecode[instructionPointer + sizeof(Instruction)];
            locals.at(localIndex) = operandStack.back();
            instructionPointer += sizeof(Instruction) + sizeof(u8);
        } break;

        case Instruction::CMP_EQ: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(compareOp(lhs, rhs, [](auto a, auto b) {
                return a == b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::CMP_NEQ: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(compareOp(lhs, rhs, [](auto a, auto b) {
                return a != b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::CMP_LT: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(compareOp(lhs, rhs, [](auto a, auto b) {
                return a < b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::CMP_GT: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(compareOp(lhs, rhs, [](auto a, auto b) {
                return a > b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::CMP_LTE: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(compareOp(lhs, rhs, [](auto a, auto b) {
                return a <= b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::CMP_GTE: {
            auto lhs = operandStack.pop();
            auto rhs = operandStack.pop();

            operandStack.push(compareOp(lhs, rhs, [](auto a, auto b) {
                return a >= b;
            }));
            
            instructionPointer += sizeof(Instruction);
        } break;

        case Instruction::JUMP: {
            u64 jumpOffset = *(u64*)&bytecode[instructionPointer + sizeof(Instruction)];

            instructionPointer = jumpOffset;
        } break;

        case Instruction::JUMP_COND: {
            u64 jumpOffset = *(u64*)&bytecode[instructionPointer + sizeof(Instruction)];

            Value cond = operandStack.pop();
            assert(cond.type == ValueType::U8);

            if (cond.data.u8) {
                instructionPointer = jumpOffset;
            } else {
                instructionPointer += sizeof(Instruction) + sizeof(u64);
            }
        } break;

        default: {
            assert(false);
        };
    }

    return std::nullopt;
}

void Snap::VM::dump() const {
    callStack.dump();
}

bool Snap::VM::isHalted() const {
    return systemHalted;
}