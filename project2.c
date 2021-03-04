#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

// Global variables
int* array;
int arraySize;
int threshold;
bool shouldMultithread = true;
int numPartitions = 10;
int maxThreads = 4;

// Methods
bool isSorted();
void swap(int* a, int* b);
void quickSort(int p, int r);
int partition(int p, int r);
void shellSort(int low, int hi);

int main(int argc, char* argv[]) {
	// command line syntax is "project2 SIZE THRESHOLD [SEED [MULTITHREAD [PIECES [THREADS]]]]"
	printf("Found %d args\n", argc);
	if (argc < 3) {
		printf("Not enough args.\n");
		return 1;
	}
	arraySize = atoi(argv[1]);
	threshold = atoi(argv[2]);

	if (argc >= 4) {
		printf("Seed specified\n");
		// At least 4 args, must have a seed
		// TODO: Error Handling
		unsigned int seed = (unsigned int) strtoul(argv[3], NULL, 10);
		if (seed == 4294967295) { // Uint max, is probably -1, will handle better eventually
			printf("Using clock as seed\n");
			srand((unsigned int) clock());
		} else {
			printf("Seed %u\n", seed);
 			srand(seed);
		}
	}

	if (argc >= 5) {
		// At least 5 args, must have a multithread choice
		if (strncmp(argv[4], "n", 1) == 0|| strncmp(argv[4], "N", 1) == 0) {
			printf("Not multithreading\n");
			shouldMultithread = false;
		}
		else printf("Multithreading\n");
	}

	if (argc >= 6) {
		// At least 6 args, must specify number of pieces
		// TODO: Error Handling
		if (shouldMultithread) {
			int partsSpec = atoi(argv[5]);
			if (partsSpec > 0) numPartitions = partsSpec;
			printf("Number of partitions: %d\n", numPartitions);
		}
	}

	if (argc >= 7) {
		// Specifying max threads
		if (shouldMultithread) {
			int threadsSpec = atoi(argv[6]);
			if (threadsSpec > numPartitions) {
				printf("Too many threads specified\n");
				return 1;
			} else {
				maxThreads = threadsSpec;
				printf("Max threads: %d\n", maxThreads);
			}
		}
	}

	array = (int*) malloc(sizeof(int)*arraySize);

	for (int i = 0; i < arraySize; i++) {
		// Fill our dynamic array
		array[i] = i;
	}
	
	printf("Created array\n");

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

	printf("Randomized array\n");

	sorted = isSorted();

	if (sorted) {
		printf("Array is sorted!\n");
	} else {
		printf("Array is not sorted! :(\n");
	}

	printf("Testing quicksort\n");

	quickSort(0, arraySize - 1);

	sorted = isSorted();

	if (sorted) {
		printf("Array is sorted!\n");
	} else {
		printf("Array is not sorted! :(\n");
	}

	// Randomize again to test shellsort
	for (int i = 0; i < arraySize; i++) {
		int secondIndex = rand() % arraySize;
		swap(&array[i], &array[secondIndex]);
	}

	printf("Re-randomized array\n");

	sorted = isSorted();

	if (sorted) {
		printf("Array is sorted!\n");
	} else {
		printf("Array is not sorted! :(\n");
	}

	printf("Testing shellsort\n");

	shellSort(0, arraySize - 1);

	sorted = isSorted();

	if (sorted) {
		printf("Array is sorted!\n");
	} else {
		printf("Array is not sorted! :(\n");
	}
	return 0;

	free(array);
}

bool isSorted() {
	// Checks if the global array variable is sorted
	for (int i = 0; i < arraySize; i++) {
		if (array[i] != i) {
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

void quickSort(int p, int r) {
	if (p < r) { // base case: when down to 1 item, p == r
		if (threshold != 0 && r - p + 1 <= threshold) {
			// Shellsort instead of quicksort
			shellSort(p, r);			
		} else {
			int q = partition(p, r); // split the list, pivot is q
			quickSort(p, q - 1); // recursively quicksort the left side
			quickSort(q + 1, r); // recursively quicksort the right side
		}
	}
}

int partition(int p, int r) {
	int i = p;
	int j = r + 1;
	int x = array[p];
	do {
		do i++; while (array[i] < x);
		do j--; while (array[j] > x);
		if (i < j) {
			swap(&array[i], &array[j]);
		} else {
			break;
		}
	} while (true);
	swap(&array[p], &array[j]);
	return j;
}

void shellSort(int low, int hi) {
	int k = 1;
	int size = hi - low + 1;
	while (k <= size) {
		k *= 2;
	}
	k = (k / 2) - 1;

	do {
		for (int i = low; i < (low + size - k); i++) {
			for (int j = i; j >= low; j -= k) {
				if (array[j] <= array[j + k]) break;
				else swap(&array[j], &array[j+k]);
			}
		}
		k = k >> 1;
	} while (k > 0);
}