#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

typedef struct {
    char *array;
    size_t array_size;
    char *ptr;
} machine;

void machine_clear(machine *);

void machine_init(machine *m, int array_size) {
    m->array_size = array_size;
    m->array = calloc(m->array_size, sizeof(*m->array));
    m->ptr = m->array;
}

void machine_next(machine *state) {
    state->ptr++;
}

void machine_prev(machine *state) {
    state->ptr--;
}

void machine_incr(machine *state) {
    (*state->ptr)++;
}

void machine_decr(machine *state) {
    (*state->ptr)--;
}

void machine_put(machine *state) {
    putchar(*state->ptr);
}

void machine_get(machine *state) {
    int c = getchar();
    // Choose to mimic Müller's Brainfuck behavior on EOF
    if (c != EOF) {
        *state->ptr = c;
    }
}

void machine_dump(machine *state) {
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

void machine_cleanup(machine *m) {
    free(m->array);
}

void machine_exec(machine *m, char *program) {
    char *instruction = program;
    stack *jumps = NULL;

    while (*instruction != '\0') {
        char *jump = NULL;
        char c = *instruction;
        switch (c) {
            case '>':
                machine_next(m);
                break;
            case '<':
                machine_prev(m);
                break;
            case '+':
                machine_incr(m);
                break;
            case '-':
                machine_decr(m);
                break;
            case '.':
                machine_put(m);
                break;
            case ',':
                machine_get(m);
                break;
            case '#':
                machine_dump(m);
                break;
            case '[':
                if (*m->ptr == 0) {
                    /* Advance the instruction pointer past the matching ] */
                    stack *bracket_match = NULL;
                    stack_push(&bracket_match, instruction);
                    char *i = instruction;
                    while (*i++ != '\0' && !stack_empty(bracket_match)) {
                        if (*i == '[') {
                            stack_push(&bracket_match, i);
                        } else if (*i == ']') {
                            stack_pop(&bracket_match);
                        }
                    }
                    jump = i;
                } else {
                    stack_push(&jumps, instruction);
                }
                break;
            case ']':
                jump = stack_pop(&jumps);
                break;
            default:
                break;
        }

        if (jump != NULL) {
            instruction = jump;
        } else {
            instruction++;
        }
    }
}

void read_program(char *filename, char **program_out) {
    int const CHUNK_SIZE = 1024;
    char *program = malloc(CHUNK_SIZE);
    FILE *fp = NULL;

    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: couldn't %s for reading.\n", filename);
        exit(EXIT_FAILURE);
    }

    if (fgets(program, CHUNK_SIZE, fp) == NULL) {
        fprintf(stderr, "Error: couldn't read program from %s\n", filename);
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    *program_out = program;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *program = NULL;
    read_program(argv[1], &program);

    machine m;
    machine_init(&m, 30000);
    machine_exec(&m, program);
    machine_cleanup(&m);
}
