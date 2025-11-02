#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// #include <sys/mman.h>

#define EXIT_FAILURE 1

#define throw(msg)                           \
	{                                        \
		fprintf(stderr, "Error: %s\n", msg); \
		exit(EXIT_FAILURE);                  \
	}

/*
I'll track the memory pool with the start pointer in place and the size of the pool
is known and then the current pointer will increment and decrement as per the allocations
*/
const int size_of_memory_pool = 10 * 1024 * 1024; // 10 MB
void *memory_pool = NULL;						  // allocate 10 MB initially
void *current_brk = NULL;						  // current break pointer

typedef struct
{
	unsigned int block_size : 8;
	unsigned int free : 1;
} size_free;

/*
Structure to represent a memory block in the pool.
Basic list allocator structure.
 */
typedef struct
{
	size_free info;
	char data[0];
	struct mem_block *next;
} mem_block;

/*
I'll be using memory pool of 10 MB initially.

I'll be using this function to initialize the
memory pool. Then I'll be using my own alloc
and dealloc functions to manage memory within
this pool.
*/
void init_allocate_memory_pool()
{
	memory_pool = sbrk(size_of_memory_pool);
	if (memory_pool == (void *)-1)
	{
		throw("Initial memory pool allocation failed");
	}
	mem_block *first_block = (mem_block *)memory_pool;
	first_block->info.block_size = 1024 * 1024 - sizeof(size_free);
	first_block->info.free = 1;
	first_block->next = NULL;
	head = first_block;
}

/*
I will use this to keep track of the memory segments allocated.
This will help me defragment the memory later on.
also this will help me to find free blocks of memory when needed.
*/
struct mem_block *head = NULL;

/*
seems to be working fine for now.
didn't do any such optimizations yet.
or the alignment of the memory blocks.
Probably will do it later.

Future-self: if you see this, please optimize this code.
*/
void *alloc(size_t size)
{
	if (memory_pool == NULL)
	{
		init_allocate_memory_pool();
	}
	size_t total_size = size + sizeof(size_free);
	size_t aligned_size = (total_size + 7) & ~7; // align to 8 bytes
	// size to be allocate + current break pointer should not exceed memory pool
	if (current_brk + aligned_size > memory_pool + size_of_memory_pool)
	{
		throw("Out of memory in the memory pool");
	}
	mem_block *ptr = (mem_block *)current_brk + aligned_size;
	if (!ptr)
		return NULL;

	ptr->info.block_size = aligned_size;
	ptr->info.free = 0;
	ptr->next = NULL;
	current_brk += aligned_size;
	return ptr;
}

void dealloc(void *p)
{
	if (!p)
		return;
	mem_block *block = (mem_block *)((char *)p - sizeof(size_free));
	block->info.free = 1;
}

// merge adjacent free blocks
void coalesce_free_blocks()
{
}