#define _CRT_SECURE_NO_WARNINGS
#include "data_types.h"
#include "colors.h"

/*
================================================================================
 MAIN.C - TOXIC WORDS ANALYSIS SYSTEM
 
 Purpose:
  - Main entry point for the toxic words detection program
  - Handles menu-driven user interface
  - Manages file loading and analysis operations
  - Implements comprehensive input validation
 
 Key Functions:
  - readAndAnalyzeFileMenu(): Loads and analyzes text/CSV files
  - displaySortedWordsMenu(): Displays sorted words by various criteria
  - compareSortingAlgorithms(): Compares performance of sorting algorithms
  - main(): Program entry point with main menu loop
================================================================================
*/

// ============= Global Variables Definition =============

// Word storage
char (*words)[50];              // All words extracted from file
char (*uniqueWords)[50];        // Unique words only
int wordCount = 0;              // Total words count
int uniqueCount = 0;            // Unique words count

// Stopwords dictionary (common words to exclude)
char stopwords[MAX_STOPWORDS][50];
int stopwordCount = 0;

// Toxic words dictionary (loaded from toxic_words/ folder)
char toxicWords[MAX_TOXIC][50];
int toxicCount = 0;             // Number of toxic words loaded
int toxicFreq[MAX_TOXIC] = { 0 };     // Frequency counters
int toxicSeverity[MAX_TOXIC] = { 0 }; // Severity levels (1-3)
struct ToxicTerm toxicTerms[MAX_TOXIC];

// Analysis statistics
int totalWords = 0;
int totalToxicWords = 0;
int totalSentences = 0;

// Toxic phrases (multi-word terms)
char toxicPhrases[MAX_TOXIC_PHRASES][100];
int toxicPhrasesCount = 0;
int toxicPhrasesFreq[MAX_TOXIC_PHRASES] = { 0 };

// Comparative analysis storage
struct AnalysisResult fileA_Results = {0};
struct AnalysisResult fileB_Results = {0};

// ============= File Analysis Menu =============

/*
 Function: readAndAnalyzeFileMenu()
 
 Purpose: Load and analyze a text or CSV file for toxic content
 
 Features:
  - Loads files from current directory or ./analysis/ folder
  - Auto-detects file type (TXT or CSV)
  - For CSV files: displays headers and allows column selection
  - Extracts and processes words
  - Detects both single toxic words and multi-word phrases
  - Calculates statistics (word count, unique words, sentences, toxicity ratio)
  - Shows comprehensive analysis report
 
 Returns: void
 Side Effects: Modifies global word arrays and counters
*/

