#ifndef Z_ALLOC_H_
#define Z_ALLOC_H_


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


// ----------------------------------------------------------------------------
// - Circular allocators ------------------------------------------------------
// ----------------------------------------------------------------------------


typedef struct circular_allocator_node_s circular_allocator_node_t;

struct circular_allocator_node_s {
    circular_allocator_node_t* next;
};

typedef struct {
    circular_allocator_node_t* first_free;
    circular_allocator_node_t* last_free;
    uint64_t*                  buffer;
    size_t                     buffer_size;
    size_t                     obj_size;
    bool                       is_static;
} circular_allocator_t;


/// 
/// Initializes a new Circular Allocator.
/// 
/// \param a a existing z_CircularAllocator.
/// \param buffer an array to be used as a back buffer.
/// \param bufferSize the size of the buffer in bytes.
/// \param objSize size of the object to be stored.
/// \param isStatic if 'false', when the z_CircularAllocator is unmade
///                 or deleted the back buffer will be freed.
/// 
bool make_circular_allocator(circular_allocator_t *restrict a,
                             uint64_t*                      buffer,
                             size_t                         buffer_size,
                             size_t                         obj_size,
                             bool                           is_static);

void unmake_circular_allocator(circular_allocator_t *restrict a);

circular_allocator_t* new_circular_allocator(uint64_t* buffer,
                                             size_t    buffer_size,
                                             size_t    obj_size,
                                             bool      is_static);

void delete_circular_allocator(circular_allocator_t **restrict a);


void* circular_allocator_alloc(circular_allocator_t *restrict a);

bool circular_allocator_free(circular_allocator_t *restrict a, void *restrict ptr);


// ----------------------------------------------------------------------------
// - Stack allocator ----------------------------------------------------------
//
// Memory layout:
//
// 15 |  .  |
// 14 |  .  |
// 13 |  .  |
// 12 |_____|
// 11 |__6__| <- index of block 1 first position
// 10 |     | <- end of block 1
//  9 |     |
//  8 |     |
//  7 |     |
//  6 |_____|
//  5 |__1__| <- beggining of block 1
//  4 |     | <- index of block 0's first position
//  3 |     | <- end of block 0
//  2 |_____|
//  1 |__0__| <- beggining of block 0
//  0 |     | <- dead byte
//       #
//
// ----------------------------------------------------------------------------


typedef struct {
    uint64_t* buffer;      // a 8-bytes-aligned chunck of memory
    size_t    buffer_size; // the length of the buffer
    size_t    index;       // current position at the buffer
    bool      is_static;   // is it a global static array?
} stack_allocator_t;


bool make_stack_allocator(stack_allocator_t *restrict a,
                          uint64_t*                   buffer,
                          size_t                      buffer_len,
                          bool                        is_static);

void unmake_stack_allocator(stack_allocator_t *restrict a);

/// 
/// Allocates a stack allocator handle.
/// 
/// \param buffer a 8-byte-aligned chunk of memory (global static or
///               malloced) that will be used as memory space for this
///               stack memory handle;
/// \param bufferLength the length of the buffer/array passed in (not
///                     in bytes, but in '8-bytes');
/// \param isStatic tells if the buffer is a global static array or
///                 (false) it was malloced. This flag is later used
///                 by 'z_StackAllocator_delete': if it is false, it
///                 will free;
/// 
stack_allocator_t* new_stack_allocator(uint64_t* buffer,
                                       size_t    buffer_len,
                                       bool      is_static);
/// 
/// Frees the memory allocated by 'z_StackAllocator_new'. If 'is_static'
/// was set to false, stdlib/free will be called on the buffer.
/// 
/// \param a a stack allocator handle;
/// 
void delete_stack_allocator(stack_allocator_t **restrict a);

/// 
/// Allocates 'size' bytes and returns a pointer to the first. Internally
/// it rounds 'size' to the next 8-byte block (for instance, for 13 bytes 
/// internally it'll be allocated 18 bytes, 21 will be 24 and so on) plus
/// 8 bytes for stack's internals.
/// 
/// \param a a stack allocator handle;
/// \param size the size in bytes of the chuck to be allocated;
/// 
void* stack_allocator_alloc(stack_allocator_t *restrict a, size_t size);

/// 
/// Frees the last allocated block of memory.
/// 
/// \param a a stack allocator handle;
/// 
void stack_allocator_free(stack_allocator_t *restrict a);

#endif // Z_ALLOC_H_