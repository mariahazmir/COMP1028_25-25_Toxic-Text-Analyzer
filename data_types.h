/*
================================================================================
 DATA_TYPES.H - GLOBAL DEFINITIONS AND STRUCTURES
 
 Purpose: Header file containing all data types, constants, and global
          declarations used throughout the toxic words analysis system
 
 Key Features:
  - Constants for array sizes and limits
  - Data structures for analysis and storage
  - Input validation helper functions
  - External function declarations
 
 Configuration:
  - MAX_TOXIC: 10,000 toxic words
  - MAX_TOXIC_PHRASES: 2,000 multi-word phrases
  - MAX_WORDS: 100,000 words per file
  - Supports 12 languages
================================================================================
*/

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

// ============= Configuration Constants =============

#define BUFFER_SIZE 100000      // Buffer for file reading
#define MAX_WORDS 1000000       // Maximum words to extract from file
#define MAX_UNIQUE 100000       // Maximum unique words to track
#define MAX_STOPWORDS 2000      // Maximum stopwords to load
#define MAX_TOXIC 50000         // Maximum toxic words
#define MAX_TOXIC_PHRASES 10000 // Maximum multi-word phrases

// ============= Severity Levels =============

#define SEVERITY_MILD 1         // Vulgar language, crude sexual terms
#define SEVERITY_MODERATE 2     // Insults, derogatory terms, strong profanities
#define SEVERITY_SEVERE 3       // Hate speech, racist slurs, dehumanizing terms

// ============= Input Validation Helpers =============

/*
 clearInputBuffer(): Clears remaining characters from input buffer
 getValidMenuChoice(): Validates single character menu input
 getValidIntInput(): Validates integer input within specified range
 All include error messages and retry logic for robust input handling
*/

static void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static char getValidMenuChoice(const char* validChoices) {
    char buffer[10];
    char choice;
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Error: Failed to read input.\n");
        return '\0';
    }
    
    // Get first non-whitespace character
    choice = buffer[0];
    if (choice == '\n' || choice == '\r') {
        printf("Error: Invalid input. Please enter a single character.\n");
        return '\0';
    }
    
    if (validChoices != NULL) {
        int found = 0;
        for (int i = 0; validChoices[i] != '\0'; i++) {
            if (choice == validChoices[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Error: '%c' is not a valid choice. Please try again.\n", choice);
            return '\0';
        }
    }
    
    return choice;
}

static int getValidIntInput(int min, int max) {
    int value;
    int result;
    
    while (1) {
        result = scanf("%d", &value);
        
        if (result != 1) {
            // Clear the invalid input from buffer
            clearInputBuffer();
            printf("Error: Please enter a valid number between %d and %d.\n", min, max);
            continue;
        }
        
        if (value < min || value > max) {
            clearInputBuffer();
            printf("Error: Please enter a number between %d and %d.\n", min, max);
            continue;
        }
        
        clearInputBuffer();
        return value;
    }
}

// ============= Toxic Term Structure =============

/*
 Structure for storing individual toxic words with metadata
 Used for detailed toxicity analysis and reporting
*/

struct ToxicTerm {
    char word[100];     // The toxic word or phrase
    int count;          // Frequency count in analyzed text
    int severity;       // Severity level (1=Mild, 2=Moderate, 3=Severe)
};

// ============= Word Frequency Structure =============

/*
 Structure for tracking word frequencies
 Used for frequency-based analysis and sorting
*/

struct WordFreq {
    char word[100];
    int freq;
};

// ============= Analysis Result Structure =============

/*
 Structure for storing complete analysis results for a file
 Used for comparative analysis of multiple files
*/

struct AnalysisResult {
    char filename[260];         // Name of analyzed file
    int totalWords;             // Total word count
    int uniqueWords;            // Unique word count
    int totalSentences;         // Sentence count
    int totalToxicWords;        // Toxic occurrences found
    double averageWordLength;   // Average word length
    double averageSentenceLength; // Average sentence length
    double lexicalDiversity;    // Lexical diversity index
    double toxicRatio;          // Percentage toxic words
    double nonToxicRatio;       // Percentage non-toxic words
    int isLoaded;               // Flag: valid data loaded
};

// ============= Global Variables =============

extern char (*words)[50];
extern char (*uniqueWords)[50];
extern int wordCount;
extern int uniqueCount;

extern char stopwords[MAX_STOPWORDS][50];
extern int stopwordCount;

extern char toxicWords[MAX_TOXIC][50];
extern int toxicCount;
extern int toxicFreq[MAX_TOXIC];
extern int toxicSeverity[MAX_TOXIC];

extern struct ToxicTerm toxicTerms[MAX_TOXIC];

extern char toxicPhrases[MAX_TOXIC_PHRASES][100];
extern int toxicPhrasesCount;
extern int toxicPhrasesFreq[MAX_TOXIC_PHRASES];

extern int totalWords;
extern int totalToxicWords;
extern int totalSentences;

// ============= Comparative Analysis =============

extern struct AnalysisResult fileA_Results;
extern struct AnalysisResult fileB_Results;

// ============= text_processor.c =============

void normalizeCase(char str[]);
void removePunctuation(char str[]);
void removeNonASCII(char str[]);
void storeUniqueWords(char* line, char words[][50], int* wordCount,
    char uniqueWords[][50], int* uniqueCount,
    char stopwords[][50], int stopwordCount);
int stringEqualsIgnoreCase(const char* a, const char* b);
int parseCSVLine(char* line, char fields[][256], int maxFields);
int getCSVColumnCount(const char* filename);
void displayCSVHeaders(const char* filename);

// ============= file_handler.c =============

int loadWordsFromFile(const char* filename, char words[][50]);
void reloadDictionaries(void);
void saveReportMenu(void);
void saveCSVReport(void);
void addToxicWordsMenu(void);
void loadToxicDictionary(void);

// ============= analyzer.c =============

int isStopword(char* word, char stopwords[][50], int stopwordCount);
int isToxic(char* word, char toxicWords[][50], int toxicCount);
int isUnique(char token[], char uniqueWords[][50], int uniqueCount);
double calculateAverageWordLength(char words[][50], int wordCount);
double calculateAverageSentenceLength(int wordCount, int sentenceCount);
double calculateLexicalDiversity(int uniqueCount, int totalWords);

// ============= sorting.c =============

void bubbleSortWords(char words[][50], int count);
void swapWords(char a[50], char b[50]);
int partition(char words[][50], int low, int high);
void quickSortWords(char words[][50], int low, int high);
void mergeSortWords(char words[][50], int count);
void mergeSortWordsHelper(char words[][50], int left, int right);
void mergeWords(char words[][50], int left, int mid, int right);

// ============= reporting.c =============

void displayToxicBarChart(void);
void displayWordsByFrequency(char (*sortedWords)[50], int wordCount, char sortChoice);
void displayToxicWordsByCount(char sortChoice);
void bubbleSortWordFreq(struct WordFreq *arr, int n);
void quickSortWordFreq(struct WordFreq *arr, int low, int high);
void mergeSortWordFreq(struct WordFreq *arr, int n);
void mergeWordFreq(struct WordFreq *arr, int left, int mid, int right);
void mergeSortWordFreqHelper(struct WordFreq *arr, int left, int right);
void displaySeverityBreakdown(void);
int getToxicSeverity(char* word);

// ============= Comparative Analysis Menu =============

void comparativeAnalysisMenu(void);
void analyzeFileA(void);
void analyzeFileB(void);
void generateComparativeReport(void);

#endif