void readAndAnalyzeFileMenu(void) {
    char fileName[260];
    char fullPath[260];
    char columnInput[100];
    
    printf("Enter filename to read: ");
    if (fgets(fileName, sizeof(fileName), stdin) == NULL) {
        printf("Error reading filename.\n");
        return;
    }
    // Remove trailing newline and any whitespace
    fileName[strcspn(fileName, "\n\r")] = 0;
    
    // Also trim any leading/trailing spaces
    int len = strlen(fileName);
    while (len > 0 && isspace((unsigned char)fileName[len-1])) {
        fileName[--len] = 0;
    }

    // Try to open file from provided path first
    FILE* file = fopen(fileName, "r");
    
    if (!file) {
        // If filename doesn't already contain "analysis", try adding it
        if (strstr(fileName, "analysis") == NULL) {
            snprintf(fullPath, sizeof(fullPath), "analysis/%s", fileName);
            file = fopen(fullPath, "r");
            
            if (!file) {
                // Try with backslash
                snprintf(fullPath, sizeof(fullPath), "analysis\\%s", fileName);
                file = fopen(fullPath, "r");
            }
        } else {
            // Path already contains "analysis", use it as-is in fullPath
            snprintf(fullPath, sizeof(fullPath), "%s", fileName);
        }
    } else {
        // File was found, use original path
        snprintf(fullPath, sizeof(fullPath), "%s", fileName);
    }
    
    if (!file) {
        printf("\n");
        printf("====== FILE ERROR ======\n");
        printf("  Could not find file: '%s'\n\n", fileName);
        printf("Checked locations:\n");
        printf("  1. Current directory\n");
        printf("  2. ./analysis/ folder\n");
        printf("  3. ./analysis\\ folder (Windows)\n\n");
        printf("Solutions:\n");
        printf("  - Check that the filename is spelled correctly\n");
        printf("  - Ensure the file is in one of the checked locations\n");
        printf("  - Use option 1 to load a file first\n\n");
        return;
    }

    // Check if file is CSV
    int isCSV = 0;
    const char* ext = strrchr(fileName, '.');
    if (ext && stringEqualsIgnoreCase(ext, ".csv")) {
        isCSV = 1;
    }

    // Reset counters
    wordCount = uniqueCount = totalWords = totalToxicWords = totalSentences = 0;
    for (int i = 0; i < MAX_TOXIC; i++)
        toxicFreq[i] = 0;
    for (int i = 0; i < MAX_TOXIC_PHRASES; i++)
        toxicPhrasesFreq[i] = 0;

    int selectedColumns[20];
    int selectedColCount = 0;
    
    // If CSV, ask user which columns to analyze
    if (isCSV) {
        displayCSVHeaders(fullPath);
        int colCount = getCSVColumnCount(fullPath);
        
        printf("\nEnter column numbers to analyze (comma-separated, e.g. '1,2,3' or '1'): ");
        if (fgets(columnInput, sizeof(columnInput), stdin) == NULL) {
            printf("Error reading input.\n");
            fclose(file);
            return;
        }
        // Remove trailing newline
        columnInput[strcspn(columnInput, "\n")] = 0;
        
        // Parse column selections
        char inputCopy[100];
        strcpy(inputCopy, columnInput);
        char* token = strtok(inputCopy, ",");
        while (token != NULL && selectedColCount < 20) {
            int col = atoi(token) - 1; // Convert to 0-indexed
            if (col >= 0 && col < colCount) {
                selectedColumns[selectedColCount++] = col;
            }
            token = strtok(NULL, ",");
        }
        
        if (selectedColCount == 0) {
            printf("No valid columns selected.\n");
            fclose(file);
            return;
        }
        
        printf("Analyzing %d column(s)...\n\n", selectedColCount);
        
        // Reopen file to skip header
        fclose(file);
        file = fopen(fullPath, "r");
        if (!file) return;
        
        // Skip header line
        char headerLine[4096];
        fgets(headerLine, sizeof(headerLine), file);
    }

    // Process file rows
    int rowCount = 0;
    printf("Starting to process rows...\n");
    fflush(stdout);
    
    while (1) {
        char line[4096];
        if (!fgets(line, sizeof(line), file)) {
            printf("EOF reached.\n");
            fflush(stdout);
            break;
        }
        
        rowCount++;
        if (rowCount % 5000 == 0) {
            printf("Processed %d rows, %d words extracted\n", rowCount, wordCount);
            fflush(stdout);
        }
        
        char processLine[BUFFER_SIZE] = {0};
        
        if (isCSV) {
            char fields[50][256];
            int fieldCount = parseCSVLine(line, fields, 50);
            
            // Concatenate selected columns with space
            for (int i = 0; i < selectedColCount; i++) {
                int col = selectedColumns[i];
                if (col < fieldCount) {
                    // Trim whitespace
                    int start = 0;
                    while (fields[col][start] == ' ') start++;
                    
                    if (i > 0) strcat(processLine, " ");
                    strcat(processLine, &fields[col][start]);
                }
            }
        } else {
            strcpy(processLine, line);
        }
        
        if (strlen(processLine) > 0 && wordCount < MAX_WORDS) {
            // For CSV, use faster processing without unique word tracking
            if (isCSV) {
                char copy[BUFFER_SIZE];
                strcpy(copy, processLine);
                
                // Count sentences
                for (int i = 0; processLine[i]; i++) {
                    if (processLine[i] == '.' || processLine[i] == '!' || processLine[i] == '?')
                        totalSentences++;
                }
                
                // Extract and tokenize words directly for faster processing
                char* token = strtok(copy, " \t\n,.:;!?\"'");
                while (token != NULL && wordCount < MAX_WORDS) {
                    normalizeCase(token);
                    removePunctuation(token);
                    removeNonASCII(token);
                    // Filter: only keep words with 2+ characters and not stopwords
                    if (strlen(token) > 1 && !isStopword(token, stopwords, stopwordCount)) {
                        strcpy(words[wordCount++], token);
                    }
                    token = strtok(NULL, " \t\n,.:;!?\"'");
                }
            } else {
                storeUniqueWords(processLine, words, &wordCount, uniqueWords, &uniqueCount,
                    stopwords, stopwordCount);
            }
        }
    }
    
    printf("Finished processing: %d rows | %d words extracted\n\n", rowCount, wordCount);
    fflush(stdout);

    totalWords = wordCount;
    
    // Check if file is empty (no valid words extracted)
    if (wordCount == 0) {
        printf("\n*** WARNING: EMPTY FILE DETECTED ***\n");
        printf("No valid words could be extracted from the file.\n");
        printf("Possible reasons:\n");
        printf("  - File contains only whitespace or stopwords\n");
        printf("  - File is corrupted or in an unsupported format\n");
        printf("  - All words are single characters (filtered out)\n");
        printf("Please try analyzing a different file.\n\n");
        fclose(file);
        return;
    }
    
    // For CSV files, build unique words list from word frequencies
    if (isCSV && uniqueCount == 0) {
        printf("Building unique words list...\n");
        fflush(stdout);
        
        // For very large files (100k words), use hash-based approach for speed
        if (wordCount > 50000) {
            printf("Extracting unique words from %d total words...\n", wordCount);
            fflush(stdout);
            
            // Build unique words with early exit at 10000 unique words found
            // This is still O(n*m) but with early termination at 10000 unique
            for (int i = 0; i < wordCount && uniqueCount < 10000; i++) {
                if (i > 0 && i % 10000 == 0) {
                    printf("  Processed %d words, found %d unique so far...\n", i, uniqueCount);
                    fflush(stdout);
                }
                
                int found = 0;
                for (int j = 0; j < uniqueCount; j++) {
                    if (strcmp(words[i], uniqueWords[j]) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    strcpy(uniqueWords[uniqueCount++], words[i]);
                }
            }
        } else {
            // Standard O(n^2) unique checking for smaller files
            for (int i = 0; i < wordCount && uniqueCount < MAX_WORDS; i++) {
                int found = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(words[i], words[j]) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    strcpy(uniqueWords[uniqueCount++], words[i]);
                }
            }
        }
        printf("Unique words built: %d\n", uniqueCount);
        fflush(stdout);
    }

    // Count toxic words and phrases
    printf("Analyzing toxic words...\n");
    fflush(stdout);
    
    // Process toxic word detection with progress reporting
    for (int w = 0; w < wordCount; w++) {
        // Progress reporting every 10000 words
        if (w > 0 && w % 10000 == 0) {
            printf("Analyzed %d/%d words for toxicity...\n", w, wordCount);
            fflush(stdout);
        }
        
        // Check for toxic bigrams/trigrams first
        int phraseFound = 0;
        
        // Try to match multi-word phrases (up to 3 words)
        if (w + 1 < wordCount && toxicPhrasesCount > 0) {
            // Build normalized bigram for comparison
            char bigram[150];
            sprintf(bigram, "%s %s", words[w], words[w+1]);
            normalizeCase(bigram);
            
            for (int p = 0; p < toxicPhrasesCount; p++) {
                if (p >= MAX_TOXIC_PHRASES) break; // Safety check
                
                // Create a normalized copy of the phrase for comparison
                char normalizedPhrase[150];
                strcpy(normalizedPhrase, toxicPhrases[p]);
                normalizeCase(normalizedPhrase);
                
                if (strcmp(bigram, normalizedPhrase) == 0) {
                    if (p < MAX_TOXIC_PHRASES) {
                        toxicPhrasesFreq[p]++;
                    }
                    totalToxicWords++;
                    phraseFound = 1;
                    w++; // Skip next word to avoid double counting
                    break;
                }
            }
        }
        
        // If no bigram found, check for trigrams (3 words)
        if (!phraseFound && w + 2 < wordCount && toxicPhrasesCount > 0) {
            char trigram[150];
            sprintf(trigram, "%s %s %s", words[w], words[w+1], words[w+2]);
            normalizeCase(trigram);
            
            for (int p = 0; p < toxicPhrasesCount; p++) {
                if (p >= MAX_TOXIC_PHRASES) break; // Safety check
                
                // Create a normalized copy of the phrase for comparison
                char normalizedPhrase[150];
                strcpy(normalizedPhrase, toxicPhrases[p]);
                normalizeCase(normalizedPhrase);
                
                if (strcmp(trigram, normalizedPhrase) == 0) {
                    if (p < MAX_TOXIC_PHRASES) {
                        toxicPhrasesFreq[p]++;
                    }
                    totalToxicWords++;
                    phraseFound = 1;
                    w += 2; // Skip next two words to avoid double counting
                    break;
                }
            }
        }
        
        // If no phrase found, check for single toxic words
        if (!phraseFound && isToxic(words[w], toxicWords, toxicCount)) {
            for (int t = 0; t < toxicCount; t++) {
                if (t >= MAX_TOXIC) break; // Safety check
                
                if (stringEqualsIgnoreCase(words[w], toxicWords[t])) {
                    toxicFreq[t]++;
                    totalToxicWords++;
                    break;
                }
            }
        }
    }
    
    printf("Toxic analysis complete! Found %d toxic occurrences.\n", totalToxicWords);
    fflush(stdout);
              
    printf("\n%s====================================================%s\n", DIVIDER, RESET);
    printf("%s  ANALYSIS RESULTS: %s\n", MENU_LABEL, fileName);
    printf("%s====================================================%s\n", DIVIDER, RESET);
    
    printf("\n%s>> Text Statistics%s\n", MENU_LABEL, RESET);
    printf("   %sTotal words:%s         %s%d%s\n", STAT_LABEL, RESET, STAT_VALUE, totalWords, RESET);
    printf("   %sUnique words:%s        %s%d%s\n", STAT_LABEL, RESET, STAT_VALUE, uniqueCount, RESET);
    printf("   %sTotal sentences:%s     %s%d%s\n", STAT_LABEL, RESET, STAT_VALUE, totalSentences, RESET);
    printf("   %sAverage word length:%s %s%.2f%s\n", STAT_LABEL, RESET, STAT_VALUE, calculateAverageWordLength(words, wordCount), RESET);
    printf("   %sAvg sentence length:%s %s%.2f%s words\n", STAT_LABEL, RESET, STAT_VALUE, calculateAverageSentenceLength(totalWords, totalSentences), RESET);
    printf("   %sLexical diversity:%s   %s%.4f%s\n", STAT_LABEL, RESET, STAT_VALUE, calculateLexicalDiversity(uniqueCount, totalWords), RESET);

    double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    double nonToxicRatio = totalWords ? (double)(totalWords - totalToxicWords) / totalWords * 100 : 0.0;

    printf("\n%s>> Toxicity Analysis%s\n", ERROR, RESET);
    printf("   %sToxic occurrences:%s   %s%d%s\n", STAT_LABEL, RESET, STAT_VALUE, totalToxicWords, RESET);
    printf("   %sToxic ratio:%s         %s%.2f%%%s\n", STAT_LABEL, RESET, BRIGHT_GREEN, toxicRatio, RESET);
    printf("   %sNon-toxic ratio:%s     %s%.2f%%%s\n", STAT_LABEL, RESET, BRIGHT_GREEN, nonToxicRatio, RESET);
    printf("%s====================================================%s\n\n", DIVIDER, RESET);

    fclose(file);
}

