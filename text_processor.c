#include "data_types.h"

// ============= Normalization Functions =============

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

int stringEqualsIgnoreCase(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

// ============= Word Processing Functions =============

void storeUniqueWords(char* line, char words[][50], int* wordCount,
    char uniqueWords[][50], int* uniqueCount,
    char stopwords[][50], int stopwordCount) {
    char copy[BUFFER_SIZE];
    strcpy(copy, line);
    
    // Count sentence boundaries in the original line
    for (int i = 0; line[i]; i++) {
        if (line[i] == '.' || line[i] == '!' || line[i] == '?')
            totalSentences++;
    }
    
    char* token = strtok(copy, " \t\n,.:;!?\"'");
    while (token != NULL) {
        normalizeCase(token);
        removePunctuation(token);
        removeNonASCII(token);
        // Filter: only keep words with 2+ characters and not stopwords
        if (strlen(token) > 1 && !isStopword(token, stopwords, stopwordCount)) {
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

// ============= CSV Parsing Functions =============

int parseCSVLine(char* line, char fields[][256], int maxFields) {
    int fieldCount = 0;
    int fieldIndex = 0;
    int inQuotes = 0;
    
    for (int i = 0; line[i] && fieldCount < maxFields; i++) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields[fieldCount][fieldIndex] = '\0';
            fieldCount++;
            fieldIndex = 0;
        } else if (c != '\n' && c != '\r') {
            if (fieldIndex < 255) {
                fields[fieldCount][fieldIndex++] = c;
            }
        }
    }
    
    // Add the last field
    if (fieldIndex > 0 || (fieldCount > 0 && line[strlen(line)-1] == ',')) {
        fields[fieldCount][fieldIndex] = '\0';
        fieldCount++;
    }
    
    return fieldCount;
}

int getCSVColumnCount(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    
    char line[BUFFER_SIZE];
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return 0;
    }
    
    fclose(f);
    
    char fields[20][256];
    return parseCSVLine(line, fields, 20);
}

void displayCSVHeaders(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Cannot read CSV file.\n");
        return;
    }
    
    char line[BUFFER_SIZE];
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return;
    }
    
    // Remove UTF-8 BOM if present (EF BB BF)
    unsigned char* line_ptr = (unsigned char*)line;
    if (line[0] && line_ptr[0] == 0xEF && line_ptr[1] == 0xBB && line_ptr[2] == 0xBF) {
        memmove(line, line + 3, strlen(line) - 2);
    }
    
    fclose(f);
    
    char fields[20][256];
    int colCount = parseCSVLine(line, fields, 20);
    
    printf("\nCSV Columns found:\n");
    for (int i = 0; i < colCount; i++) {
        // Trim whitespace and remove non-ASCII
        int start = 0;
        while (fields[i][start] == ' ') start++;
        
        char* field_ptr = (unsigned char*)&fields[i][start];
        // Remove any non-ASCII characters
        for (int j = 0; field_ptr[j]; j++) {
            if ((unsigned char)field_ptr[j] > 127) {
                field_ptr[j] = '?';
            }
        }
        
        printf("  %d. %s\n", i + 1, &fields[i][start]);
    }
}

