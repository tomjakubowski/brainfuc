#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *program;
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

int main(int argc, char **argv) {
    machine_t *m = malloc(sizeof(*m));

    machine_init(m, 30000);

    for (int i = 0; i < 6; i++) {
        machine_get(m);
        machine_next(m);
    }
    for (int i = 0; i < 6; i++) {
        machine_prev(m);
    }
    for (int i = 0; i < 6; i++) {
        machine_put(m);
        machine_next(m);
    }

    machine_free(m);
}
