#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 5000
#define MAX_WORDS 2000
#define MAX_UNIQUE 1000
#define MAX_STOPWORDS 200
#define MAX_TOXIC 200

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

int isUnique(char token[], char uniqueWords[][50], int uniqueCount) {
    for (int i = 0; i < uniqueCount; i++)
        if (strcmp(token, uniqueWords[i]) == 0) return 0;
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
        if (strcmp(word, stopwords[i]) == 0) return 1;
    return 0;
}

// Cross-platform case-insensitive string comparison
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

// Store unique non-stopwords
void storeUniqueWords(char* line, char words[][50], int* wordCount,
    char uniqueWords[][50], int* uniqueCount,
    char stopwords[][50], int stopwordCount) {
    char copy[BUFFER_SIZE];
    strcpy(copy, line);

    char* token = strtok(copy, " \t\n,.:;!?\"'");
    while (token != NULL) {
        normalizeCase(token);
        removePunctuation(token);

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

void bubbleSortWords(char words[][50], int count) {
    char temp[50];
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(words[j], words[j + 1]) > 0) {
                // Swap words[j] and words[j+1]
                strcpy(temp, words[j]);
                strcpy(words[j], words[j + 1]);
                strcpy(words[j + 1], temp);
            }
        }
    }
}

void bubbleSortToxicByFrequency(char toxicWords[][50], int toxicFreq[], int toxicCount) {
    char tempWord[50];
    int tempFreq;
    for (int i = 0; i < toxicCount - 1; i++) {
        for (int j = 0; j < toxicCount - i - 1; j++) {
            if (toxicFreq[j] < toxicFreq[j + 1]) { // descending frequency
                // Swap frequency
                tempFreq = toxicFreq[j];
                toxicFreq[j] = toxicFreq[j + 1];
                toxicFreq[j + 1] = tempFreq;
                // Swap words
                strcpy(tempWord, toxicWords[j]);
                strcpy(toxicWords[j], toxicWords[j + 1]);
                strcpy(toxicWords[j + 1], tempWord);
            }
        }
    }
}

// ------------top N frequent words-------------

//Step 1: Count word frequencies
typedef struct {
    char word[50];
    int freq;
} WordFreq;

void countWordFrequencies(char words[][50], int wordCount, WordFreq wordFreqs[], int* uniqueCount) {
    *uniqueCount = 0;
    for (int i = 0; i < wordCount; i++) {
        int found = 0;
        for (int j = 0; j < *uniqueCount; j++) {
            if (strcmp(words[i], wordFreqs[j].word) == 0) {
                wordFreqs[j].freq++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(wordFreqs[*uniqueCount].word, words[i]);
            wordFreqs[*uniqueCount].freq = 1;
            (*uniqueCount)++;
        }
    }
}

//Step 2: Sort by frequency (descending)
void bubbleSortByFrequency(WordFreq wordFreqs[], int count) {
    WordFreq temp;
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (wordFreqs[j].freq < wordFreqs[j + 1].freq) {
                temp = wordFreqs[j];
                wordFreqs[j] = wordFreqs[j + 1];
                wordFreqs[j + 1] = temp;
            }
        }
    }
}

//Step 3: Display top N words
void displayTopNWords(WordFreq wordFreqs[], int uniqueCount, int N) {
    if (N > uniqueCount) N = uniqueCount;
    printf("\nTop %d frequent words:\n", N);
    for (int i = 0; i < N; i++) {
        printf("%s: %d times\n", wordFreqs[i].word, wordFreqs[i].freq);
    }
}

