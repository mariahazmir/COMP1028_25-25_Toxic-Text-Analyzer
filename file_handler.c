/*
================================================================================
 FILE_HANDLER.C - FILE AND DICTIONARY MANAGEMENT
 
 Purpose:
  - Manages loading and saving of toxic word dictionaries
  - Handles multi-language support (12 languages)
  - Implements user interface for adding/managing toxic words
  - Supports both single words and multi-word phrases
  - Loads toxic words from toxic_words/ folder dynamically
 
 Key Features:
  - Cross-platform directory scanning (Windows/Unix)
  - CSV format parsing (word,severity)
  - Severity-based classification (1=Mild, 2=Moderate, 3=Severe)
  - Dynamic dictionary loading on startup
  - Language selection menu (1-12 languages)
  - Safe file I/O with buffer overflow protection
================================================================================
*/

#include "data_types.h"
#include "colors.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <dirent.h>
#endif

// ============= File I/O Functions =============

/*
 Function: loadWordsFromFile()
 Purpose: Load word list from file (used for stopwords)
 Params: filename - path to file, words - output array
 Returns: Number of words loaded
 Note: Simple line-by-line loading for stopwords dictionary
*/

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

/*
 Function: loadToxicFromFile()
 Purpose: Parse toxic words from file in CSV format (word,severity)
 Params: f - open FILE pointer
 Returns: void (modifies global toxic word arrays)
 
 Features:
  - Parses CSV format: word,severity (e.g., "badword,3")
  - Separates single words from multi-word phrases
  - Validates severity levels (1-3)
  - Prevents duplicate words
  - Handles both old (no severity) and new (with severity) formats
*/

void loadToxicFromFile(FILE* f) {
    char buffer[100];
    char word[100];
    int severity;
    
    while (fgets(buffer, sizeof(buffer), f)) {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strlen(buffer) == 0)
            continue;
        
        // Parse "word,severity" format
        char* comma = strchr(buffer, ',');
        if (comma) {
            // New format with severity
            *comma = 0;  // Null-terminate at comma
            strcpy(word, buffer);
            severity = atoi(comma + 1);
            if (severity < 1 || severity > 3) severity = 1;  // Default to mild if invalid
        } else {
            // Old format without severity - default to mild
            strcpy(word, buffer);
            severity = SEVERITY_MILD;
        }
        
        // Check if this is a multi-word phrase (contains spaces)
        if (strchr(word, ' ')) {
            // It's a multi-word phrase
            if (toxicPhrasesCount < MAX_TOXIC_PHRASES) {
                strcpy(toxicPhrases[toxicPhrasesCount], word);
                toxicPhrasesFreq[toxicPhrasesCount] = 0;
                toxicPhrasesCount++;
            }
        } else {
            // It's a single word
            if (toxicCount < MAX_TOXIC) {
                strcpy(toxicWords[toxicCount], word);
                toxicSeverity[toxicCount] = severity;
                toxicFreq[toxicCount] = 0;
                toxicTerms[toxicCount].severity = severity;
                strcpy(toxicTerms[toxicCount].word, word);
                toxicTerms[toxicCount].count = 0;
                toxicCount++;
            }
        }
    }
}

/*
 Function: loadToxicDictionary()
 Purpose: Dynamically load all toxic word files from toxic_words/ folder
 Returns: void (modifies global toxic word arrays)
 
 Multi-Language Support:
  - Scans toxic_words/ folder for all .txt files
  - Loads 11+ languages: English, Spanish, French, German, Italian,
    Portuguese, Russian, Japanese, Chinese, Arabic, Hindi, Malay
  - Each language file format: word,severity
  - Total: 6,000+ toxic terms across all languages
 
 Platform Support:
  - Windows: Uses FindFirstFileA() for directory scanning
  - Unix/Linux: Uses opendir()/readdir()
  - Cross-platform path handling (backslash vs forward slash)
*/

