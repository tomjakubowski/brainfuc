#ifndef BRAINFUC_OPS_H
#define BRAINFUC_OPS_H

#include <stdlib.h>

enum Opcode {
    OP_NEXT,   // >
    OP_PREV,   // <
    OP_INCR,   // +
    OP_DECR,   // -
    OP_OUT,    // .
    OP_INP,    // ,
    OP_JUMPZ,  // [
    OP_JUMPNZ, // ]
    OP_DUMP,   // #
    OP_EOP,     // end of program
    OP_NOP
};

typedef struct {
    enum Opcode op;
    size_t addr;
} Instruction;

#endif
