#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

typedef struct {
    char *array;
    size_t array_size;
    char *ptr;
} machine_t;

void machine_clear(machine_t *);

void machine_init(machine_t *m, int array_size) {
    m->array_size = 30000;
    machine_clear(m);
    m->ptr = m->array;
}

void machine_next(machine_t *state) {
    state->ptr++;
}

void machine_prev(machine_t *state) {
    state->ptr--;
}

void machine_incr(machine_t *state) {
    (*state->ptr)++;
}

void machine_decr(machine_t *state) {
    (*state->ptr)--;
}

void machine_put(machine_t *state) {
    putchar(*state->ptr);
}

void machine_get(machine_t *state) {
    int c = getchar();
    // Choose to mimic Müller's Brainfuck behavior on EOF
    if (c != EOF) {
        *state->ptr = c;
    }
}

void machine_free(machine_t *state) {
    free(state->array);
    free(state);
}

void machine_dump(machine_t *state) {
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

void machine_clear(machine_t *m) {
    free(m->array);
    m->array = calloc(m->array_size, sizeof(*m->array));
}

void machine_exec(machine_t *m, char *program) {
    char *instruction = program;
    char c;
    stack *jumps = NULL;

    while ((c = *instruction) != '\0') {
        char *jump = NULL;
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
        }

        if (jump != NULL) {
            instruction = jump;
        } else {
            instruction++;
        }
    }
}

char *read_program(char *filename) {
    int const CHUNK_SIZE = 1024;
    char *program = malloc(CHUNK_SIZE * sizeof(*program));

    FILE *fd = NULL;

    if ((fd = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: couldn't %s for reading.\n", filename);
        exit(1);
    }

    if (fgets(program, CHUNK_SIZE, fd) == NULL) {
        fprintf(stderr, "Error: couldn't read program from %s\n", filename);
        fclose(fd);
        exit(1);
    }
    fclose(fd);
    return program;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    char *program = read_program(argv[1]);
    machine_t *m = malloc(sizeof(*m));
    machine_init(m, 30000);
    machine_exec(m, program);
    machine_free(m);
    free(program);
}
