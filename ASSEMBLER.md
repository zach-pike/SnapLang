# Instructions
 - PUSH_U64
 - PUSH_U32
 - PUSH_U16
 - PUSH_U8
 - PUSH_I64
 - PUSH_I32
 - PUSH_I16
 - PUSH_I8
 - PUSH_F32
 - PUSH_F64

 - ADD
 - SUB
 - MULT
 - DIV

 - HALT
 - CALL
 - RETURN
 - RETURN_V

 - DUMP

 - STORE_LOCAL
 - LOAD_LOCAL


# Instruction how-to
---
## PUSH_(data type)
Take a single literal value (100, 700, 1000, -32) and pushes to stack

---

## Math functions (add, subtract, multiply, divide, etc..)
Pops two values off stack and tries to add them, expects the same data type for both values.
rhs is the top item on stack

Pushes result back to stack

---

## Function call
Push your args to the stack, use call and denote number of args to pop off stack
```
Test:
...

Main:
PUSH_U64 10
PUSH_U64 20
CALL Test 2
RETURN
```
`Test`s Call Stack Locals
| Idx |  0  |  1  |  2  |  3  |  4  |  5  |
|-----|-----|-----|-----|-----|-----|-----|
| Val | U64(10) | U64(20) | nil | nil | nil | nil |

To load the values onto the stack, refer to the next section

To return from a function, you use `RETURN` or `RETURN_V`

`RETURN` Returns with no value (ie just pops call stack frame)

`RETURN_V` Pops a item from the functions operand stack before return, then pushes it back to the original callers operand stack

In this example below, the function pushes a value onto the stack, which will be moved back down to the Main functions stack (1234 can be seen on the dump)

Code:
```
Test:
PUSH_U64 1234
RETURN_V

Main:
CALL Test
DUMP
RETURN
```
Output:
```
Call Stack Dump
-------- (Frame 00000) --------
IP: 45 
---- (Operand Stack Dump) -----
00000 | U64(1234)
------ (Frame 00000 END) ------
```

---

## *_LOCAL
Theese instructions do things with frame local variables

`STORE_LOCAL (u8)` Pops a value off the stack and store it in the slot specified by the argument
`LOAD_LOCAL (u8)` Reads a value from a slot specified by the arg and pushes it to the stack

---