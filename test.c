#include <stdio.h>
#include "mem_alloc.h"

int main(){
	int* p = (int*)alloc(4 * sizeof(int));
	if (!p) {
		throw("Memory allocation failed");
	}
	for (int i = 0; i < 4; i++) {
		p[i] = i * 10;
	}
	for (int i = 0; i < 4; i++) {
		printf("%d %p\n", p[i], &p[i]);
	}
	printf("\n");
	return 0;
}