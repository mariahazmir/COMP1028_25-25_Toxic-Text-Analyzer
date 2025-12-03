#include "data_types.h"

// ============= Helper: Perform Analysis on a File and Store Results =============

struct AnalysisResult performFileAnalysis(const char* fileName) {
    struct AnalysisResult result = {0};
    strcpy(result.filename, fileName);
    
    // Reset global counters
    wordCount = uniqueCount = 0;
    totalWords = totalToxicWords = totalSentences = 0;
    for (int i = 0; i < MAX_TOXIC; i++)
        toxicFreq[i] = 0;
    for (int i = 0; i < MAX_TOXIC_PHRASES; i++)
        toxicPhrasesFreq[i] = 0;

    char fullPath[260];
    
    // Try to open file
    FILE* file = fopen(fileName, "r");
    if (!file) {
        snprintf(fullPath, sizeof(fullPath), "analysis/%s", fileName);
        file = fopen(fullPath, "r");
        if (!file) {
            snprintf(fullPath, sizeof(fullPath), "analysis\\%s", fileName);
            file = fopen(fullPath, "r");
        }
    } else {
        snprintf(fullPath, sizeof(fullPath), "%s", fileName);
    }
    
    if (!file) {
        printf("\n  *** FILE NOT FOUND ***\n");
        printf("  File: '%s'\n", fileName);
        printf("  Checked: current dir, ./analysis/, ./analysis\\\n\n");
        result.isLoaded = 0;
        return result;
    }

    // Check if CSV
    int isCSV = 0;
    const char* ext = strrchr(fileName, '.');
    if (ext && stringEqualsIgnoreCase(ext, ".csv")) {
        isCSV = 1;
    }

    int selectedColumns[20] = {0};
    int selectedColCount = 1;
    selectedColumns[0] = 0;
    
    if (isCSV) {
        // For simplicity, analyze first column only
        selectedColCount = 1;
    }

    // Read and process file
    int rowCount = 0;
    printf("Processing '%s'...\n", fileName);
    
    while (1) {
        char line[4096];
        if (!fgets(line, sizeof(line), file)) {
            break;
        }
        
        rowCount++;
        char processLine[100000] = {0};
        
        if (isCSV && rowCount == 1) {
            // Skip header
            continue;
        }
        
        if (isCSV) {
            char fields[50][256];
            int fieldCount = parseCSVLine(line, fields, 50);
            for (int c = 0; c < selectedColCount && c < fieldCount; c++) {
                strcat(processLine, fields[selectedColumns[c]]);
                strcat(processLine, " ");
            }
        } else {
            strcpy(processLine, line);
        }
        
        if (strlen(processLine) > 0 && wordCount < MAX_WORDS) {
            char copy[100000];
            strcpy(copy, processLine);
            
            for (int i = 0; processLine[i]; i++) {
                if (processLine[i] == '.' || processLine[i] == '!' || processLine[i] == '?')
                    totalSentences++;
            }
            
            char* token = strtok(copy, " \t\n,.:;!?\"'");
            while (token != NULL && wordCount < MAX_WORDS) {
                normalizeCase(token);
                removePunctuation(token);
                removeNonASCII(token);
                if (strlen(token) > 1 && !isStopword(token, stopwords, stopwordCount)) {
                    strcpy(words[wordCount++], token);
                }
                token = strtok(NULL, " \t\n,.:;!?\"'");
            }
        }
    }
    
    fclose(file);
    
    if (wordCount == 0) {
        printf("\n  ** No Content Extracted **\n");
        printf("  File: '%s'\n", fileName);
        printf("  The file may be empty or contain only stopwords\n\n");
        result.isLoaded = 0;
        return result;
    }
    
    totalWords = wordCount;
    