// ============= Sorting Menu =============

/*
 Function: displaySortedWordsMenu()
 
 Purpose: Display words sorted by user's chosen criterion and algorithm
 
 Criteria Supported:
  1. Alphabetical order (A-Z)
  2. Frequency-based (Most to Least frequent)
  3. Toxicity count (Most toxic words first)
 
 Sorting Algorithms:
  1. Bubble Sort - O(nÂ²), simple but slower
  2. Quick Sort - O(n log n), efficient
  3. Merge Sort - O(n log n), stable
 
 Returns: void
*/

void displaySortedWordsMenu(void) {
    if (uniqueCount == 0) {
        printf("\n** No data available **\n");
        printf("Please load and analyze a file first (Option 1)\n\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    char criterionChoice;
    printf("\n%s====================================================%s\n", DIVIDER, RESET);
    printf("%s  SORTING OPTIONS                               %s\n", MENU_LABEL, DIVIDER);
    printf("%s====================================================%s\n", DIVIDER, RESET);
    printf("\n%sSelect sorting criterion:%s\n", MENU_LABEL, RESET);
    printf("   %s[1]%s Alphabetical (A-Z)\n", OPTION_NUM, RESET);
    printf("   %s[2]%s Frequency-based (Most to Least)\n", OPTION_NUM, RESET);
    printf("   %s[3]%s Toxicity count (Most to Least)\n", OPTION_NUM, RESET);
    printf("\n%s> Enter choice (1-3):%s ", PROMPT, RESET);
    criterionChoice = getValidMenuChoice("123");
    
    if (criterionChoice == '\0') {
        printf("%sInvalid choice. Returning to menu.%s\n", ERROR, RESET);
        return;
    }

    char sortChoice;
    printf("\n%sSelect sorting algorithm:%s\n", MENU_LABEL, RESET);
    printf("   %s[1]%s Bubble Sort      O(n^2) - Simple\n", OPTION_NUM, RESET);
    printf("   %s[2]%s Quick Sort       O(n log n) - Efficient\n", OPTION_NUM, RESET);
    printf("   %s[3]%s Merge Sort       O(n log n) - Stable\n", OPTION_NUM, RESET);
    printf("\n%s> Enter choice (1-3):%s ", PROMPT, RESET);
    sortChoice = getValidMenuChoice("123");
    
    if (sortChoice == '\0') {
        printf("Invalid choice. Returning to menu.\n");
        return;
    }

    // Allocate memory for unique word sorting
    char (*sortedWords)[50] = malloc(uniqueCount * 50);
    if (!sortedWords) {
        printf("\n**CRITICAL ERROR**: Could not allocate memory for sorting\\n");
        printf("Close other programs and try again.\\n\\n");
        return;
    }

    // Copy UNIQUE words only
    for (int i = 0; i < uniqueCount; i++) {
        strcpy(sortedWords[i], uniqueWords[i]);
    }

    if (criterionChoice == '1') {
        // Alphabetical sorting
        // Sort the unique words
        printf("\n%s====================================================%s\n", DIVIDER, RESET);
        clock_t start = clock();

        if (sortChoice == '1') {
            printf("%s  BUBBLE SORT - %d words                        %s\n", MENU_LABEL, uniqueCount, DIVIDER);
            bubbleSortWords(sortedWords, uniqueCount);
        }
        else if (sortChoice == '2') {
            printf("%s  QUICK SORT - %d words                         %s\n", MENU_LABEL, uniqueCount, DIVIDER);
            quickSortWords(sortedWords, 0, uniqueCount - 1);
        }
        else if (sortChoice == '3') {
            printf("%s  MERGE SORT - %d words                         %s\n", MENU_LABEL, uniqueCount, DIVIDER);
            mergeSortWords(sortedWords, uniqueCount);
        }

        clock_t end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

        printf("%s====================================================%s\n", DIVIDER, RESET);
        printf("%s[DONE]%s Time: %s%.3f ms%s\n\n", SUCCESS, RESET, BRIGHT_BLUE, time_taken, RESET);
               
        printf("%s SORTED RESULTS (First 50 of %d)%s\n", MENU_LABEL, uniqueCount, RESET);
        printf("%s===================================================%s\n", DIVIDER, RESET);

        int displayLimit = (uniqueCount < 50) ? uniqueCount : 50;
        for (int i = 0; i < displayLimit; i++) {
            printf("%s%3d.%s %-40s\n", OPTION_NUM, i + 1, RESET, sortedWords[i]);
        }

        if (uniqueCount > 50) {
            printf("%s\n... and %d more words%s\n", INFO, uniqueCount - 50, RESET);
        }

        printf("%s===================================================%s\n", DIVIDER, RESET);
        printf("%sTOTAL: %d unique words sorted%s\n\n", SUCCESS, uniqueCount, RESET);
    }
    else if (criterionChoice == '2') {
        // Frequency-based sorting
        displayWordsByFrequency(sortedWords, uniqueCount, sortChoice);
    }
    else if (criterionChoice == '3') {
        // Toxicity count sorting
        free(sortedWords);
        displayToxicWordsByCount(sortChoice);
        return;
    }

    free(sortedWords);
}

// ============= Algorithm Comparison =============

/*
 Function: compareSortingAlgorithms()
 
 Purpose: Compare performance and correctness of three sorting algorithms
 
 Operations:
  - Runs Bubble Sort, Quick Sort, and Merge Sort on same dataset
  - Measures execution time for each algorithm
  - Verifies all produce identical results
  - Displays performance metrics and complexity analysis
 
 Returns: void
 Time Complexity: 
  - Overall: O(3 * nÂ²) due to Bubble Sort (worst case)
  - But typically O(3 * n log n) with Quick/Merge sort
*/

void compareSortingAlgorithms(void) {
    if (uniqueCount == 0) {
        printf("\n** No words to sort **\n");
        printf("Please load and analyze a file first (Option 1)\n\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    printf("\n====================================================\n");
    printf("          COMPARING SORTING ALGORITHMS\n");
    printf("====================================================\n");
    printf("Dataset size: %d UNIQUE words\n", uniqueCount);

    // Allocate based on uniqueCount
    char(*bubbleSorted)[50] = (char(*)[50])malloc(uniqueCount * 50 * sizeof(char));
    char(*quickSorted)[50] = (char(*)[50])malloc(uniqueCount * 50 * sizeof(char));
    char(*mergeSorted)[50] = (char(*)[50])malloc(uniqueCount * 50 * sizeof(char));

    if (!bubbleSorted || !quickSorted || !mergeSorted) {
        printf("ERROR: Memory allocation failed!\n");
        if (bubbleSorted) free(bubbleSorted);
        if (quickSorted) free(quickSorted);
        if (mergeSorted) free(mergeSorted);
        return;
    }

    // Copy UNIQUE words only
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

    // Test Merge Sort
    for (int i = 0; i < uniqueCount; i++) {
        strcpy(mergeSorted[i], uniqueWords[i]);
    }

    printf("\nRunning Merge Sort on %d unique words...\n", uniqueCount);
    clock_t mergeStart = clock();
    mergeSortWords(mergeSorted, uniqueCount);
    clock_t mergeEnd = clock();
    double mergeTime = ((double)(mergeEnd - mergeStart)) / CLOCKS_PER_SEC * 1000;
    printf("Merge Sort complete! Time: %.3f ms\n", mergeTime);

    // Verify all produce same result
    int match = 1;
    for (int i = 0; i < uniqueCount; i++) {
        if (strcmp(bubbleSorted[i], quickSorted[i]) != 0 || 
            strcmp(bubbleSorted[i], mergeSorted[i]) != 0) {
            match = 0;
            break;
        }
    }

    printf("\n====================================================\n");
    printf("              COMPARISON RESULTS\n");
    printf("====================================================\n");

    // Correctness check
    if (match) {
        printf("\n[OK] All three algorithms produced IDENTICAL results!\n");
    }
    else {
        printf("\n[WARNING] Results differ!\n");
    }

    // Performance comparison
    printf("\nPERFORMANCE METRICS:\n");
    printf("  Bubble Sort: %.3f ms\n", bubbleTime);
    printf("  Quick Sort:  %.3f ms\n", quickTime);
    printf("  Merge Sort:  %.3f ms\n", mergeTime);

    // Find fastest
    double fastest = bubbleTime;
    const char *fastestName = "Bubble Sort";
    
    if (quickTime < fastest) {
        fastest = quickTime;
        fastestName = "Quick Sort";
    }
    if (mergeTime < fastest) {
        fastest = mergeTime;
        fastestName = "Merge Sort";
    }

    printf("\n[WINNER] FASTEST ALGORITHM: %s\n", fastestName);

    printf("\nALGORITHM CHARACTERISTICS:\n");
    printf("  Bubble Sort : O(n^2)       - Simple but slow for large data\n");
    printf("  Quick Sort  : O(n log n) - Fast and efficient\n");
    printf("  Merge Sort  : O(n log n) - Stable and consistent performance\n");

    printf("\n====================================================\n");

    // Show first 10 from all
    printf("        First 10 Sorted Unique Words\n");
    printf("====================================================\n");
    for (int i = 0; i < 10 && i < uniqueCount; i++) {
        printf("%2d. %s\n", i + 1, bubbleSorted[i]);
    }
    printf("====================================================\n");

    // Free allocated memory
    free(bubbleSorted);
    free(quickSorted);
    free(mergeSorted);
}

// ============= Main Program =============

/*
 Function: main()
 
 Purpose: Program entry point with main menu loop
 
 Operations:
  1. Allocates dynamic memory for word arrays
  2. Loads stopwords and toxic words dictionaries
  3. Displays interactive menu loop
  4. Handles user input with robust validation
  5. Dispatches to appropriate menu function
  6. Frees memory on exit
 
 Menu Options:
  1-3: File operations (load, add words, reload)
  4-7: Analysis & reports (charts, frequencies, severity, algorithms)
  8-A: Advanced features (comparison, save reports, export)
  0: Exit program
 
 Returns: 0 on success, 1 on memory allocation failure
*/

int main() {
    // Allocate large arrays dynamically at program start
    words = (char(*)[50])malloc(MAX_WORDS * 50 * sizeof(char));
    uniqueWords = (char(*)[50])malloc(MAX_UNIQUE * 50 * sizeof(char));

    if (!words || !uniqueWords) {
        printf("\n============================================\n");
        printf("  FATAL ERROR - INSUFFICIENT MEMORY\n");
        printf("============================================\n");
        printf("The program cannot start due to low memory.\n");
        printf("Please close other applications and try again.\n\n");
        printf("Please reduce MAX_WORDS or MAX_UNIQUE constants.\n");
        return 1;
    }

    stopwordCount = loadWordsFromFile("stopwords.txt", stopwords);
    loadToxicDictionary();

    char choice;

    while (1) {
        printf("\n");
        printf("%s===================================================%s\n", DIVIDER, RESET);
        printf("%s  %sTOXIC TEXT ANALYZER%s                       %s\n", DIVIDER, HEADER, RESET, DIVIDER);
        printf("%s===================================================%s\n", DIVIDER, RESET);
        printf("\n");
        printf("%sFile Operations:%s\n", MENU_LABEL, RESET);
        printf("   %s[1]%s Load and analyze file\n", OPTION_NUM, RESET);
        printf("   %s[2]%s Add/manage toxic words\n", OPTION_NUM, RESET);
        printf("   %s[3]%s Reload dictionaries\n", OPTION_NUM, RESET);
        printf("\n");
        printf("%sAnalysis & Reports:%s\n", MENU_LABEL, RESET);
        printf("   %s[4]%s Display toxic words chart\n", OPTION_NUM, RESET);
        printf("   %s[5]%s Analyze word frequencies\n", OPTION_NUM, RESET);
        printf("   %s[6]%s View severity breakdown\n", OPTION_NUM, RESET);
        printf("   %s[7]%s Compare sorting algorithms\n", OPTION_NUM, RESET);
        printf("\n");
        printf("%sAdvanced:%s\n", MENU_LABEL, RESET);
        printf("   %s[8]%s Compare two files\n", OPTION_NUM, RESET);
        printf("   %s[9]%s Save text report\n", OPTION_NUM, RESET);
        printf("   %s[A]%s Export CSV report\n", OPTION_NUM, RESET);
        printf("\n");
        printf("%s---------------------------------------------------%s\n", DIVIDER, RESET);
        printf("   %s[0]%s Exit program\n", OPTION_NUM, RESET);
        printf("%s---------------------------------------------------%s\n", DIVIDER, RESET);
        printf("\n%s> Enter your choice:%s ", PROMPT, RESET);
    
        choice = getValidMenuChoice("0123456789Aa");
        
        if (choice == '\0') {
            continue; // Loop back on invalid input
        }

        switch (choice) {
        case '1':
            readAndAnalyzeFileMenu();
            break;
        case '2':
            addToxicWordsMenu();
            break;
        case '3':
            reloadDictionaries();
            break;
        case '4':
            displayToxicBarChart();
            break;
        case '5':
            displaySortedWordsMenu();
            break;
        case '6':
            displaySeverityBreakdown();
            break;
        case '7':
            compareSortingAlgorithms();
            break;
        case '8':
            comparativeAnalysisMenu();
            break;
        case '9':
            saveReportMenu();
            break;
        case 'A':
        case 'a':
            saveCSVReport();
            break;
        case '0':
            printf("\n%sThank you for using Toxic Text Analyzer!%s\n\n", SUCCESS, RESET);
            // Free allocated memory before exiting
            free(words);
            free(uniqueWords);
            return 0;
        default:
            printf("Invalid choice. Try again.\n");
            break;
        }
    }
}


