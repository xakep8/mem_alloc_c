#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// #include <sys/mman.h>

#define EXIT_FAILURE 1

#define throw(msg) {\
	fprintf(stderr, "Error: %s\n", msg);\
	exit(EXIT_FAILURE);\
}

typedef struct{
	unsigned int block_size : 8;
	unsigned int free : 1;
} size_free;

/*
*/
typedef struct{
	size_free info;
	char data[0];
	struct mem_block* next;
} mem_block;

/*
I will use this to keep track of the memory segments allocated.
This will help me defragment the memory later on.
also this will help me to find free blocks of memory when needed.
*/
struct mem_block* head = NULL;

/*
seems to be working fine for now.
didn't do any such optimizations yet.
or the alignment of the memory blocks.
Probably will do it later.

Future-self: if you see this, please optimize this code.
*/
void* alloc(size_t size){
	mem_block* ptr = (mem_block*)sbrk(size + sizeof(size_free));
	if (!ptr) return NULL;

	mem_block* block = (mem_block*)ptr;
	block->info.block_size = size;
	block->info.free = 1;
	block->next = NULL;
	return block;
}

void dealloc(void* p){
	if (!p) return;
	mem_block* block = (mem_block*)((char*)p - sizeof(size_free));
	block->info.free = 1;
}
