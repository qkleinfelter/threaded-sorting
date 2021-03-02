#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
	// command line syntax is "project2 SIZE THRESHOLD [SEED [MULTITHREAD [PIECES [THREADS]]]]"
	printf("Found %d args\n", argc);
	if (argc < 3) {
		printf("Not enough args.");
		return 1;
	}
	int size = atoi(argv[2]);

	int* array = (int*) malloc(sizeof(int)*size);

	for (int i = 0; i < size; i++) {
		// Fill our dynamic array
		array[i] = i;
	}

	return 0;
}
