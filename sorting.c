/*
================================================================================
 SORTING.C - SORTING ALGORITHMS IMPLEMENTATION
 
 Purpose: Implements three fundamental sorting algorithms for word sorting
 
 Algorithms:
  1. Bubble Sort - O(n²), simple comparison-based sort
  2. Quick Sort - O(n log n) average, efficient divide-and-conquer
  3. Merge Sort - O(n log n) stable sort with guaranteed performance
 
 Performance Comparison:
  - Bubble: Best for small datasets (< 50 items)
  - Quick: Best average performance on large datasets
  - Merge: Best for guaranteed O(n log n) performance
================================================================================
*/

#include "data_types.h"

// ============= Global Buffer for Merge Sort =============
// Dynamic buffer allocated on-demand to avoid stack overflow
static char (*mergeBuffer)[50] = NULL;
static int mergeBufferSize = 0;

// Initialize merge buffer on first use
static void initMergeBuffer(int size) {
    if (mergeBuffer == NULL || mergeBufferSize < size) {
        if (mergeBuffer) free(mergeBuffer);
        mergeBuffer = (char(*)[50])malloc(size * 50);
        if (!mergeBuffer) {
            fprintf(stderr, "ERROR: Cannot allocate merge buffer\n");
            exit(1);
        }
        mergeBufferSize = size;
    }
}

// Cleanup merge buffer (non-static so it can be called from main)
void cleanupMergeBuffer(void) {
    if (mergeBuffer) {
        free(mergeBuffer);
        mergeBuffer = NULL;
        mergeBufferSize = 0;
    }
}

// ============= Sorting Utility Functions =============

void swapWords(char a[50], char b[50]) {
    char temp[50];
    strcpy(temp, a);
    strcpy(a, b);
    strcpy(b, temp);
}

// ============= Bubble Sort Implementation =============

void bubbleSortWords(char words[][50], int count) {
    char temp[50];
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(words[j], words[j + 1]) > 0) {
                strcpy(temp, words[j]);
                strcpy(words[j], words[j + 1]);
                strcpy(words[j + 1], temp);
            }
        }
    }
}

// ============= Quick Sort Implementation =============

int partition(char words[][50], int low, int high) {
    // Store pivot value, not pointer (since array might move during swaps)
    char pivot[50];
    strcpy(pivot, words[high]);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (strcmp(words[j], pivot) < 0) {
            i++;
            swapWords(words[i], words[j]);
        }
    }
    swapWords(words[i + 1], words[high]);
    return i + 1;
}

void quickSortWords(char words[][50], int low, int high) {
    if (low < high) {
        int pi = partition(words, low, high);
        quickSortWords(words, low, pi - 1);
        quickSortWords(words, pi + 1, high);
    }
}

// ============= Merge Sort Implementation =============

void mergeWords(char words[][50], int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = 0;
    int mergeSize = right - left + 1;
    
    // Safety check
    if (k >= mergeBufferSize || mergeSize > mergeBufferSize) {
        fprintf(stderr, "ERROR: Merge buffer overflow! Size: %d, Buffer: %d\n", mergeSize, mergeBufferSize);
        return;
    }
    
    // Use pre-allocated global buffer instead of malloc
    // to avoid allocation failures on large datasets
    
    // Merge the two sorted subarrays
    while (i <= mid && j <= right) {
        if (strcmp(words[i], words[j]) <= 0) {
            strcpy(mergeBuffer[k++], words[i++]);
        } else {
            strcpy(mergeBuffer[k++], words[j++]);
        }
    }
    
    // Copy remaining elements from left subarray
    while (i <= mid) {
        strcpy(mergeBuffer[k++], words[i++]);
    }
    
    // Copy remaining elements from right subarray
    while (j <= right) {
        strcpy(mergeBuffer[k++], words[j++]);
    }
    
    // Copy sorted elements back to original array
    for (int i = left, k = 0; i <= right; i++, k++) {
        strcpy(words[i], mergeBuffer[k]);
    }
}

void mergeSortWordsHelper(char words[][50], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortWordsHelper(words, left, mid);
        mergeSortWordsHelper(words, mid + 1, right);
        mergeWords(words, left, mid, right);
    }
}

void mergeSortWords(char words[][50], int count) {
    if (count > 1) {
        initMergeBuffer(count);
        mergeSortWordsHelper(words, 0, count - 1);
    }
}