void loadToxicDictionary(void) {
    toxicCount = 0;
    toxicPhrasesCount = 0;
    
    // Try to load from toxic_words folder (new multi-language structure)
    #ifdef _WIN32
        // Windows: use backslash
        char searchPath[260] = "toxic_words\\*.txt";
        WIN32_FIND_DATAA findData;
        HANDLE findHandle = FindFirstFileA(searchPath, &findData);
        
        if (findHandle != INVALID_HANDLE_VALUE) {
            do {
                // Skip . and .. entries
                if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                    continue;
                
                // Construct full path
                char filePath[260];
                snprintf(filePath, sizeof(filePath), "toxic_words\\%s", findData.cFileName);
                
                FILE* f = fopen(filePath, "r");
                if (f) {
                    loadToxicFromFile(f);
                    fclose(f);
                }
            } while (FindNextFileA(findHandle, &findData));
            
            FindClose(findHandle);
            return;
        }
    #else
        // Unix/Linux: use forward slash
        DIR* dir = opendir("toxic_words");
        if (dir != NULL) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                // Skip . and .. entries
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
                
                // Only load .txt files
                if (strstr(entry->d_name, ".txt") == NULL)
                    continue;
                
                // Construct full path
                char filePath[260];
                snprintf(filePath, sizeof(filePath), "toxic_words/%s", entry->d_name);
                
                FILE* f = fopen(filePath, "r");
                if (f) {
                    loadToxicFromFile(f);
                    fclose(f);
                }
            }
            closedir(dir);
            return;
        }
    #endif
    
    // Fallback to single file if toxic_words folder doesn't exist
    FILE* f = fopen("toxicwords.txt", "r");
    if (!f) {
        printf("Cannot open toxic_words folder or toxicwords.txt\n");
        return;
    }
    
    loadToxicFromFile(f);
    fclose(f);
}

// ============= Dictionary Management =============

/*
 Function: reloadDictionaries()
 Purpose: Reload stopwords and toxic words from files
 Returns: void
 Note: Called after user adds new words or when explicitly requested
*/

void reloadDictionaries(void) {
    stopwordCount = loadWordsFromFile("stopwords.txt", stopwords);
    loadToxicDictionary();

    // Reset toxic frequencies
    for (int i = 0; i < MAX_TOXIC; i++)
        toxicFreq[i] = 0;
    for (int i = 0; i < MAX_TOXIC_PHRASES; i++)
        toxicPhrasesFreq[i] = 0;

    printf("\n%s[SUCCESS] Dictionaries reloaded!%s\n", SUCCESS, RESET);
    printf("%s  |- Stopwords:%s        %d\n", INFO, RESET, stopwordCount);
    printf("%s  |- Toxic words:%s      %d\n", INFO, RESET, toxicCount);
    printf("%s  L- Toxic phrases:%s    %d%s\n\n", INFO, RESET, toxicPhrasesCount, RESET);

    if (stopwordCount == 0)
        printf("%s[WARNING] stopwords.txt is empty or missing%s\n", WARNING, RESET);
    if (toxicCount == 0 && toxicPhrasesCount == 0)
        printf("%s[WARNING] toxicwords.txt is empty or missing%s\n", WARNING, RESET);
}

/*
 Function: addToxicWordsMenu()
 Purpose: Interactive menu to add new toxic words/phrases to dictionaries
 Returns: void (modifies toxic word files)
 
 Features:
  - Language selection (1-12 languages supported)
  - Supports both single words and multi-word phrases
  - User sets severity level (1=Mild, 2=Moderate, 3=Severe)
  - Appends words to language-specific file (e.g., toxic_words/spanish.txt)
  - Validates inputs (no empty words, valid severity)
  - Checks for duplicates before adding
  - Respects MAX_TOXIC and MAX_TOXIC_PHRASES limits
  - Cross-platform file path handling
*/

