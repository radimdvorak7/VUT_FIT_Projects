// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný (xslany03)

// Dynamic array type (vector) and its associated functions.

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    // Inner buffer of the allocated bytes.
    // This buffer should not *ever* be accessed directly as it may lead to
    // inconsistent state of this struct
    void* inner;

    // Number of elements the `inner` buffer can hold.
    size_t capacity;

    // Current amount of elements within the buffer.
    size_t len;

    // Size of each elemnt within the `inner` buffer.
    size_t elem_size;

} vector;

vector* vec_init(size_t init_capacity, const size_t sizeof_type);
void vec_free(vector** vec);
void* vec_leak(vector** vec);

bool vec_grow(vector* vec);
bool vec_resize(vector* vec, size_t num_elements);

void* vec_get(vector* vec, size_t index);
void* vec_push(vector* vec, void* element);
char* vec_pushchar(vector* vec, const char c);
void* vec_pop(vector* vec);
void vec_remove(vector* vec, size_t index);
void vec_swap_remove(vector* vec, size_t index);
void vec_clear(vector* vec);

char* vec_to_str(vector** vec);
char* vec_get_str(vector* vec);

#define vec_remove_last(vec) vec_remove((vec), (vec)->len - 1)
#endif /* VECTOR_H */
