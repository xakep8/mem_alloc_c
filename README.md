# Custom Memory Allocator in C

A custom memory allocator implementation in C that provides `alloc()` and `dealloc()` functions as alternatives to the standard `malloc()` and `free()`.

## Overview

This project implements a simple memory allocator using a list-based approach. It manages a memory pool allocated using `sbrk()` system calls and maintains metadata for each memory block to track allocation status and size.

## Features

- **Custom Memory Allocation**: Drop-in replacement for `malloc()` with `alloc()`
- **Custom Deallocation**: Memory freeing with `dealloc()`
- **Memory Pool Management**: Uses `sbrk()` to request memory from the OS
- **8-byte Alignment**: Ensures proper memory alignment for performance
- **Block Metadata**: Tracks block size and allocation status using compact bit fields
- **Linked List Structure**: Maintains a list of allocated blocks for tracking

## Architecture

### Memory Block Structure

Each memory block contains:
- **Block Size** (8 bits): Size of the allocated block
- **Free Flag** (1 bit): Indicates if the block is free or allocated
- **Data**: The actual user data
- **Next Pointer**: Links to the next block in the list

### Memory Pool

- Initial pool size: 10 MB (configurable)
- Allocated using `sbrk()` system call
- Managed through a linked list of `mem_block` structures

## Building the Project

The project uses CMake as its build system.

```bash
# Create build directory
mkdir -p build && cd build

# Configure and build
cmake ..
make

# Run the test program
./mem_alloc
```

## Usage

Include the header file in your C program:

```c
#include "mem_alloc.h"

int main() {
    // Allocate memory for an array of 4 integers
    int* p = (int*)alloc(4 * sizeof(int));
    
    if (!p) {
        throw("Memory allocation failed");
    }
    
    // Use the allocated memory
    for (int i = 0; i < 4; i++) {
        p[i] = i * 10;
    }
    
    // Free the memory when done
    dealloc(p);
    
    return 0;
}
```

## API Reference

### `void* alloc(size_t size)`

Allocates a block of memory of the specified size.

- **Parameters**: `size` - Number of bytes to allocate
- **Returns**: Pointer to the allocated memory, or `NULL` on failure
- **Note**: Automatically initializes the memory pool on first call

### `void dealloc(void* p)`

Frees a previously allocated block of memory.

- **Parameters**: `p` - Pointer to the memory block to free
- **Note**: Does nothing if `p` is `NULL`

### `void init_allocate_memory_pool()`

Initializes the memory pool (called automatically by `alloc()` if needed).

### `throw(msg)`

Macro for error handling that prints an error message and exits.

## Current Limitations

- **No Memory Compaction**: Free blocks are not coalesced (defragmentation not implemented)
- **Simple Allocation Strategy**: Uses basic sequential allocation
- **Fixed Pool Size**: Initial 10 MB pool, grows with additional `sbrk()` calls
- **Limited Metadata**: 8-bit block size limits individual block sizes
- **No Thread Safety**: Not safe for concurrent use

## Future Improvements

Planned enhancements (as noted in the code):

- [ ] Implement `coalesce_free_blocks()` for memory defragmentation
- [ ] Add memory alignment optimizations
- [ ] Create a hybrid allocator using both list and page table approaches
- [ ] Improve allocation strategy (e.g., best-fit, first-fit)
- [ ] Add thread safety with mutexes
- [ ] Implement memory statistics and debugging features
- [ ] Add support for `realloc()` functionality

## Project Structure

```
mem_alloc_c/
├── CMakeLists.txt      # CMake build configuration
├── mem_alloc.h         # Memory allocator header and implementation
├── test.c              # Test program demonstrating usage
└── README.md           # This file
```

## Requirements

- **C Compiler**: GCC, Clang, or compatible C99 compiler
- **CMake**: Version 3.10 or higher
- **Operating System**: Unix-like systems (Linux, macOS) that support `sbrk()`

## Contributing

This is a personal learning project. Contributions, suggestions, and improvements are welcome!

## License

This project is open source and available for educational purposes.

## Author

Created as a learning exercise in memory management and systems programming.

---

**Note**: This is an educational implementation. For production use, consider well-tested allocators like jemalloc, tcmalloc, or the standard library allocator.
