#ifndef SYSY_TARGET_ARM_H
#define SYSY_TARGET_ARM_H

#include "../ENUM.h"
#include "../ssa/link.h"
#define PC R15
#define LR R14
#define SP R13
#define IP R12
#define FP R11
#define V8 R11
#define V7 R10
#define V6 R9
#define SB R9
#define TR R9
#define V5 R8
#define V4 R7
#define V3 R6
#define V2 R5
#define V1 R4
#define A4 R3
#define A3 R2
#define A2 R1
#define A1 R0

struct Operand2 {
    int valuetype;  // REGISTER, INT
    union {
        int reg;
        int value;  // immi 8bit
    };
};

// OP_ADD: Rd:=Rn+operand2
// OP_SUB: Rd:=Rn-operand2
// OP_CMP: 更新Rn-operand2的CPSR标记
struct OP_Operand2 {
    list_entry_t list;
    int type;  // OP_ADD OP_SUB
    int Rd;
    int Rn;
    struct Operand2* operand2;
};

// OP_MUL: Rd:=Rm*Rs
// OP_SDIV: Rd:=Rm/Rs
// OP_BX: PC:=Rd
struct OP_REG3 {
    list_entry_t list;
    int type;
    int Rd;
    int Rm;
    int Rs;
};

// OP_B: PC:=label <+-32MB>
// OP_LABEL: just a label
// OP_BL: LR:=PC, PC:=label
struct OP_B {
    list_entry_t list;
    int type;
    const char* label;
};

// OP_LTR: Rd:=[Rd,offset]
// OP_STR:
struct OP_LS {
    list_entry_t list;
    int type;
    int Rd;
    int Rn;
    int offset;  // optional, immi
};

#endif