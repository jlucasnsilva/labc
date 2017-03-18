#include "alloc.h"
#include "util.h"

#include <stdio.h>


// ----------------------------------------------------------------------------
// - CIRCULAR ALLOCATOR -------------------------------------------------------
// ----------------------------------------------------------------------------


bool make_circular_allocator(circular_allocator_t *restrict a,
                             uint64_t*                      buffer,
                             size_t                         buffer_size,
                             size_t                         obj_size,
                             bool                           is_static)
{
    if (!a || !buffer || buffer_size < obj_size) {
        return false;
    }

    if (a && obj_size < buffer_size) {
        deftype(blockptr, bptr_t, obj_size);
        circular_allocator_node_t* first = Ptr(buffer);
        circular_allocator_node_t* last  = NULL;
        bptr_t                     buff  = Ptr(buffer);

        for (int i = 1; i < (buffer_size / obj_size); i++) {
            last = Ptr(&buff[i - 1]);
            last->next = Ptr(&buff[i]);
        }

        last->next->next = NULL;

        *a = (circular_allocator_t) {
            .buffer      = buffer,
            .buffer_size = buffer_size,
            .obj_size    = obj_size,
            .is_static   = is_static,
            .first_free  = first,
            .last_free   = last
        };
    }

    return true;
}

void unmake_circular_allocator(circular_allocator_t *restrict a)
{
    if (a && !a->is_static) {
        delete(a->buffer);
    }
}

circular_allocator_t* new_circular_allocator(uint64_t* buffer,
                                             size_t    buffer_size,
                                             size_t    obj_size,
                                             bool      is_static)
{
    if (!buffer || buffer_size < obj_size) {
        return NULL;
    }

    circular_allocator_t* a = new(circular_allocator_t);
    make_circular_allocator(a, buffer, buffer_size, obj_size, is_static);
    
    return a;
}

void delete_circular_allocator(circular_allocator_t **restrict a)
{
    if (a && *a) {
        if ( !(*a)->is_static ) {
            delete((*a)->buffer);
        }

        delete(*a);
    }
}

void* circular_allocator_alloc(circular_allocator_t *restrict a)
{
    void* ptr = NULL;

    if (a && a->first_free) {
        ptr = a->first_free;
        a->first_free = a->first_free->next;
    }

    return ptr;
}

bool circular_allocator_free(circular_allocator_t *restrict a, void *restrict ptr)
{
    char*  cbuff    = Ptr(a->buffer);
    size_t last_idx = a->buffer_size - a->obj_size;

    if (a && ptr) {
        if (ptr >= Ptr(&cbuff[0]) && ptr <= Ptr(&cbuff[last_idx])) {
            a->last_free->next = ptr;
            a->last_free       = ptr;
        } else {
            // puts("circular allocator error: trying to free chuck of memory that doesn't belong to this pool!");
            return false;
        }
    }

    return true;
}


// ----------------------------------------------------------------------------
// - STACK ALLOCATOR ----------------------------------------------------------
// ----------------------------------------------------------------------------


bool make_stack_allocator(stack_allocator_t *restrict a,
                          uint64_t*                   buffer,
                          size_t                      buffer_len,
                          bool                        is_static)
{
    if (!a || !buffer || buffer_len < 1) {
        return false;
    }

    if (a) {
        *a = (stack_allocator_t) {
            .buffer      = buffer,
            .buffer_size = buffer_len - 1,
            .index       = 0,
            .is_static   = is_static,
        };
    }

    return true;
}

void unmake_stack_allocator(stack_allocator_t *restrict a)
{
    if (a && !a->is_static) {
        delete(a->buffer);
    }
}

stack_allocator_t* new_stack_allocator(uint64_t* buffer,
                                       size_t    buffer_len,
                                       bool      is_static)
{
    if (!buffer || buffer_len < 1) {
        return NULL;
    }

    stack_allocator_t* a = new(stack_allocator_t);
    make_stack_allocator(a, buffer, buffer_len, is_static);

    return a;
}

void delete_stack_allocator(stack_allocator_t **restrict a)
{
    if (a && *a) {
        if ( !(*a)->is_static ) {
            delete((*a)->buffer);
        }

        delete(*a);
    }
}

static inline size_t sa_roundup(float x)
{
    return (size_t) (x + 0.99f);
}


void* stack_allocator_alloc(stack_allocator_t *restrict a, size_t size)
{
    uint64_t* block     = NULL;
    // the +1 is the spot to hold the index where the block starts
    size_t    realSize  = 1 + sa_roundup(Float(size) / sizeof(uint64_t));

    if (a && realSize <= a->buffer_size) {
        size_t index = a->index + 1;
        
        block = &a->buffer[index];

        a->index            += realSize;
        a->buffer[a->index]  = index;
        a->buffer_size      -= realSize;
    }

    return Ptr(block);
}

void stack_allocator_free(stack_allocator_t *restrict a)
{
    if (a && a->buffer[a->index] > 0) {
        size_t s        = a->index - a->buffer[a->index] + 1;
        a->index        = a->buffer[a->index] - 1;
        a->buffer_size += s;
    }
}
