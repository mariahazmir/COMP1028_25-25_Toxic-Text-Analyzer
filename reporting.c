#include "data_types.h"
#include "colors.h"

// Compare function for sorting by frequency (descending)
int compareByFreqDesc(const void *a, const void *b) {
    struct WordFreq *wa = (struct WordFreq *)a;
    struct WordFreq *wb = (struct WordFreq *)b;
    return wb->freq - wa->freq; // Descending order
}

// ============= Bubble Sort for WordFreq Structs =============

void bubbleSortWordFreq(struct WordFreq *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].freq < arr[j + 1].freq) {
                struct WordFreq temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// ============= Quick Sort for WordFreq Structs =============

int partitionWordFreq(struct WordFreq *arr, int low, int high) {
    struct WordFreq pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].freq > pivot.freq) {
            i++;
            struct WordFreq temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    struct WordFreq temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

void quickSortWordFreq(struct WordFreq *arr, int low, int high) {
    if (low < high) {
        int pi = partitionWordFreq(arr, low, high);
        quickSortWordFreq(arr, low, pi - 1);
        quickSortWordFreq(arr, pi + 1, high);
    }
}

// ============= Merge Sort for WordFreq Structs =============

void mergeWordFreq(struct WordFreq *arr, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = 0;
    
    struct WordFreq *temp = malloc((right - left + 1) * sizeof(struct WordFreq));
    
    while (i <= mid && j <= right) {
        if (arr[i].freq >= arr[j].freq) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    
    for (int i = left, k = 0; i <= right; i++, k++) {
        arr[i] = temp[k];
    }
    
    free(temp);
}

void mergeSortWordFreqHelper(struct WordFreq *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortWordFreqHelper(arr, left, mid);
        mergeSortWordFreqHelper(arr, mid + 1, right);
        mergeWordFreq(arr, left, mid, right);
    }
}

void mergeSortWordFreq(struct WordFreq *arr, int n) {
    if (n > 1) {
        mergeSortWordFreqHelper(arr, 0, n - 1);
    }
}

void displayToxicBarChart(void) {
    printf("\n");
    printf("%s====================================================%s\n", DIVIDER, RESET);
    printf("%s  TOXIC WORDS & PHRASES BAR CHART%s\n", MENU_LABEL, RESET);
    printf("%s====================================================%s\n", DIVIDER, RESET);
    int found = 0;

    // --- Display Single Toxic Words (sorted by frequency) ---
    printf("\n--- Single Words (by frequency) ---\n");
    
    // Create array for sorting
    struct WordFreq *wordFreqs = malloc(toxicCount * sizeof(struct WordFreq));
    int wordFreqCount = 0;
    
    for (int i = 0; i < toxicCount; i++) {
        if (toxicFreq[i] > 0) {
            strcpy(wordFreqs[wordFreqCount].word, toxicWords[i]);
            wordFreqs[wordFreqCount].freq = toxicFreq[i];
            wordFreqCount++;
            found = 1;
        }
    }
    
    // Sort by frequency (descending)
    qsort(wordFreqs, wordFreqCount, sizeof(struct WordFreq), compareByFreqDesc);
    
    // Display sorted words
    for (int i = 0; i < wordFreqCount; i++) {
        printf("%-15s | ", wordFreqs[i].word);
        for (int j = 0; j < wordFreqs[i].freq; j++) {
            printf("#");
        }
        printf(" (%d)\n", wordFreqs[i].freq);
    }
    
    free(wordFreqs);

    // --- Display Toxic Phrases (sorted by frequency) ---
    if (toxicPhrasesCount > 0) {
        printf("\n--- Multi-word Phrases (by frequency) ---\n");
        
        // Create array for sorting
        struct WordFreq *phraseFreqs = malloc(toxicPhrasesCount * sizeof(struct WordFreq));
        int phraseFreqCount = 0;
        
        for (int i = 0; i < toxicPhrasesCount; i++) {
            if (toxicPhrasesFreq[i] > 0) {
                strcpy(phraseFreqs[phraseFreqCount].word, toxicPhrases[i]);
                phraseFreqs[phraseFreqCount].freq = toxicPhrasesFreq[i];
                phraseFreqCount++;
                found = 1;
            }
        }
        
        // Sort by frequency (descending)
        qsort(phraseFreqs, phraseFreqCount, sizeof(struct WordFreq), compareByFreqDesc);
        
        // Display sorted phrases
        for (int i = 0; i < phraseFreqCount; i++) {
            printf("%-15s | ", phraseFreqs[i].word);
            for (int j = 0; j < phraseFreqs[i].freq; j++) {
                printf("#");
            }
            printf(" (%d)\n", phraseFreqs[i].freq);
        }
        
        free(phraseFreqs);
    }

    if (!found) {
        printf("No toxic words or phrases found in the last analysis.\n");
        printf("Please analyze a file first (Menu 2) or check your toxic words list.\n");
    }
    printf("\n=============================================================\n");
}

// ============= Frequency-Based Sorting Display =============

void displayWordsByFrequency(char (*sortedWords)[50], int wordCount, char sortChoice) {
    // Create array to hold word frequencies
    struct WordFreq *wordFreqs = malloc(wordCount * sizeof(struct WordFreq));
    if (!wordFreqs) {
        printf("\n**CRITICAL ERROR**: Memory allocation failed for frequencies\n");
        printf("Unable to allocate %lu bytes\n\n", (unsigned long)(wordCount * sizeof(struct WordFreq)));
        return;
    }

    // Count frequency of each word in the full words array
    int wfCount = 0;
    for (int i = 0; i < wordCount; i++) {
        int freq = 0;
        for (int j = 0; j < totalWords; j++) {
            if (stringEqualsIgnoreCase(sortedWords[i], words[j])) {
                freq++;
            }
        }
        strcpy(wordFreqs[wfCount].word, sortedWords[i]);
        wordFreqs[wfCount].freq = freq;
        wfCount++;
    }

    // Sort by frequency (descending) using selected algorithm
    printf("\n====================================================\n");
    clock_t start = clock();

    if (sortChoice == '1') {
        printf(" SORTING %d UNIQUE WORDS BY FREQUENCY (BUBBLE SORT)...\n", wfCount);
        printf("====================================================\n");
        bubbleSortWordFreq(wordFreqs, wfCount);
    } else if (sortChoice == '2') {
        printf(" SORTING %d UNIQUE WORDS BY FREQUENCY (QUICK SORT)...\n", wfCount);
        printf("====================================================\n");
        quickSortWordFreq(wordFreqs, 0, wfCount - 1);
    } else if (sortChoice == '3') {
        printf(" SORTING %d UNIQUE WORDS BY FREQUENCY (MERGE SORT)...\n", wfCount);
        printf("====================================================\n");
        mergeSortWordFreq(wordFreqs, wfCount);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("[OK] Sort complete! Time: %.3f ms\n\n", time_taken);

    printf("=============Sorted UNIQUE Words (By Frequency)==============\n");

    int displayLimit = (wfCount < 50) ? wfCount : 50;
    for (int i = 0; i < displayLimit; i++) {
        printf("%3d. %-20s Freq: %d\n", i + 1, wordFreqs[i].word, wordFreqs[i].freq);
    }

    if (wfCount > 50) {
        printf("\n... (%d more unique words not shown)\n", wfCount - 50);
    }

    printf("\n====================================================\n");
    printf(" Total Unique Words Sorted: %d\n", wfCount);
    printf("====================================================\n");

    free(wordFreqs);
}

// ============= Toxicity Count Sorting Display =============

void displayToxicWordsByCount(char sortChoice) {
    if (toxicCount == 0) {
        printf("\n  ** No Toxic Content **\n");
        printf("  The analyzed file contains no toxic words or phrases\n\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    // Create array for sorting toxic words by count
    struct WordFreq *toxicFreqs = malloc(toxicCount * sizeof(struct WordFreq));
    if (!toxicFreqs) {
        printf("\n**CRITICAL ERROR**: Memory allocation failed for toxic analysis\n");
        printf("Close other programs and try again\n\n");
        return;
    }

    int toxicFreqCount = 0;
    for (int i = 0; i < toxicCount; i++) {
        if (toxicFreq[i] > 0) {
            strcpy(toxicFreqs[toxicFreqCount].word, toxicWords[i]);
            toxicFreqs[toxicFreqCount].freq = toxicFreq[i];
            toxicFreqCount++;
        }
    }

    // Sort by toxicity count (descending) using selected algorithm
    printf("\n====================================================\n");
    clock_t start = clock();

    if (sortChoice == '1') {
        printf(" SORTING %d TOXIC WORDS BY COUNT (BUBBLE SORT)...\n", toxicFreqCount);
        printf("====================================================\n");
        bubbleSortWordFreq(toxicFreqs, toxicFreqCount);
    } else if (sortChoice == '2') {
        printf(" SORTING %d TOXIC WORDS BY COUNT (QUICK SORT)...\n", toxicFreqCount);
        printf("====================================================\n");
        quickSortWordFreq(toxicFreqs, 0, toxicFreqCount - 1);
    } else if (sortChoice == '3') {
        printf(" SORTING %d TOXIC WORDS BY COUNT (MERGE SORT)...\n", toxicFreqCount);
        printf("====================================================\n");
        mergeSortWordFreq(toxicFreqs, toxicFreqCount);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("[OK] Sort complete! Time: %.3f ms\n\n", time_taken);

    printf("=============Toxic Words (By Toxicity Count)==============\n");

    int displayLimit = (toxicFreqCount < 50) ? toxicFreqCount : 50;
    for (int i = 0; i < displayLimit; i++) {
        printf("%3d. %-20s Count: %d\n", i + 1, toxicFreqs[i].word, toxicFreqs[i].freq);
    }

    if (toxicFreqCount > 50) {
        printf("\n... (%d more toxic words not shown)\n", toxicFreqCount - 50);
    }

    printf("\n====================================================\n");
    printf(" Total Toxic Words Sorted: %d\n", toxicFreqCount);
    printf("====================================================\n");

    free(toxicFreqs);
}

// ============= Severity Breakdown Report =============

void displaySeverityBreakdown(void) {
    int severeMild = 0, severeMod = 0, severeSev = 0;
    int countMild = 0, countMod = 0, countSev = 0;
    
    printf("\n");
    printf("%s====================================================%s\n", DIVIDER, RESET);
    printf("%s  SEVERITY ANALYSIS REPORT%s\n", MENU_LABEL, RESET);
    printf("%s====================================================%s\n", DIVIDER, RESET);
    
    // Count by severity level
    for (int i = 0; i < toxicCount; i++) {
        if (toxicFreq[i] > 0) {
            if (toxicSeverity[i] == SEVERITY_MILD) {
                severeMild++;
                countMild += toxicFreq[i];
            } else if (toxicSeverity[i] == SEVERITY_MODERATE) {
                severeMod++;
                countMod += toxicFreq[i];
            } else if (toxicSeverity[i] == SEVERITY_SEVERE) {
                severeSev++;
                countSev += toxicFreq[i];
            }
        }
    }
    
    int totalDetected = countMild + countMod + countSev;
    
    printf("\n=== SEVERITY DISTRIBUTION ===\n\n");
    printf("  [SEVERE] Hate Speech & Slurs\n");
    printf("     %d occurrences | %d unique words\n\n", countSev, severeSev);
    printf("  [MODERATE] Insults & Strong Language\n");
    printf("     %d occurrences | %d unique words\n\n", countMod, severeMod);
    printf("  [MILD] Vulgar Language & Crude Terms\n");
    printf("     %d occurrences | %d unique words\n\n", countMild, severeMild);
    printf("  =============================\n");
    printf("  TOTAL: %d occurrences | %d unique toxic words\n", totalDetected, severeSev + severeMod + severeMild);
    
    // Show severity percentages with visual bars
    if (totalDetected > 0) {
        printf("\n=== SEVERITY BREAKDOWN (%%) ===\n\n");
        double severePct = (double)countSev / totalDetected * 100;
        double modPct = (double)countMod / totalDetected * 100;
        double mildPct = (double)countMild / totalDetected * 100;
        
        printf("  [SEVERE]    %5.1f%%  ", severePct);
        int sevBar = (int)(severePct / 5);
        printf("[");
        for (int i = 0; i < 10; i++) printf(i < sevBar ? "#" : "-");
        printf("]\n");
        
        printf("  [MODERATE]  %5.1f%%  ", modPct);
        int modBar = (int)(modPct / 5);
        printf("[");
        for (int i = 0; i < 10; i++) printf(i < modBar ? "#" : "-");
        printf("]\n");
        
        printf("  [MILD]      %5.1f%%  ", mildPct);
        int mildBar = (int)(mildPct / 5);
        printf("[");
        for (int i = 0; i < 10; i++) printf(i < mildBar ? "#" : "-");
        printf("]\n");
    }
    
    printf("\n====================================================\n");
}