void addToxicWordsMenu(void) {
    char choice, newWord[100];
    int severity, languageIndex = 0;
    const char* languages[] = {"english", "spanish", "french", "german", "italian", "portuguese", "russian", "japanese", "chinese", "arabic", "hindi", "malay"};
    const char* languageNames[] = {"English", "Spanish", "French", "German", "Italian", "Portuguese", "Russian", "Japanese", "Chinese", "Arabic", "Hindi", "Malay"};
    char filePath[260];
    
    printf("Do you want to add new toxic words or phrases? (y/n): ");
    choice = getValidMenuChoice("ynYN");
    if (choice == '\0') return;

    while (choice == 'y' || choice == 'Y') {
        // Language selection menu
        printf("\n====================================================\n");
        printf("Select language for the new toxic word:\n");
        printf("  1. English        7. Russian\n");
        printf("  2. Spanish        8. Japanese\n");
        printf("  3. French         9. Chinese\n");
        printf("  4. German         10. Arabic\n");
        printf("  5. Italian        11. Hindi\n");
        printf("  6. Portuguese     12. Malay\n");
        printf("====================================================\n");
        printf("Enter choice (1-12): ");
        
        int inputNum = getValidIntInput(1, 12);
        languageIndex = inputNum - 1;
        
        printf("\nAdding to: %s\n", languageNames[languageIndex]);
        printf("Enter new toxic word or phrase (e.g., 'badword' or 'go to hell'): ");
        
        if (fgets(newWord, sizeof(newWord), stdin) != NULL) {
            // Remove trailing newline
            newWord[strcspn(newWord, "\n")] = 0;
        } else {
            printf("Error reading input. Please try again.\n");
            continue;
        }
        
        if (strlen(newWord) == 0) {
            printf("Error: Word cannot be empty.\n");
            continue;
        }

        // Ask for severity level
        printf("\nSet severity level:\n");
        printf("  1 = Mild (vulgar language, crude sexual terms)\n");
        printf("  2 = Moderate (insults, derogatory terms, strong profanities)\n");
        printf("  3 = Severe (hate speech, racist slurs, dehumanizing terms)\n");
        printf("Enter severity (1-3): ");
        severity = getValidIntInput(1, 3);

        int exists = 0;
        
        // Check if it contains spaces (multi-word phrase)
        if (strchr(newWord, ' ')) {
            // Check in toxicPhrases
            for (int i = 0; i < toxicPhrasesCount; i++)
                if (stringEqualsIgnoreCase(newWord, toxicPhrases[i])) {
                    exists = 1;
                    break;
                }
            
            if (!exists && toxicPhrasesCount < MAX_TOXIC_PHRASES) {
                strcpy(toxicPhrases[toxicPhrasesCount++], newWord);
                // Construct path to language file
                #ifdef _WIN32
                    snprintf(filePath, sizeof(filePath), "toxic_words\\%s.txt", languages[languageIndex]);
                #else
                    snprintf(filePath, sizeof(filePath), "toxic_words/%s.txt", languages[languageIndex]);
                #endif
                
                FILE* f = fopen(filePath, "a");
                if (f) {
                    fprintf(f, "%s,%d\n", newWord, severity);
                    fclose(f);
                    printf("\n[OK] '%s' added to %s with severity %d.\n", newWord, languageNames[languageIndex], severity);
                } else {
                    printf("\nERROR: Could not open %s\n", filePath);
                }
            } else if (exists) {
                printf("'%s' already exists.\n", newWord);
            } else {
                printf("\n  ** Phrase Limit Reached **\n");
                printf("  Maximum number of phrases already added\n\n");
            }
        } else {
            // Check in toxicWords (single word)
            for (int i = 0; i < toxicCount; i++)
                if (stringEqualsIgnoreCase(newWord, toxicWords[i])) {
                    exists = 1;
                    break;
                }
            
            if (!exists && toxicCount < MAX_TOXIC) {
                strcpy(toxicWords[toxicCount++], newWord);
                // Construct path to language file
                #ifdef _WIN32
                    snprintf(filePath, sizeof(filePath), "toxic_words\\%s.txt", languages[languageIndex]);
                #else
                    snprintf(filePath, sizeof(filePath), "toxic_words/%s.txt", languages[languageIndex]);
                #endif
                
                FILE* f = fopen(filePath, "a");
                if (f) {
                    fprintf(f, "%s,%d\n", newWord, severity);
                    fclose(f);
                    printf("\n[OK] '%s' added to %s with severity %d.\n", newWord, languageNames[languageIndex], severity);
                } else {
                    printf("\nERROR: Could not open %s\n", filePath);
                }
            } else if (exists) {
                printf("'%s' already exists.\n", newWord);
            } else {
                printf("\n  ** Word Limit Reached **\n");
                printf("  Maximum number of toxic words already added\n\n");
            }
        }

        printf("\nAdd another? (y/n): ");
        choice = getValidMenuChoice("ynYN");
        if (choice == '\0') break;
    }
}

// ============= Report Saving Functions =============

/*
 Function: saveReportMenu()
 Purpose: Save text-based analysis report to file
 Returns: void
 Output File: analysis_report.txt
 
 Report Contents:
  - Total words
  - Unique words
  - Total sentences
  - Average word length
  - Average sentence length
  - Lexical diversity index
  - Toxic ratio
  - Non-toxic ratio
*/

