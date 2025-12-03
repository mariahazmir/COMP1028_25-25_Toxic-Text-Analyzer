/*
================================================================================
 ANALYZER.C - TEXT ANALYSIS UTILITIES
 
 Purpose: Provides core text analysis and word processing functions
 
 Features:
  - Dictionary lookups (stopwords, toxic words)
  - Word normalization and cleaning
  - Unique word extraction
  - Linguistic metric calculations
  - Text processing utilities
================================================================================
*/

#include "data_types.h"

// ============= Dictionary Check Functions =============

int isStopword(char* word, char stopwords[][50], int stopwordCount) {
    for (int i = 0; i < stopwordCount; i++)
        if (strcmp(word, stopwords[i]) == 0)
            return 1;
    return 0;
}

int isToxic(char* word, char toxicWords[][50], int toxicCount) {
    for (int i = 0; i < toxicCount; i++)
        if (stringEqualsIgnoreCase(word, toxicWords[i]))
            return 1;
    return 0;
}

int isUnique(char token[], char uniqueWords[][50], int uniqueCount) {
    for (int i = 0; i < uniqueCount; i++)
        if (strcmp(token, uniqueWords[i]) == 0)
            return 0;
    return 1;
}

// ============= Statistical Functions =============

double calculateAverageWordLength(char words[][50], int wordCount) {
    int totalLetters = 0;
    for (int i = 0; i < wordCount; i++)
        totalLetters += strlen(words[i]);
    return wordCount ? (double)totalLetters / wordCount : 0.0;
}

double calculateAverageSentenceLength(int wordCount, int sentenceCount) {
    return sentenceCount ? (double)wordCount / sentenceCount : 0.0;
}

double calculateLexicalDiversity(int uniqueCount, int totalWords) {
    return totalWords ? (double)uniqueCount / totalWords : 0.0;
}

// ============= Severity Functions =============

int getToxicSeverity(char* word) {
    for (int i = 0; i < toxicCount; i++) {
        if (stringEqualsIgnoreCase(word, toxicWords[i]))
            return toxicSeverity[i];
    }
    return SEVERITY_MILD;  // Default if not found
}

