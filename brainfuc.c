#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *array;
    size_t array_size;
    char *ptr;
} machine_t;

void machine_init(machine_t *state, int array_size) {
    state->array_size = 30000;
    state->array = calloc(state->array_size, sizeof(*state->array));
    state->ptr = state->array;
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
    *state->ptr = getchar();
}

void machine_free(machine_t *state) {
    free(state->array);
    free(state);
}

void machine_dump(machine_t *state) {
    size_t max = state->array_size;
    int const per_row = 8;

    printf("\n---- DUMP ----\n");
    for (size_t i = 0; i < max; i++) {
        printf("%08x%s",
                *(state->array + i),
                i % per_row == (per_row - 1) ? "\n" : " ");
    }
}

void machine_exec(machine_t *m, char *program) {
    char *inst = program;
    char c;
    while ((c = *inst++) != '\0') {
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
        }
    }
}

char *read_program(char *filename) {
    int const CHUNK_SIZE = 1024;
    char *program = malloc(CHUNK_SIZE * sizeof(char));

    FILE *fd = NULL;

    if ((fd = fopen(filename, "r")) == NULL) {
        printf("Error opening %s for reading.\n", filename);
        exit(1);
    }

    if (fgets(program, CHUNK_SIZE, fd) == NULL) {
        printf("Error reading program from %s\n", filename);
        fclose(fd);
        exit(2);
    }
    fclose(fd);
    return program;
}

int main(int argc, char **argv) {
    char *program = read_program("test.bf");
    machine_t *m = malloc(sizeof(*m));
    machine_init(m, 30000);
    machine_exec(m, program);
    machine_free(m);
    free(program);
}