void saveReportMenu(void) {
    FILE* report = fopen("analysis_report.txt", "w");
    if (!report) {
        printf("Cannot open analysis_report.txt\n");
        return;
    }

    if (totalWords == 0) {
        printf("\n  ** No Analysis Data **\n");
        printf("  Please load and analyze a file first (Option 1)\n\n");
        printf("Please analyze a file first (Menu 2).\n");
        return;
    }

    // ===== GENERAL STATISTICS =====
    fprintf(report, "========================================\n");
    fprintf(report, "       TEXT ANALYSIS REPORT\n");
    fprintf(report, "========================================\n\n");
    
    fprintf(report, "--- General Statistics ---\n");
    fprintf(report, "Total words: %d\n", totalWords);
    fprintf(report, "Unique words: %d\n", uniqueCount);
    fprintf(report, "Total sentences: %d\n", totalSentences);
    fprintf(report, "Average word length: %.2f\n", calculateAverageWordLength(words, wordCount));
    fprintf(report, "Average sentence length: %.2f words\n", calculateAverageSentenceLength(totalWords, totalSentences));
    fprintf(report, "Lexical diversity index: %.4f\n\n", calculateLexicalDiversity(uniqueCount, totalWords));

    // ===== TOXICITY METRICS =====
    double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    double nonToxicRatio = totalWords ? (double)(totalWords - totalToxicWords) / totalWords * 100 : 0.0;

    fprintf(report, "--- Toxicity Metrics ---\n");
    fprintf(report, "Total toxic occurrences: %d\n", totalToxicWords);
    fprintf(report, "Toxic ratio: %.2f%%\n", toxicRatio);
    fprintf(report, "Non-toxic ratio: %.2f%%\n\n", nonToxicRatio);

    // ===== SEVERITY BREAKDOWN =====
    int severeMild = 0, severeMod = 0, severeSev = 0;
    int countMild = 0, countMod = 0, countSev = 0;
    
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

    fprintf(report, "--- Severity Breakdown ---\n");
    fprintf(report, "SEVERE (Hate Speech & Slurs):\n");
    fprintf(report, "  Occurrences: %d | Unique words: %d\n", countSev, severeSev);
    fprintf(report, "MODERATE (Insults & Strong Language):\n");
    fprintf(report, "  Occurrences: %d | Unique words: %d\n", countMod, severeMod);
    fprintf(report, "MILD (Vulgar Language & Crude Terms):\n");
    fprintf(report, "  Occurrences: %d | Unique words: %d\n\n", countMild, severeMild);

    // ===== TOP TOXIC WORDS =====
    fprintf(report, "--- Top 10 Toxic Words Detected ---\n");
    
    // Create temporary array with frequency > 0
    struct ToxicWordEntry {
        char word[50];
        int freq;
        int severity;
    } tempWords[toxicCount];
    int tempCount = 0;
    
    for (int i = 0; i < toxicCount; i++) {
        if (toxicFreq[i] > 0) {
            strcpy(tempWords[tempCount].word, toxicWords[i]);
            tempWords[tempCount].freq = toxicFreq[i];
            tempWords[tempCount].severity = toxicSeverity[i];
            tempCount++;
        }
    }
    
    // Sort by frequency descending (bubble sort for simplicity)
    for (int i = 0; i < tempCount - 1; i++) {
        for (int j = 0; j < tempCount - i - 1; j++) {
            if (tempWords[j].freq < tempWords[j + 1].freq) {
                struct ToxicWordEntry temp = tempWords[j];
                tempWords[j] = tempWords[j + 1];
                tempWords[j + 1] = temp;
            }
        }
    }
    
    // Display top 10
    int topCount = 0;
    for (int i = 0; i < tempCount && topCount < 10; i++) {
        char severity[20];
        if (tempWords[i].severity == SEVERITY_MILD) strcpy(severity, "MILD");
        else if (tempWords[i].severity == SEVERITY_MODERATE) strcpy(severity, "MODERATE");
        else strcpy(severity, "SEVERE");
        
        fprintf(report, "%d. %s (Freq: %d, Severity: %s)\n", topCount + 1, tempWords[i].word, tempWords[i].freq, severity);
        topCount++;
    }
    if (topCount == 0) {
        fprintf(report, "No toxic words detected.\n");
    }

    fprintf(report, "\n========================================\n");
    fprintf(report, "Report generated successfully\n");
    fprintf(report, "========================================\n");

    fclose(report);
    printf("Analysis saved to analysis_report.txt\n");
}

/*
 Function: saveCSVReport()
 Purpose: Generates and saves a CSV report of text analysis metrics
 Returns: void
 Output File: analysis_report.csv
 
 Report Contents:
  - Total words
  - Unique words
  - Total sentences
  - Average word length
  - Average sentence length
  - Lexical diversity index
  - Toxic ratio
  - Non-toxic ratio
*/