// ----------another sorting method-(Quick)--------
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
        if (strcmp(words[j], pivot) < 0) {  // alphabetical order
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



// ---------------- Main Program ----------------

int main() {
    char stopwords[MAX_STOPWORDS][50];
    int stopwordCount = loadWordsFromFile("stopwords.txt", stopwords);

    char toxicWords[MAX_TOXIC][50];
    int toxicCount = loadWordsFromFile("toxicwords.txt", toxicWords);

    // --- Add new toxic words ---
    char choice, newWord[50];
    printf("Do you want to add new toxic words? (y/n): ");
    scanf(" %c", &choice);
    while (choice == 'y' || choice == 'Y') {
        printf("Enter new toxic word: ");
        scanf("%s", newWord);

        int exists = 0;
        for (int i = 0; i < toxicCount; i++)
            if (stringEqualsIgnoreCase(newWord, toxicWords[i])) { exists = 1; break; }

        if (!exists) {
            strcpy(toxicWords[toxicCount++], newWord);
            FILE* f = fopen("toxicwords.txt", "a");
            if (f) { fprintf(f, "%s\n", newWord); fclose(f); }
            printf("'%s' added.\n", newWord);
        }
        else {
            printf("'%s' already exists.\n", newWord);
        }

        printf("Add another? (y/n): ");
        scanf(" %c", &choice);
    }

    int numFiles;
    printf("How many files to read? ");
    scanf("%d", &numFiles);

    for (int i = 0; i < numFiles; i++) {
        char fileName[260];
        printf("Enter filename %d: ", i + 1);
        scanf("%s", fileName);

        FILE* file = fopen(fileName, "r");
        if (!file) { printf("Cannot open %s\n", fileName); i--; continue; }

        printf("\n--- Reading %s ---\n", fileName);

        char line[BUFFER_SIZE];
        char words[MAX_WORDS][50];
        int wordCount = 0;
        char uniqueWords[MAX_UNIQUE][50];
        int uniqueCount = 0;
        int toxicFreq[MAX_TOXIC] = { 0 };
        int totalWords = 0;
        int totalToxicWords = 0;

        // --- Read lines and tokenize ---
        while (fgets(line, sizeof(line), file)) {
            storeUniqueWords(line, words, &wordCount, uniqueWords, &uniqueCount,
                stopwords, stopwordCount);
        }
        totalWords = wordCount;

        // --- Detect toxic words ---
        for (int w = 0; w < wordCount; w++) {
            for (int t = 0; t < toxicCount; t++) {
                if (stringEqualsIgnoreCase(words[w], toxicWords[t])) {
                    toxicFreq[t]++;
                    totalToxicWords++;
                }
            }
        }

        // --- Step 1: Gather toxic words that actually appear ---
        WordFreq toxicWordFreqs[MAX_TOXIC];
        int toxicFreqCount = 0;
        for (int t = 0; t < toxicCount; t++) {
            if (toxicFreq[t] > 0) {
                strcpy(toxicWordFreqs[toxicFreqCount].word, toxicWords[t]);
                toxicWordFreqs[toxicFreqCount].freq = toxicFreq[t];
                toxicFreqCount++;
            }
        }

        // --- Step 2: Sort toxic words by frequency ---
        bubbleSortByFrequency(toxicWordFreqs, toxicFreqCount);

        // --- Step 3: Display top N toxic words ---
        if (toxicFreqCount > 0) {
            int N_toxic;
            printf("\nHow many top toxic words to display? ");
            scanf("%d", &N_toxic);
            displayTopNWords(toxicWordFreqs, toxicFreqCount, N_toxic);
        }
        else {
            printf("\nNo toxic words found.\n");
        }

        // --- General statistics ---
        printf("\nTotal words: %d\n", totalWords);
        printf("Unique words (excluding stopwords): %d\n", uniqueCount);
        printf("Average word length: %.2f\n", calculateAverageWordLength(words, wordCount));
        double lexDiv = totalWords ? (double)uniqueCount / totalWords : 0.0;
        printf("Lexical Diversity Index: %.4f\n", lexDiv);

        // --- Words sorted alphabetically (Quick Sort) ---
        quickSortWords(words, 0, wordCount - 1);
        printf("\nWords sorted alphabetically (Quick Sort):\n");
        for (int w = 0; w < wordCount; w++)
            printf("%s\n", words[w]);

        // --- Toxic words sorted by frequency ---
        printf("\nToxic words sorted by frequency:\n");
        for (int t = 0; t < toxicCount; t++) {
            if (toxicFreq[t] > 0) {
                double perc = ((double)toxicFreq[t] / totalWords) * 100;
                printf("%s: %d times (%.2f%%)\n", toxicWords[t], toxicFreq[t], perc);
            }
        }
        printf("Total toxic words: %d / %d words (%.2f%%)\n",
            totalToxicWords, totalWords,
            totalWords ? ((double)totalToxicWords / totalWords) * 100 : 0.0);

        // Calculate toxic vs non-toxic ratios
        double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
        double nonToxicRatio = totalWords ? (double)(totalWords - totalToxicWords) / totalWords * 100 : 0.0;

        printf("\n--- Extra Summary Stats ---\n");
        printf("Toxic words ratio: %.2f%%\n", toxicRatio);
        printf("Non-toxic words ratio: %.2f%%\n", nonToxicRatio);


        // --- Save filtered words ---
        FILE* out = fopen("filtered_words.txt", "w");
        if (out) {
            for (int w = 0; w < wordCount; w++)
                fprintf(out, "%s\n", words[w]);
            fclose(out);
            printf("Filtered words saved to filtered_words.txt\n");
        }

        // --- Top N frequent words ---
        WordFreq wordFreqs[MAX_WORDS];
        int uniqueFreqCount = 0;
        countWordFrequencies(words, wordCount, wordFreqs, &uniqueFreqCount);
        bubbleSortByFrequency(wordFreqs, uniqueFreqCount);
        int N = 10;
        displayTopNWords(wordFreqs, uniqueFreqCount, N);

        fclose(file);
    }

    return 0;
}
