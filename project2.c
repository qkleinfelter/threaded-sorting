#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

// Global variables
int* array;
int arraySize;

// Methods
bool isSorted();
void swap(int* a, int* b);

int main(int argc, char* argv[]) {
	// command line syntax is "project2 SIZE THRESHOLD [SEED [MULTITHREAD [PIECES [THREADS]]]]"
	printf("Found %d args\n", argc);
	if (argc < 3) {
		printf("Not enough args.\n");
		return 1;
	}
	arraySize = atoi(argv[2]);

	array = (int*) malloc(sizeof(int)*arraySize);

	for (int i = 0; i < arraySize; i++) {
		// Fill our dynamic array
		array[i] = i;
	}
	
	bool sorted = isSorted();

	if (sorted) {
		printf("Array is sorted!\n");
	} else {
		printf("Array is not sorted! :(\n");
	}

	// Randomize array
	for (int i = 0; i < arraySize; i++) {
		int secondIndex = rand() % arraySize;
		swap(&array[i], &array[secondIndex]);
	}

	sorted = isSorted();
	
	if (sorted) {
		printf("Array is sorted!\n");
	} else {
		printf("Array is not sorted! :(\n");
	}
	return 0;
}

bool isSorted() {
	// Checks if the global array variable is sorted
	for (int i = 0; i < arraySize - 1; i++) {
		if (array[i] > array[i+1]) {
			return false;
		}
	}
	return true;
}

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}
