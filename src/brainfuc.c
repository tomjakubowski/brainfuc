#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "machine.h"
#include "stack.h"

typedef struct {
    char *array;
    size_t array_size;
    char *ptr;
} Machine;

void machine_init(Machine *m, int array_size) {
    m->array_size = array_size;
    m->array = calloc(m->array_size, sizeof(*m->array));
    m->ptr = m->array;
}

void machine_next(Machine *state) {
    state->ptr++;
}

void machine_prev(Machine *state) {
    state->ptr--;
}

void machine_incr(Machine *state) {
    (*state->ptr)++;
}

void machine_decr(Machine *state) {
    (*state->ptr)--;
}

void machine_put(Machine *state) {
    putchar(*state->ptr);
}

void machine_get(Machine *state) {
    int c = getchar();
    // Choose to mimic Müller's Brainfuck behavior on EOF
    if (c != EOF) {
        *state->ptr = c;
    }
}

void machine_dump(Machine *state) {
    size_t max = 16;
    int const per_row = 16;

    printf("\n---- DUMP ----\n");
    for (size_t i = 0; i < max; i++) {
        printf("0x%02x%s", *(state->array + i),
                i % per_row == (per_row - 1) ? "\n" : " ");
    }
    printf("pointer at cell %zu with value: 0x%02x\n",
            (state->ptr - state->array), *state->ptr);
    printf("---- DUMP ----\n");
}

void machine_cleanup(Machine *m) {
    free(m->array);
}

void machine_exec(Machine *m, Instruction *program) {
    Instruction *inst_ptr = program;

    while (inst_ptr->op != OP_EOP) {
        Instruction *jump = NULL;
        switch (inst_ptr->op) {
            case OP_NEXT:
                machine_next(m);
                break;
            case OP_PREV:
                machine_prev(m);
                break;
            case OP_INCR:
                machine_incr(m);
                break;
            case OP_DECR:
                machine_decr(m);
                break;
            case OP_OUT:
                machine_put(m);
                break;
            case OP_INP:
                machine_get(m);
                break;
            case OP_DUMP:
                machine_dump(m);
                break;
            case OP_JUMPZ:
                if (*m->ptr == 0) {
                    jump = program + inst_ptr->addr;
                }
                break;
            case OP_JUMPNZ:
                if (*m->ptr != 0) {
                    jump = program + inst_ptr->addr;
                }
                break;
            case OP_NOP:
                break;
            default:
                assert("can't touch this!");
                break;
        }

        if (jump != NULL) {
            inst_ptr = jump;
        } else {
            inst_ptr++;
        }
    }
}

void read_source(char *filename, char **source_out, size_t *source_len_out) {
    FILE *fp = NULL;
    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: couldn't %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = 0,
           buffer_size = 1024;
    char *source = malloc(buffer_size),
         *buffer = source;

    while (fgets(buffer, buffer_size - bytes_read, fp)) {
        size_t len = strlen(buffer);
        bytes_read += len;
        if (buffer_size - bytes_read == 1) {
            buffer_size *= 2;
            source = realloc(source, buffer_size);
        }
        buffer = source + bytes_read;
    }

    if (ferror(fp)) {
        fprintf(stderr, "Error: couldn't read source from %s\n", filename);
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    *source_out = source;
    *source_len_out = bytes_read;
}

void compile(char *source, size_t source_len, Instruction **program_out) {
    Instruction *program = malloc((source_len + 1) * sizeof(Instruction));
    stack *jumps = NULL;

    for (size_t i = 0; i < source_len; i++) {
        char c = source[i];
        Instruction ins;

        switch (c) {
            case '>':
                ins.op = OP_NEXT;
                break;
            case '<':
                ins.op = OP_PREV;
                break;
            case '+':
                ins.op = OP_INCR;
                break;
            case '-':
                ins.op = OP_DECR;
                break;
            case '.':
                ins.op = OP_OUT;
                break;
            case ',':
                ins.op = OP_INP;
                break;
            case '[':
                ins.op = OP_JUMPZ;
                stack_push(&jumps, program + i);
                break;
            case ']':
                ins.op = OP_JUMPNZ;
                Instruction *match = stack_pop(&jumps);
                match->addr = i;
                ins.addr = (match - program);
                break;
            case '#':
                ins.op = OP_DUMP;
                break;
            default:
                ins.op = OP_NOP;
                break;
        }
        program[i] = ins;
    }
    program[source_len].op = OP_EOP;

    if (!stack_empty(jumps)) {
        printf("%p %p\n", jumps, jumps->next);
        fprintf(stderr, "Compile error: unmatched brackets\n");
        exit(EXIT_FAILURE);
    }
    *program_out = program;
}

void print_code(Instruction *program) {
    uint8_t indent = 0;
    while (program->op != OP_EOP) {
        switch (program->op) {
            case OP_NEXT:
                printf("%*s%s\n", indent, "", "next");
                break;
            case OP_PREV:
                printf("%*s%s\n", indent, "", "prev");
                break;
            case OP_INCR:
                printf("%*s%s\n", indent, "", "incr");
                break;
            case OP_DECR:
                printf("%*s%s\n", indent, "", "decr");
                break;
            case OP_OUT:
                printf("%*s%s\n", indent, "", "out");
                break;
            case OP_INP:
                printf("%*s%s\n", indent, "", "inp");
                break;
            case OP_JUMPZ:
                printf("%*s[ (0x%04x)\n", indent, "", (unsigned int) program->addr);
                indent += 2;
                break;
            case OP_JUMPNZ:
                indent -= 2;
                printf("%*s] (0x%04x)\n", indent, "", (unsigned int) program->addr);
                break;
            case OP_DUMP:
                printf("%*s%s\n", indent, "", "dump");
                break;
            default:
                assert("can't touch this");
                break;
        }
        program++;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *source = NULL;
    size_t source_len;

    read_source(argv[1], &source, &source_len);

    Instruction *program = NULL;
    compile(source, source_len, &program);

    Machine m;
    machine_init(&m, 30000);
    machine_exec(&m, program);
    machine_cleanup(&m);
}
