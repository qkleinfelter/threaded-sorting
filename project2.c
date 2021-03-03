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
void quickSort(int p, int r);
int partition(int p, int r);
void shellSort();

int main(int argc, char* argv[]) {
	// command line syntax is "project2 SIZE THRESHOLD [SEED [MULTITHREAD [PIECES [THREADS]]]]"
	printf("Found %d args\n", argc);
	if (argc < 3) {
		printf("Not enough args.\n");
		return 1;
	}
	arraySize = atoi(argv[1]);

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

	shellSort();

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
		int q = partition(p, r); // split the list, pivot is q
		quickSort(p, q - 1); // recursively quicksort the left side
		quickSort(q + 1, r); // recursively quicksort the right side
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

void shellSort() {
	int k = 1;
	while (k <= arraySize) {
		k *= 2;
		k = (k / 2) - 1;
	}

	do {
		for (int i = 0; i < (arraySize - k); i++) {
			for (int j = i; j >= 0; j -= k) {
				if (array[j] <= array[j + k]) break;
				else swap(&array[j], &array[j+k]);
			}
		}
		k = k >> 1;
	} while (k > 0);
}