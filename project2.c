/*
	File: project2.c - Implements a hybrid sorting process in C

	This file implements a hybrid sort, that uses both the quickSort
	and shellSort algorithms, as well as posix style pthreads
	to quickly and effectively sort large arrays multithreaded.
	Takes in between 2 and 6 arguments, specifying, in order, the size of the array,
	when to switch to shellsort, the seed for the random number generator,
	whether or not to multithread, the number of pieces to partition the
	array into before sorting, and the maximum number of threads to spin up concurrently.

	Author: Quinn Kleinfelter
	Class: EECS 3540-001 Operating Systems & Systems Programming Spring 2021
	Instructor: Dr. Thomas
	Last Edited: 3/8/21
	Copyright: Copyright 2021 by Quinn Kleinfelter. All rights reserved.
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#define NO_SEED -2

// Global variables
int* array;
int arraySize;
int threshold; // when we switch from quicksort to shellsort

// Methods
bool isSorted();
void swap(int* a, int* b);
void hybridSort(int p, int r);
int partition(int p, int r);
void shellSort(int low, int hi);
void* runner(void* parameters);

// Struct for passing L and R to worker threads
typedef struct {
	int L;
	int R;
} range;

int main(int argc, char* argv[]) {
	// variables for various things
	// (its a surprise tool that will help us later)
	int seed = NO_SEED;
	double createTime, initTime, shuffleTime, partitionTime, sortingWallClock, sortingCPU, overallWallClock, overallCPU;
	bool shouldMultithread = true; 
	int numPartitions = 10; // number of pieces to split the array into
	int maxThreads = 4; // max threads to run at once

	// Clock values for the beginning of everything
	clock_t veryStart = clock();
	struct timeval veryStartWall;
	gettimeofday(&veryStartWall, NULL);

	if (argc < 3) {
		printf("Correct usage: project2 SIZE THRESHOLD [SEED [MULTITHREAD [PIECES [THREADS]]]]\n");
		return 1;
	}
	arraySize = atoi(argv[1]);
	threshold = atoi(argv[2]);

	if (argc >= 4) {
		// At least 4 args, must have a seed
		if (atoi(argv[3]) == -1) {
			seed = clock();
			srand(seed);
		} else {
			seed = atoi(argv[3]);
			srand(atoi(argv[3]));
		}
	}

	if (argc >= 5) {
		// At least 5 args, must have a multithread choice
		if (strncmp(argv[4], "n", 1) == 0|| strncmp(argv[4], "N", 1) == 0) {
			shouldMultithread = false;
		}
	}

	if (argc >= 6) {
		// At least 6 args, must specify number of pieces
		if (shouldMultithread) {
			int partsSpec = atoi(argv[5]);
			if (partsSpec > 0) numPartitions = partsSpec;
		} // if we aren't multithreading, then it doesn't matter whats specified
	}

	if (argc >= 7) {
		// Specifying max threads
		if (shouldMultithread) {
			int threadsSpec = atoi(argv[6]);
			if (threadsSpec > numPartitions) {
				// We can't have more threads than pieces
				printf("Too many threads specified\n");
				return 1;
			} else {
				maxThreads = threadsSpec;
			}
		} // again, requires multithreading
	}

	// Handles the case where a user inputs a number of partitions that is less than
	// maxThreads's default value of 4, and doesn't enter a maxThreads value
	if (maxThreads > numPartitions) {
		printf("Can't have more threads than partitions\n");
		return 1;
	}

	// Creating the array & timing it
	clock_t start = clock();
	array = (int*) malloc(sizeof(int)*arraySize);
	clock_t end = clock();
	createTime = (double) (end - start) / 1000000;

	// Initializing the array & timing it
	start = clock();
	for (int i = 0; i < arraySize; i++) {
		// Fill our dynamic array
		array[i] = i;
	}
	end = clock();
	initTime = (double) (end - start) / 1000000;

	// Randomizing the array & timing it
	start = clock();
	for (int i = 0; i < arraySize; i++) {
		int secondIndex = rand() % arraySize;
		swap(&array[i], &array[secondIndex]);
	}
	end = clock();
	shuffleTime = (double) (end - start) / 1000000;

	// timing structure, used either in multithreaded mode or not, so keep it out of the if
	struct timeval startTime;
	if (shouldMultithread) {
		// If we're multithreading, the first thing we need to do is partition
		// the array into numPartitions pieces
		
		// Start timing how long it takes to partition
		start = clock();
		// we start currPieces at 1 because we're going to make the first piece before the loop
		int currPieces = 1;
		// Array of ranges which are the partitions we have currently
		range pieces[numPartitions];

		// Our first "piece" is the whole array
		pieces[0].L = 0;
		pieces[0].R = arraySize - 1;

		while (currPieces < numPartitions) {
			// While we have less pieces than the number of partitions specified
			// we loop and partition continuously

			// Finding the largest partition, start with the first one
			range* largest = &pieces[0];
			int largestSize = largest->R - largest->L + 1;

			// Loop through the current pieces we have
			for (int i = 1; i < currPieces; i++) {
				range* current = &pieces[i];
				int currSize = current->R - current->L + 1;

				// if the size of the current piece is greater than the largest
				// then we need to make this one the largest
				if (currSize > largestSize) {
					largest = current;
					largestSize = currSize;
				}
			}

			// Now that we've determined our largest piece, we can partition it into 
			// 2 smaller pieces, with the pivot value being stored in midpt
			int midpt = partition(largest->L, largest->R);

			// the new partition always goes on the spot 1 after our last one in the list
			// which is located at currPieces because that is 1-indexed while the array is 0-indexed
			range* newPart = &pieces[currPieces];

			// this new piece needs to be the right half of the partition so we start it at the midpt + 1
			// and it goes until the largest's right half
			newPart->L = midpt + 1; 
			newPart->R = largest->R;

			// then, we change the largest's right half value to be the midpt - 1
			// so that it becomes the left half of the partition
			largest->R = midpt - 1;

			// finally, we can increment currPieces and continue looping
			currPieces++;
		}
		// Once we're done partitioning we can finish the timing and print it
		end = clock();
		partitionTime = (double) (end - start) / 1000000;

		// Now we have the list of partitions, so we should sort these in descending
		// order by size, so we can shoot them off into threads largest -> smallest
		// this uses insertion sort for now
		for (int i = 1; i < numPartitions; i++) {
			range current = pieces[i];
			int j = i - 1;

			while(j >= 0 && current.R - current.L + 1 > pieces[j].R - pieces[j].L + 1) {
				pieces[j + 1] = pieces[j];
				--j;
			}
			pieces[j + 1] = current;
		}

		// Now that we have our list of partitions sorted appropriately we can start the timing for
		// the multithreading and get to work spawning out multiple threads
		start = clock();
		gettimeofday(&startTime, NULL);

		// Now start spawning threads to sort
		// Array of threads & thread attributes
		pthread_t threads[maxThreads];
		pthread_attr_t threadAttributes[maxThreads];
		// variable to keep track of the next partition to shoot off
		int nextPartition = 0;

		for (int i = 0; i < maxThreads; i++) {
			// Spawn off our initial maxThreads threads

			// grab the current piece
			range* piece = &pieces[nextPartition];
			// setup its attributes
			pthread_attr_init(&threadAttributes[i]);
			// and create the thread
			pthread_create(&threads[i], &threadAttributes[i], runner, piece);
			nextPartition++; // then increment which partition we're working on
		}

		// Now that we've spawned our first set of threads, we need to check repeatedly
		// to continue spawning the rest of our threads
		while (nextPartition < numPartitions) {
			for (int i = 0; i < maxThreads && nextPartition < numPartitions; i++) {
				// grab the thread at our current index
				pthread_t* currThread = &threads[i];

				// Check if the current thread has been completed
				// and if it has, spawn another one
				if (pthread_tryjoin_np(*currThread, NULL) == 0) {
					// grab the next one we need to spawn
					range* piece = &pieces[nextPartition];
					// setup its attributes
					pthread_attr_init(&threadAttributes[i]);
					// and create the thread
					pthread_create(&threads[i], &threadAttributes[i], runner, piece);
					// increment the partition we're working on
					nextPartition++;
				}
			}
			// Don't poll the threads again immediately, because its unlikely they've finished
			// instead wait 50ms
			usleep(50000);
		}

		// Now we can loop through our remaining running threads
		// and wait for them to terminate to join them up
		for (int i = 0; i < maxThreads; i++) {
			// join waits for the thread to terminate before returning
			// so we can just loop through them all once
			pthread_join(threads[i], NULL);
		}
	} else {
		// No multithreading, so we can just sort & time it
		// also we set numPartitions and maxThreads to 1, so that it prints correctly
		numPartitions = 1;
		maxThreads = 1;
		start = clock();
		gettimeofday(&startTime, NULL);
		hybridSort(0, arraySize - 1);
	}

	// At this point the array is sorted, so we can finish up the timing and print the results
	end = clock();
	struct timeval endTime;
	gettimeofday(&endTime, NULL);
	double timeTakenSort = (endTime.tv_sec - startTime.tv_sec) * 1e6;
	timeTakenSort = (timeTakenSort + (endTime.tv_usec - startTime.tv_usec)) * 1e-6;
	sortingCPU = (double) (end - start) / 1000000;
	sortingWallClock = timeTakenSort;

	// as well as finish up the timing for the overall program
	clock_t veryEnd = clock();
	struct timeval veryEndWall;
	gettimeofday(&veryEndWall, NULL);
	double timeTakenOverall = (veryEndWall.tv_sec - veryStartWall.tv_sec) * 1e6;
	timeTakenOverall = (timeTakenOverall + (veryEndWall.tv_usec - veryStartWall.tv_usec)) * 1e-6;
	overallCPU = (double) (veryEnd - veryStart) / 1000000;
	overallWallClock = timeTakenOverall;

	// Make sure the array is actually sorted before we finish the program!!!
	// Should be sorted
	if (!isSorted()) {
		printf("Array is not sorted!!!\n");
	}

	// Print out our final summary statistics & labels
	printf("     SIZE    THRESHOLD  SEED  PC T CREATE   INIT  SHUFFLE   PART  SrtWall Srt CPU ALLWall ALL CPU\n");
	printf("   --------- --------- ------ -- - ------ ------- ------- ------- ------- ------- ------- -------\n");
	printf("F: %9d ", arraySize);
	printf("%9d ", threshold);
	if (seed != NO_SEED) {
		printf("%6d ", seed);
	} else {
		printf("    00 ");
	}
	printf("%2d ", numPartitions);
	printf("%1d ", maxThreads);
	printf("%6.3f ", createTime);
	printf("%7.3f ", initTime);
	printf("%7.3f ", shuffleTime);
	printf("%7.3f ", partitionTime);
	printf("%7.3f ", sortingWallClock);
	printf("%7.3f ", sortingCPU);
	printf("%7.3f ", overallWallClock);
	printf("%7.3f\n", overallCPU);

	// free up the array
	free(array);
	return 0;
}

bool isSorted() {
	// Checks if the global array variable is sorted
	for (int i = 0; i < arraySize; i++) {
		if (array[i] != i) {
			// if the array position isn't equivalent to its number
			// we know it isn't sorted since our array is from 0 to arraySize - 1
			return false;
		}
	}
	// if we make it here, then we must have a sorted array
	return true;
}

// helper function that swaps 2 int variables
void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void hybridSort(int p, int r) {
	if (p < r) { // base case: when down to 1 item, p == r
		int size = r - p + 1;
		if (size < 2) return; // nothing to sort
		if (size == 2) {
			// check if we need to swap our 2 elements
			if (array[p] > array[r]) {
				swap(&array[p], &array[r]);
			}
		} else if (size <= threshold) {
			// Shellsort instead of quicksort
			// if our remaining number of elements is less than the threshold
			shellSort(p, r);			
		} else {
			// otherwise continue quicksorting
			int q = partition(p, r); // split the list, pivot is q
			hybridSort(p, q - 1); // recursively quicksort the left side
			hybridSort(q + 1, r); // recursively quicksort the right side
		}
	}
}

int partition(int p, int r) {
	// start i at the left side
	int i = p;
	// and j one past the right side
	int j = r + 1;
	// with x being the value on the left
	int x = array[p];
	do {
		// increase i over and over again while its value is less than the leftmost value
		do i++; while (array[i] < x);
		// decrease j over and over again while its value is greater than the leftmost value
		do j--; while (array[j] > x);
		if (i < j) {
			// if i < j we need to swap the values at i and j
			swap(&array[i], &array[j]);
		} else {
			// otherwise break out
			break;
		}
	} while (true);
	// then swap the values at p and j
	swap(&array[p], &array[j]);
	// and return j, our pivot value
	return j;
}

void shellSort(int low, int hi) {
	// hibbards sequence
	int k = 1;
	int size = hi - low + 1;
	while (k <= size) {
		k *= 2;
	}
	k = (k / 2) - 1;

	do {
		for (int i = low; i < (low + size - k); i++) { // for each comb position
			for (int j = i; j >= low; j -= k) { // tooth to tooth is k
				if (array[j] <= array[j + k]) break; // move upstream/exit?
				else swap(&array[j], &array[j+k]);
			}
		}
		k = k >> 1;
	} while (k > 0);
}

void* runner(void* parameters) {
	// runner for our multithreading
	// grab the range specified in the parameters
	range* params = (range*) parameters;
	// Print the left side, right side, and size as we start the thread
	printf("(%9d, %9d, %9d)\n", params->L, params->R, params->R - params->L + 1);
	// quicksort on the thread from the left to the right
	hybridSort(params->L, params->R);
	// and exit the thread once we're here
	pthread_exit(0);
}