    // Build unique words
    printf("Building unique words...\n");
    if (wordCount > 50000) {
        for (int i = 0; i < wordCount && uniqueCount < 10000; i++) {
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
    
    // Count toxic words
    printf("Analyzing toxic words...\n");
    for (int w = 0; w < wordCount; w++) {
        int phraseFound = 0;
        
        if (w + 1 < wordCount && toxicPhrasesCount > 0) {
            char bigram[150];
            sprintf(bigram, "%s %s", words[w], words[w+1]);
            normalizeCase(bigram);
            
            for (int p = 0; p < toxicPhrasesCount; p++) {
                char normalizedPhrase[150];
                strcpy(normalizedPhrase, toxicPhrases[p]);
                normalizeCase(normalizedPhrase);
                if (strcmp(bigram, normalizedPhrase) == 0) {
                    if (p < MAX_TOXIC_PHRASES) toxicPhrasesFreq[p]++;
                    totalToxicWords++;
                    phraseFound = 1;
                    w++;
                    break;
                }
            }
        }
        
        if (!phraseFound && w + 2 < wordCount && toxicPhrasesCount > 0) {
            char trigram[150];
            sprintf(trigram, "%s %s %s", words[w], words[w+1], words[w+2]);
            normalizeCase(trigram);
            
            for (int p = 0; p < toxicPhrasesCount; p++) {
                char normalizedPhrase[150];
                strcpy(normalizedPhrase, toxicPhrases[p]);
                normalizeCase(normalizedPhrase);
                if (strcmp(trigram, normalizedPhrase) == 0) {
                    if (p < MAX_TOXIC_PHRASES) toxicPhrasesFreq[p]++;
                    totalToxicWords++;
                    phraseFound = 1;
                    w += 2;
                    break;
                }
            }
        }
        
        if (!phraseFound && isToxic(words[w], toxicWords, toxicCount)) {
            for (int t = 0; t < toxicCount; t++) {
                if (stringEqualsIgnoreCase(words[w], toxicWords[t])) {
                    toxicFreq[t]++;
                    totalToxicWords++;
                    break;
                }
            }
        }
    }
    
    // Store results
    result.totalWords = totalWords;
    result.uniqueWords = uniqueCount;
    result.totalSentences = totalSentences;
    result.totalToxicWords = totalToxicWords;
    result.averageWordLength = calculateAverageWordLength(words, wordCount);
    result.averageSentenceLength = calculateAverageSentenceLength(totalWords, totalSentences);
    result.lexicalDiversity = calculateLexicalDiversity(uniqueCount, totalWords);
    result.toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    result.nonToxicRatio = 100.0 - result.toxicRatio;
    result.isLoaded = 1;
    
    printf("Analysis complete for '%s'\n\n", fileName);
    return result;
}

// ============= Comparative Analysis Menu =============

void analyzeFileA(void) {
    char fileName[260];
    printf("\n");
    printf("====================================================\n");
    printf("             ANALYZE FILE A\n");
    printf("====================================================\n");
    clearInputBuffer();  // Clear any leftover input
    printf("Enter filename for File A: ");
    scanf("%s", fileName);
    
    fileA_Results = performFileAnalysis(fileName);
    
    if (fileA_Results.isLoaded) {
        printf("\nFile A Analysis Results:\n");
        printf("  Filename: %s\n", fileA_Results.filename);
        printf("  Total words: %d\n", fileA_Results.totalWords);
        printf("  Unique words: %d\n", fileA_Results.uniqueWords);
        printf("  Toxic ratio: %.2f%%\n", fileA_Results.toxicRatio);
        printf("  Lexical diversity: %.4f\n", fileA_Results.lexicalDiversity);
    }
}

void analyzeFileB(void) {
    char fileName[260];
    printf("\n");
    printf("====================================================\n");
    printf("             ANALYZE FILE B\n");
    printf("====================================================\n");
    clearInputBuffer();  // Clear any leftover input
    printf("Enter filename for File B: ");
    scanf("%s", fileName);
    
    fileB_Results = performFileAnalysis(fileName);
    
    if (fileB_Results.isLoaded) {
        printf("\nFile B Analysis Results:\n");
        printf("  Filename: %s\n", fileB_Results.filename);
        printf("  Total words: %d\n", fileB_Results.totalWords);
        printf("  Unique words: %d\n", fileB_Results.uniqueWords);
        printf("  Toxic ratio: %.2f%%\n", fileB_Results.toxicRatio);
        printf("  Lexical diversity: %.4f\n", fileB_Results.lexicalDiversity);
    }
}

void generateComparativeReport(void) {
    if (!fileA_Results.isLoaded || !fileB_Results.isLoaded) {
        printf("\n  *** COMPARISON REQUIRES TWO FILES ***\n");
        printf("  File A: %s\n", fileA_Results.isLoaded ? "LOADED" : "NOT LOADED");
        printf("  File B: %s\n", fileB_Results.isLoaded ? "LOADED" : "NOT LOADED");
        printf("  Please analyze both files before comparing\n\n");
        printf("Please use options to analyze File A and File B.\n");
        return;
    }
    
    printf("\n");
    printf("====================================================\n");
    printf("          COMPARATIVE ANALYSIS REPORT\n");
    printf("====================================================\n\n");
    
    printf("FILE A: %s\n", fileA_Results.filename);
    printf("FILE B: %s\n\n", fileB_Results.filename);
    
    printf("METRIC COMPARISON:\n");
    printf("====================================================\n");
    
    // Total Words
    printf("\nTotal Words:\n");
    printf("  File A: %12d words\n", fileA_Results.totalWords);
    printf("  File B: %12d words\n", fileB_Results.totalWords);
    int wordDiff = fileA_Results.totalWords - fileB_Results.totalWords;
    printf("  Difference: %d (%.1f%% %s)\n", 
        abs(wordDiff), 
        fileB_Results.totalWords > 0 ? (double)abs(wordDiff) / fileB_Results.totalWords * 100 : 0,
        wordDiff > 0 ? "more in A" : "more in B");
    
    // Unique Words
    printf("\nUnique Words:\n");
    printf("  File A: %12d words\n", fileA_Results.uniqueWords);
    printf("  File B: %12d words\n", fileB_Results.uniqueWords);
    int uniqueDiff = fileA_Results.uniqueWords - fileB_Results.uniqueWords;
    printf("  Difference: %d (%.1f%% %s)\n",
        abs(uniqueDiff),
        fileB_Results.uniqueWords > 0 ? (double)abs(uniqueDiff) / fileB_Results.uniqueWords * 100 : 0,
        uniqueDiff > 0 ? "more in A" : "more in B");
    
    // Toxic Ratio
    printf("\nToxic Content Ratio:\n");
    printf("  File A: %12.2f%%\n", fileA_Results.toxicRatio);
    printf("  File B: %12.2f%%\n", fileB_Results.toxicRatio);
    double toxicDiff = fileA_Results.toxicRatio - fileB_Results.toxicRatio;
    printf("  Difference: %+.2f%% (%s toxic)\n",
        toxicDiff,
        toxicDiff > 0 ? "File A is more" : toxicDiff < 0 ? "File B is more" : "equal");
    
    // Lexical Diversity
    printf("\nLexical Diversity Index:\n");
    printf("  File A: %12.4f\n", fileA_Results.lexicalDiversity);
    printf("  File B: %12.4f\n", fileB_Results.lexicalDiversity);
    double diversityDiff = fileA_Results.lexicalDiversity - fileB_Results.lexicalDiversity;
    printf("  Difference: %+.4f (%s diverse)\n",
        diversityDiff,
        diversityDiff > 0 ? "File A is more" : diversityDiff < 0 ? "File B is more" : "equal");
    
    // Average Word Length
    printf("\nAverage Word Length:\n");
    printf("  File A: %12.2f chars\n", fileA_Results.averageWordLength);
    printf("  File B: %12.2f chars\n", fileB_Results.averageWordLength);
    double lengthDiff = fileA_Results.averageWordLength - fileB_Results.averageWordLength;
    printf("  Difference: %+.2f chars\n", lengthDiff);
    
    // Average Sentence Length
    printf("\nAverage Sentence Length:\n");
    printf("  File A: %12.2f words\n", fileA_Results.averageSentenceLength);
    printf("  File B: %12.2f words\n", fileB_Results.averageSentenceLength);
    double sentenceDiff = fileA_Results.averageSentenceLength - fileB_Results.averageSentenceLength;
    printf("  Difference: %+.2f words\n", sentenceDiff);
    
    // Toxic Word Count
    printf("\nToxic Word Occurrences:\n");
    printf("  File A: %12d occurrences\n", fileA_Results.totalToxicWords);
    printf("  File B: %12d occurrences\n", fileB_Results.totalToxicWords);
    int toxicCountDiff = fileA_Results.totalToxicWords - fileB_Results.totalToxicWords;
    printf("  Difference: %+d\n", toxicCountDiff);
    
    printf("\n====================================================\n");
}

void comparativeAnalysisMenu(void) {
    char choice;
    
    while (1) {
        printf("\n");
        printf("====================================================\n");
        printf("         COMPARATIVE ANALYSIS MENU\n");
        printf("====================================================\n");
        printf("* 1. Analyze File A                                *\n");
        printf("* 2. Analyze File B                                *\n");
        printf("* 3. Generate Comparative Report                   *\n");
        printf("* 0. Return to Main Menu                           *\n");
        printf("====================================================\n");
        printf("Enter your choice: ");
        
        scanf(" %c", &choice);
        
        switch (choice) {
        case '1':
            analyzeFileA();
            break;
        case '2':
            analyzeFileB();
            break;
        case '3':
            generateComparativeReport();
            break;
        case '0':
            return;
        default:
            printf("Invalid choice. Try again.\n");
            break;
        }
    }
}
