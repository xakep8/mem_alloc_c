#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// #include <sys/mman.h>

/*
How about I create a page table for the memory pool?
Each entry in the page table will represent a block of memory.
The entry will have information about whether the block is free or allocated,
and the size of the block.

This way, when a request for memory allocation comes in,
I can look for a free block in the page table that is large enough to satisfy the request.
If found, I can mark that block as allocated in the page table and return a pointer to
the memory block.

When a request for deallocation comes in, I can mark the corresponding block as free in the page table.

Well I think a list allocator would be more efficient for now I'll keep it simple.
later I'll create a hybrid allocator using both list and page table.
*/

#define EXIT_FAILURE 1

#define throw(msg)                           \
	{                                        \
		fprintf(stderr, "Error: %s\n", msg); \
		exit(EXIT_FAILURE);                  \
	}

void *memory_pool = NULL; // allocate 10 MB initially

typedef struct
{
	unsigned int block_size : 8;
	unsigned int free : 1;
} size_free;

/*
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
	memory_pool = sbrk(10 * 1024 * 1024); // 10 MB
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
	mem_block *ptr = (mem_block *)sbrk(aligned_size);
	if (!ptr)
		return NULL;

	ptr->info.block_size = aligned_size;
	ptr->info.free = 0;
	ptr->next = NULL;
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