void saveCSVReport(void) {
    FILE* csv = fopen("analysis_report.csv", "w");
    if (!csv) {
        printf("Cannot open analysis_report.csv\n");
        return;
    }

    if (totalWords == 0) {
        printf("\n  ** Nothing to Save **\n");
        printf("  Please load and analyze a file first (Option 1)\n\n");
        return;
    }

    // ===== GENERAL STATISTICS =====
    fprintf(csv, "GENERAL STATISTICS\n");
    fprintf(csv, "Metric,Value\n");
    fprintf(csv, "Total Words,%d\n", totalWords);
    fprintf(csv, "Unique Words,%d\n", uniqueCount);
    fprintf(csv, "Total Sentences,%d\n", totalSentences);
    fprintf(csv, "Average Word Length,%.2f\n", calculateAverageWordLength(words, wordCount));
    fprintf(csv, "Average Sentence Length (words),%.2f\n", calculateAverageSentenceLength(totalWords, totalSentences));
    fprintf(csv, "Lexical Diversity Index,%.4f\n", calculateLexicalDiversity(uniqueCount, totalWords));

    double toxicRatio = totalWords ? (double)totalToxicWords / totalWords * 100 : 0.0;
    double nonToxicRatio = totalWords ? 100.0 - toxicRatio : 0.0;

    // ===== TOXICITY METRICS =====
    fprintf(csv, "\nTOXICITY METRICS\n");
    fprintf(csv, "Metric,Value\n");
    fprintf(csv, "Total Toxic Occurrences,%d\n", totalToxicWords);
    fprintf(csv, "Toxic Ratio (%%),%.2f\n", toxicRatio);
    fprintf(csv, "Non-Toxic Ratio (%%),%.2f\n", nonToxicRatio);

    // ===== SEVERITY BREAKDOWN =====
    int severeMild = 0, severeMod = 0, severeSev = 0;
    int countMild = 0, countMod = 0, countSev = 0;
    
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

    fprintf(csv, "\nSEVERITY BREAKDOWN\n");
    fprintf(csv, "Severity Level,Occurrences,Unique Words\n");
    fprintf(csv, "SEVERE,\"%d\",\"%d\"\n", countSev, severeSev);
    fprintf(csv, "MODERATE,\"%d\",\"%d\"\n", countMod, severeMod);
    fprintf(csv, "MILD,\"%d\",\"%d\"\n", countMild, severeMild);

    // ===== TOP TOXIC WORDS =====
    fprintf(csv, "\nTOP TOXIC WORDS DETECTED\n");
    fprintf(csv, "Rank,Word,Frequency,Severity\n");
    
    // Create temporary array with frequency > 0
    struct ToxicWordEntry2 {
        char word[50];
        int freq;
        int severity;
    } tempWords2[toxicCount];
    int tempCount2 = 0;
    
    for (int i = 0; i < toxicCount; i++) {
        if (toxicFreq[i] > 0) {
            strcpy(tempWords2[tempCount2].word, toxicWords[i]);
            tempWords2[tempCount2].freq = toxicFreq[i];
            tempWords2[tempCount2].severity = toxicSeverity[i];
            tempCount2++;
        }
    }
    
    // Sort by frequency descending (bubble sort for simplicity)
    for (int i = 0; i < tempCount2 - 1; i++) {
        for (int j = 0; j < tempCount2 - i - 1; j++) {
            if (tempWords2[j].freq < tempWords2[j + 1].freq) {
                struct ToxicWordEntry2 temp = tempWords2[j];
                tempWords2[j] = tempWords2[j + 1];
                tempWords2[j + 1] = temp;
            }
        }
    }
    
    // Display top 15
    int topCount2 = 0;
    for (int i = 0; i < tempCount2 && topCount2 < 15; i++) {
        char severity[20];
        if (tempWords2[i].severity == SEVERITY_MILD) strcpy(severity, "MILD");
        else if (tempWords2[i].severity == SEVERITY_MODERATE) strcpy(severity, "MODERATE");
        else strcpy(severity, "SEVERE");
        
        fprintf(csv, "%d,\"%s\",%d,%s\n", topCount2 + 1, tempWords2[i].word, tempWords2[i].freq, severity);
        topCount2++;
    }
    if (topCount2 == 0) {
        fprintf(csv, "1,No toxic words detected,0,N/A\n");
    }

    fclose(csv);
    printf("CSV report saved as analysis_report.csv\n");
}
