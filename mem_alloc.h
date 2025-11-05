#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/mman.h>
#include <string.h>

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
	size_t block_size;
	unsigned int free : 1;
} size_free;

/*
Structure to represent a memory block in the pool.
Basic list allocator structure.
 */
typedef struct mem_block
{
	size_free info;
	struct mem_block *next;
	char data[0];
} mem_block;

/*
I will use this to keep track of the memory segments allocated.
This will help me defragment the memory later on.
also this will help me to find free blocks of memory when needed.
*/
struct mem_block *head = NULL;
struct mem_block *tail = NULL;

/*
I'll be using memory pool of 10 MB initially.

I'll be using this function to initialize the
memory pool. Then I'll be using my own alloc
and dealloc functions to manage memory within
this pool.
*/
void init_allocate_memory_pool()
{
	memory_pool = mmap(NULL, size_of_memory_pool, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (memory_pool == MAP_FAILED)
	{
		throw("Initial memory pool allocation failed");
	}
	current_brk = memory_pool;
	tail = NULL;
	head = NULL;
}

static void maybe_split_block(mem_block *block, size_t size)
{
	size_t total_size = size + sizeof(mem_block);
	if (block->info.block_size >= total_size + sizeof(mem_block) + 8) // minimum split size
	{
		mem_block *new_block = (mem_block *)((char *)block + total_size);
		new_block->info.block_size = block->info.block_size - total_size;
		new_block->info.free = 1;
		new_block->next = block->next;

		block->info.block_size = total_size;
		block->next = new_block;
		if (tail == block)
			tail = new_block;
	}
}

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
	size_t total_size = size + sizeof(mem_block);
	size_t aligned_size = (total_size + 7) & ~7; // align to 8 bytes

	for (mem_block *cur = head; cur != NULL; cur = cur->next)
	{
		if (cur->info.free && cur->info.block_size >= aligned_size)
		{
			cur->info.free = 0;
			maybe_split_block(cur, size);
			return cur->data;
		}
	}

	// size to be allocate + current break pointer should not exceed memory pool
	if ((char *)current_brk + aligned_size > (char *)memory_pool + size_of_memory_pool)
	{
		throw("Out of memory in the memory pool");
	}
	mem_block *ptr = (mem_block *)current_brk;
	ptr->info.block_size = aligned_size;
	ptr->info.free = 0;
	ptr->next = head;
	if (!head)
	{
		head = ptr;
		tail = ptr;
	}
	else
	{
		tail->next = ptr;
		tail = ptr;
	}

	current_brk = (char *)current_brk + aligned_size;
	return ptr->data;
}

// merge adjacent free blocks
void coalesce_free_blocks()
{
	mem_block *cur = head;
	while (cur && cur->next)
	{
		mem_block *nxt = cur->next;
		if (cur->info.free && nxt->info.free &&
			(char *)cur + cur->info.block_size == (char *)nxt)
		{
			// merge nxt into cur
			cur->info.block_size += nxt->info.block_size;
			cur->next = nxt->next;
			if (tail == nxt)
				tail = cur;
			// stay on cur to check further merges
		}
		else
		{
			cur = cur->next;
		}
	}
}

void cleanup_memory_pool()
{
	if (memory_pool)
	{
		munmap(memory_pool, size_of_memory_pool);
		memory_pool = NULL;
		current_brk = NULL;
		head = NULL;
		tail = NULL;
	}
}

void dealloc(void *p)
{
	if (!p)
		return;
	mem_block *block = (mem_block *)((char *)p - offsetof(mem_block, data));
	size_t payload = block->info.block_size - sizeof(mem_block);
	memset(block->data, 0, payload);
	block->info.free = 1;
	coalesce_free_blocks();
}