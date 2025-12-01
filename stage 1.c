#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#define BUFFER_SIZE 100000
#define MAX_WORDS 100000
#define MAX_UNIQUE 10000
#define MAX_STOPWORDS 2000
#define MAX_TOXIC 2000

// ---------------- Global Variables ----------------
char (*words)[50];
char (*uniqueWords)[50];
int wordCount = 0;
int uniqueCount = 0;

char stopwords[MAX_STOPWORDS][50];
int stopwordCount = 0;

char toxicWords[MAX_TOXIC][50];
int toxicCount = 0;
int toxicFreq[MAX_TOXIC] = { 0 };

int totalWords = 0;
int totalToxicWords = 0;

// ---------------- Utility Functions ----------------
void normalizeCase(char str[]) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

void removePunctuation(char str[]) {
    int j = 0;
    for (int i = 0; str[i]; i++) {
        if (isalnum((unsigned char)str[i]) || str[i] == ' ')
            str[j++] = str[i];
    }
    str[j] = '\0';
}

void removeNonASCII(char str[]) {
    int j = 0;
    for (int i = 0; str[i]; i++) {
        if ((unsigned char)str[i] <= 127)
            str[j++] = str[i];
    }
    str[j] = '\0';
}

int isUnique(char token[], char uniqueWords[][50], int uniqueCount) {
    for (int i = 0; i < uniqueCount; i++)
        if (strcmp(token, uniqueWords[i]) == 0)
            return 0;
    return 1;
}

int loadWordsFromFile(const char* filename, char words[][50]) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Cannot open %s\n", filename);
        return 0;
    }
    int count = 0;
    while (fscanf(f, "%s", words[count]) != EOF)
        count++;
    fclose(f);
    return count;
}

int isStopword(char* word, char stopwords[][50], int stopwordCount) {
    for (int i = 0; i < stopwordCount; i++)
        if (strcmp(word, stopwords[i]) == 0)
            return 1;
    return 0;
}

