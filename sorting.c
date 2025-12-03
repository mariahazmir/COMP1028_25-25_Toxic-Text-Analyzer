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
    char* pivot = words[high];
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
    int tempSize = right - left + 1;
    
    // Temporary array to hold merged data
    char (*temp)[50] = malloc(tempSize * sizeof(char[50]));
    if (!temp) return;
    
    // Merge the two sorted subarrays
    while (i <= mid && j <= right) {
        if (strcmp(words[i], words[j]) <= 0) {
            strcpy(temp[k++], words[i++]);
        } else {
            strcpy(temp[k++], words[j++]);
        }
    }
    
    // Copy remaining elements from left subarray
    while (i <= mid) {
        strcpy(temp[k++], words[i++]);
    }
    
    // Copy remaining elements from right subarray
    while (j <= right) {
        strcpy(temp[k++], words[j++]);
    }
    
    // Copy sorted elements back to original array
    for (int i = left, k = 0; i <= right; i++, k++) {
        strcpy(words[i], temp[k]);
    }
    
    free(temp);
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
        mergeSortWordsHelper(words, 0, count - 1);
    }
}