int stringEqualsIgnoreCase(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

int isToxic(char* word, char toxicWords[][50], int toxicCount) {
    for (int i = 0; i < toxicCount; i++)
        if (stringEqualsIgnoreCase(word, toxicWords[i]))
            return 1;
    return 0;
}

// ---------------- Word Processing ----------------
void storeUniqueWords(char* line, char words[][50], int* wordCount,
    char uniqueWords[][50], int* uniqueCount,
    char stopwords[][50], int stopwordCount) {
    char copy[BUFFER_SIZE];
    strcpy(copy, line);
    char* token = strtok(copy, " \t\n,.:;!?\"'");
    while (token != NULL) {
        normalizeCase(token);
        removePunctuation(token);
        removeNonASCII(token);
        if (strlen(token) > 0 && !isStopword(token, stopwords, stopwordCount)) {
            if (isUnique(token, uniqueWords, *uniqueCount)) {
                strcpy(uniqueWords[*uniqueCount], token);
                (*uniqueCount)++;
            }
            strcpy(words[*wordCount], token);
            (*wordCount)++;
        }
        token = strtok(NULL, " \t\n,.:;!?\"'");
    }
}

double calculateAverageWordLength(char words[][50], int wordCount) {
    int totalLetters = 0;
    for (int i = 0; i < wordCount; i++)
        totalLetters += strlen(words[i]);
    return wordCount ? (double)totalLetters / wordCount : 0.0;
}

// ---------------- Sorting ----------------
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

void swapWords(char a[50], char b[50]) {
    char temp[50];
    strcpy(temp, a);
    strcpy(a, b);
    strcpy(b, temp);
}

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

// ---------------- COMBINED Sorting Menu ----------------
void displaySortedWordsMenu() {
    if (uniqueCount == 0) {
        printf("ERROR: No unique words found.\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    char sortChoice;
    printf("\n====================================================\n");
    printf("             SELECT SORTING ALGORITHM\n");
    printf("====================================================\n");
    printf("1. Bubble Sort\n");
    printf("2. Quick Sort\n");
    printf("Enter your choice (1 or 2): ");
    scanf(" %c", &sortChoice);

    if (sortChoice != '1' && sortChoice != '2') {
        printf("Invalid choice. Returning to menu.\n");
        return;
    }

    // Allocate memory for unique word sorting
    char (*sortedWords)[50] = malloc(uniqueCount * 50);
    if (!sortedWords) {
        printf("ERROR: Memory allocation failed!\n");
        return;
    }

    // Copy UNIQUE words only
    for (int i = 0; i < uniqueCount; i++) {
        strcpy(sortedWords[i], uniqueWords[i]);
    }

    // Sort the unique words
    printf("\n====================================================\n");
    clock_t start = clock();

    if (sortChoice == '1') {
        printf(" SORTING %d UNIQUE WORDS (BUBBLE SORT)...\n", uniqueCount);
        printf("====================================================\n");
        bubbleSortWords(sortedWords, uniqueCount);

    }
    else {
        printf(" SORTING %d UNIQUE WORDS (QUICK SORT)...\n", uniqueCount);
        printf("====================================================\n");
        quickSortWords(sortedWords, 0, uniqueCount - 1);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    printf("Sort complete! Time: %.3f ms\n\n", time_taken);
           
    printf("=============Sorted UNIQUE Words (A-Z)==============\n");

    int displayLimit = (uniqueCount < 50) ? uniqueCount : 50;
    for (int i = 0; i < displayLimit; i++) {
        printf("%3d. %s\n", i + 1, sortedWords[i]);
    }

    if (uniqueCount > 50) {
        printf("\n... (%d more unique words not shown)\n", uniqueCount - 50);
    }

    printf("\n====================================================\n");
    printf(" Total Unique Words Sorted: %d\n", uniqueCount);
    printf("====================================================\n");

    free(sortedWords);
}


// ---------------- Compare Sorting Algorithms (FIXED TO USE UNIQUE WORDS) ----------------
void compareSortingAlgorithms() {
    if (uniqueCount == 0) {
        printf("ERROR: No unique words to sort.\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    printf("\n====================================================\n");
    printf("          COMPARING SORTING ALGORITHMS\n");
    printf("====================================================\n");
    printf("Dataset size: %d UNIQUE words\n", uniqueCount);

    // ✅ FIX: Allocate based on uniqueCount, not wordCount
    char(*bubbleSorted)[50] = (char(*)[50])malloc(uniqueCount * 50 * sizeof(char));
    char(*quickSorted)[50] = (char(*)[50])malloc(uniqueCount * 50 * sizeof(char));

    if (!bubbleSorted || !quickSorted) {
        printf("ERROR: Memory allocation failed!\n");
        if (bubbleSorted) free(bubbleSorted);
        if (quickSorted) free(quickSorted);
        return;
    }

    // ✅ FIX: Copy UNIQUE words only
    for (int i = 0; i < uniqueCount; i++) {
        strcpy(bubbleSorted[i], uniqueWords[i]);
    }

    printf("\nRunning Bubble Sort on %d unique words...\n", uniqueCount);
    clock_t bubbleStart = clock();
    bubbleSortWords(bubbleSorted, uniqueCount);
    clock_t bubbleEnd = clock();
    double bubbleTime = ((double)(bubbleEnd - bubbleStart)) / CLOCKS_PER_SEC * 1000;
    printf("Bubble Sort complete! Time: %.3f ms\n", bubbleTime);

    // Test Quick Sort
    for (int i = 0; i < uniqueCount; i++) {
        strcpy(quickSorted[i], uniqueWords[i]);
    }

    printf("\nRunning Quick Sort on %d unique words...\n", uniqueCount);
    clock_t quickStart = clock();
    quickSortWords(quickSorted, 0, uniqueCount - 1);
    clock_t quickEnd = clock();
    double quickTime = ((double)(quickEnd - quickStart)) / CLOCKS_PER_SEC * 1000;
    printf("Quick Sort complete! Time: %.3f ms\n", quickTime);

    // Verify both produce same result
    int match = 1;
    for (int i = 0; i < uniqueCount; i++) {
        if (strcmp(bubbleSorted[i], quickSorted[i]) != 0) {
            match = 0;
            break;
        }
    }

    printf("\n====================================================\n");
    printf("                COMPARISON RESULTS\n");
    printf("====================================================\n");

    // Correctness check
    if (match) {
        printf("Both algorithms produced IDENTICAL results!\n");
    }
    else {
        printf("WARNING: Results differ!\n");
    }

    // Performance comparison
    printf("\nPerformance:\n");
    printf("  Bubble Sort: %.3f ms\n", bubbleTime);
    printf("  Quick Sort:  %.3f ms\n", quickTime);

    if (bubbleTime > quickTime) {
        double speedup = bubbleTime / quickTime;
        printf("\nQuick Sort is %.2fx FASTER\n", speedup);
    }
    else if (quickTime > bubbleTime) {
        double speedup = quickTime / bubbleTime;
        printf("Bubble Sort is %.2fx FASTER\n", speedup);
    }
    else {
        printf("Both took approximately the same time\n");
    }

    printf("\nAlgorithm Characteristics:\n");
    printf("Bubble Sort: O(n^2) - Simple but slow for large data\n");
    printf("Quick Sort:  O(n log n) - Fast and efficient\n");

    printf("====================================================\n");

    // Show first 10 from both
    printf("\nFirst 10 sorted unique words:\n");
    for (int i = 0; i < 10 && i < uniqueCount; i++) {
        printf("%2d. %s\n", i + 1, bubbleSorted[i]);
    }

    // ✅ Free allocated memory
    free(bubbleSorted);
    free(quickSorted);
}

// ---------------- Toxic Words Menu ----------------
void addToxicWordsMenu() {
    char choice, newWord[50];
    printf("Do you want to add new toxic words? (y/n): ");
    scanf(" %c", &choice);

    while (choice == 'y' || choice == 'Y') {
        printf("Enter new toxic word: ");
        scanf("%s", newWord);

        int exists = 0;
        for (int i = 0; i < toxicCount; i++)
            if (stringEqualsIgnoreCase(newWord, toxicWords[i])) {
                exists = 1;
                break;
            }

        if (!exists) {
            strcpy(toxicWords[toxicCount++], newWord);
            FILE* f = fopen("toxicwords.txt", "a");
            if (f) {
                fprintf(f, "%s\n", newWord);
                fclose(f);
            }
            printf("'%s' added.\n", newWord);
        }
        else {
            printf("'%s' already exists.\n", newWord);
        }

        printf("Add another? (y/n): ");
        scanf(" %c", &choice);
    }
}

void reloadDictionaries() {
    stopwordCount = loadWordsFromFile("stopwords.txt", stopwords);
    toxicCount = loadWordsFromFile("toxicwords.txt", toxicWords);

    // Reset toxic frequencies
    for (int i = 0; i < MAX_TOXIC; i++)
        toxicFreq[i] = 0;

    printf("\nDictionaries reloaded successfully!\n");
    printf("Stopwords loaded: %d\n", stopwordCount);
    printf("Toxic words loaded: %d\n", toxicCount);

    if (stopwordCount == 0)
        printf("WARNING: stopwords.txt is empty or missing.\n");
    if (toxicCount == 0)
        printf("WARNING: toxicwords.txt is empty or missing.\n");
}

// ---------------- File Analysis ----------------
void readAndAnalyzeFileMenu() {
    char fileName[260];
    printf("Enter filename to read: ");
    scanf("%s", fileName);

    FILE* file = fopen(fileName, "r");
    if (!file) {
        printf("\nERROR: Unable to open file: '%s'\n", fileName);
        printf("Possible reasons:\n");
        printf(" - The file does not exist\n");
        printf(" - The file name is typed incorrectly\n");
        printf(" - The file is not in the same folder as the program\n");
        printf("Please try again.\n\n");
        return;
    }

    // Reset counters
    wordCount = uniqueCount = totalWords = totalToxicWords = 0;
    for (int i = 0; i < MAX_TOXIC; i++)
        toxicFreq[i] = 0;

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        storeUniqueWords(line, words, &wordCount, uniqueWords, &uniqueCount,
            stopwords, stopwordCount);
    }

    totalWords = wordCount;

    // Count toxic words
    for (int w = 0; w < wordCount; w++)
        for (int t = 0; t < toxicCount; t++)
            if (stringEqualsIgnoreCase(words[w], toxicWords[t])) {
                toxicFreq[t]++;
                totalToxicWords++;
            }
              
    printf("\n========File Analysis Results for (%s)=======\n", fileName);
    printf("\nTotal words: %d\n", totalWords);
    printf("Unique words: %d\n", uniqueCount);
    printf("Average word length: %.2f\n", calculateAverageWordLength(words, wordCount));

    double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    double nonToxicRatio = totalWords ? (double)(totalWords - totalToxicWords) / totalWords * 100 : 0.0;

    printf("Toxic ratio: %.2f%%\n", toxicRatio);
    printf("Non-toxic ratio: %.2f%%\n", nonToxicRatio);

    fclose(file);
}

void saveReportMenu() {
    FILE* report = fopen("analysis_report.txt", "w");
    if (!report) {
        printf("Cannot open analysis_report.txt\n");
        return;
    }

    if (totalWords == 0) {
        printf("ERROR: No analysis data found.\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    fprintf(report, "Total words: %d\n", totalWords);
    fprintf(report, "Unique words: %d\n", uniqueCount);
    fprintf(report, "Average word length: %.2f\n", calculateAverageWordLength(words, wordCount));

    double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    double nonToxicRatio = totalWords ? (double)(totalWords - totalToxicWords) / totalWords * 100 : 0.0;

    fprintf(report, "Toxic ratio: %.2f%%\n", toxicRatio);
    fprintf(report, "Non-toxic ratio: %.2f%%\n", nonToxicRatio);

    fclose(report);
    printf("Analysis saved to analysis_report.txt\n");
}

void saveCSVReport() {
    FILE* csv = fopen("analysis_report.csv", "w");
    if (!csv) {
        printf("Cannot open analysis_report.csv\n");
        return;
    }

    if (totalWords == 0) {
        printf("ERROR: No data available to save.\n");
        return;
    }

    // Header row
    fprintf(csv, "Metric,Value\n");

    // Data rows
    fprintf(csv, "Total Words,%d\n", totalWords);
    fprintf(csv, "Unique Words,%d\n", uniqueCount);
    fprintf(csv, "Average Word Length,%.2f\n", calculateAverageWordLength(words, wordCount));

    double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    double nonToxicRatio = totalWords ? 100.0 - toxicRatio : 0.0;

    fprintf(csv, "Toxic Ratio (%%),%.2f\n", toxicRatio);
    fprintf(csv, "Non-Toxic Ratio (%%),%.2f\n", nonToxicRatio);

    fclose(csv);
    printf("CSV report saved as analysis_report.csv\n");
}

void displayToxicBarChart() {
             
    printf("\n===============Toxic Words Bar Chart================\n");
    int found = 0;

    for (int i = 0; i < toxicCount; i++) {
        if (toxicFreq[i] > 0) {
            found = 1;
            printf("%-10s | ", toxicWords[i]);
            for (int j = 0; j < toxicFreq[i]; j++) {
                printf("#");
            }
            printf(" (%d)\n", toxicFreq[i]);
        }
    }

    if (!found) {
        printf("ERROR: No toxic words found in the last analysis.\n");
        printf("Please analyze a file first (Menu 2) or check your toxic words list.\n");
    }
}

// ---------------- Main Program ----------------
int main() {
    // ✅ Allocate large arrays dynamically at program start
    words = (char(*)[50])malloc(MAX_WORDS * 50 * sizeof(char));
    uniqueWords = (char(*)[50])malloc(MAX_UNIQUE * 50 * sizeof(char));

    if (!words || !uniqueWords) {
        printf("FATAL ERROR: Cannot allocate memory for program!\n");
        printf("Please reduce MAX_WORDS or MAX_UNIQUE constants.\n");
        return 1;
    }

    stopwordCount = loadWordsFromFile("stopwords.txt", stopwords);
    toxicCount = loadWordsFromFile("toxicwords.txt", toxicWords);

    char choice;

    while (1) {
        printf("\n");
        printf("****************************************************\n");
        printf("*                 TOXIC WORDS MENU                 *\n");
        printf("****************************************************\n");
        printf("* 1. Add toxic words                               *\n");
        printf("* 2. Read and analyze file                         *\n");
        printf("* 3. Save analysis report                          *\n");
        printf("* 4. Reload dictionaries (stopwords + toxic words) *\n");
        printf("* 5. Save CSV report                               *\n");
        printf("* 6. Display toxic words bar chart                 *\n");
        printf("* 7. Sort words (Choose algorithm)                 *\n");
        printf("* 8. Compare sorting algorithms                    *\n");
        printf("* 0. Exit                                          *\n");
        printf("****************************************************\n");
        printf("Enter your choice: ");
    

        scanf(" %c", &choice);

        switch (choice) {
        case '1':
            addToxicWordsMenu();
            break;
        case '2':
            readAndAnalyzeFileMenu();
            break;
        case '3':
            saveReportMenu();
            break;
        case '4':
            reloadDictionaries();
            break;
        case '5':
            saveCSVReport();
            break;
        case '6':
            displayToxicBarChart();
            break;
        case '7':
            displaySortedWordsMenu();
            break;
        case '8':
            compareSortingAlgorithms();
            break;
        case '0':
            printf("Exiting program. Goodbye!\n");
            // ✅ Free allocated memory before exiting
            free(words);
            free(uniqueWords);
            return 0;
        default:
            printf("Invalid choice. Try again.\n");
            break;
        }
    }
}
