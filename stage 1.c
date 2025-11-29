#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <direct.h> 
#include <string.h>  
#define BUFFER_SIZE 5000 

void processCharacter( //stage 1 function
	char ch,
	int isCSV,
	int columnToRead,
	int* currentColumn,
	int* inword,
	int* wordcount
) {

	if (isCSV && ch == ',') {
		(*currentColumn)++;
		*inword = 0;
	}
	else if (ch == '\n') {
		*currentColumn = 1;
		*inword = 0;
	}
	else if (isCSV && *currentColumn != columnToRead) {
		// Skip entire character if not in the column
		return;
	}
	else if (ch == ' ' || ch == '\t') {
		*inword = 0;
	}
	else if (*inword == 0) {
		*inword = 1;
		(*wordcount)++;
	}
}

void normalizeCase(char str[]) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] = str[i] + ('a' - 'A'); // convert uppercase to lowercase
		}
	}
}

void removePunctuation(char str[]) {
	int i, j = 0;
	for (i = 0; str[i] != '\0'; i++) {
		if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9') || str[i] == ' ') {
			str[j++] = str[i];
		}
	}
	str[j] = '\0';
}

int isUnique(char token[], char uniqueWords[][50], int uniqueCount) {
	for (int i = 0; i < uniqueCount; i++) {
		if (strcmp(token, uniqueWords[i]) == 0) {
			return 0; // not unique
		}
	}
	return 1; // unique
}

void tokenizeText(char str[], const char* delimiters,
	char uniqueWords[][50], int* uniqueCount,
	char stopwords[][50], int stopwordCount) {

	char* token = strtok(str, delimiters);

	while (token != NULL) {

		// check stopword
		if (!isStopword(token, stopwords, stopwordCount)) {

			// check uniqueness
			if (isUnique(token, uniqueWords, *uniqueCount)) {
				strcpy(uniqueWords[*uniqueCount], token);
				(*uniqueCount)++;
			}
		}

		token = strtok(NULL, delimiters);
	}
}


int loadStopwords(char stopwords[][50]) { //Load stopwords from stopwords.txt
	FILE* f = fopen("stopwords.txt", "r");
	if (!f) {
		printf("Cannot open stopwords.txt\n");
		return 0;
	}

	int count = 0;
	while (fscanf(f, "%s", stopwords[count]) != EOF) {
		count++;
	}
	fclose(f);

	return count; // return how many stopwords were loaded
}

int isStopword(char* word, char stopwords[][50], int stopwordCount) { //Create a function to check if a word is a stopword
	for (int i = 0; i < stopwordCount; i++) {
		if (strcmp(word, stopwords[i]) == 0) {
			return 1; // It is a stopword
		}
	}
	return 0; // Not a stopword
}

double calculateAverageWordLength(char str[]) { //Function to calculate average word length
	int letters = 0;
	int words = 0;
	int inWord = 0;

	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] != ' ') {    // count letters
			letters++;
			if (!inWord) {      // new word starts
				words++;
				inWord = 1;
			}
		}
		else {
			inWord = 0;          // space = end of a word
		}
	}

	if (words == 0) return 0.0;
	return (double)letters / words;
}

void storeWords(char* line, const char* delimiters, char words[][50], int* wordCount,
	char stopwords[][50], int stopwordCount) {
	char copy[BUFFER_SIZE];
	strcpy(copy, line);

	char* token = strtok(copy, delimiters);

	while (token != NULL) {
		if (!isStopword(token, stopwords, stopwordCount)) {  // Only non-stopwords
			if (isUnique(token, words, *wordCount)) {       // Only add if unique
				strcpy(words[*wordCount], token);
				(*wordCount)++;
			}
		}
		token = strtok(NULL, delimiters);
	}
}



int main() {

	int numFiles;
	char fileName[260]; //260 is max path length in windows
	const char* delims = " ,.!?\n";  // delimiters including newline
	char line[BUFFER_SIZE];
	int wordcount;
	int inword;
	int currentColumn;
	char uniqueWords[1000][50];
	int uniqueCount = 0;
	char stopwords[200][50];  // max 200 stopwords
	int stopwordCount = loadStopwords(stopwords);
	int totalLetters = 0;
	int totalWords = 0;
	int inWord = 0;
	char cleanedText[50000] = "";
	char words[2000][50];
	int wordCount = 0;


	printf("How many files do you want to read?\n");
	scanf("%d", &numFiles);

	for (int i = 0; i < numFiles; i++) {

		wordCount = 0;
		uniqueCount = 0;
		cleanedText[0] = '\0';  // empty string

		printf("Enter the name of file %d:\n", i + 1);
		scanf("%s", fileName);

		FILE* file = fopen(fileName, "r");

		if (file == NULL) {
			printf("Error opening file: %s\n", fileName);
			i--;          // retry the same file
			continue;
		}

		printf("\n--- Reading %s ---\n", fileName);
	

		// Check csv files
		char* extension = strrchr(fileName, '.');
		int isCSV = (extension != NULL && strcmp(extension, ".csv") == 0);
		int columnToRead = 0;

		if (isCSV) {
			printf("Which column number to analyze? (1, 2, 3, etc.): ");
			scanf("%d", &columnToRead);
		}
	
		// Reading and word counting
		

		wordcount = 0;

		while (fgets(line, sizeof(line), file)) {
			inword = 0;
			currentColumn = 1;

			// Count words manually using processCharacter
			for (int j = 0; line[j] != '\0'; j++) {
				processCharacter(line[j], isCSV, columnToRead, &currentColumn, &inword, &wordcount);
			}

			normalizeCase(line);        // Convert the line to lowercase first
			removePunctuation(line);
			storeWords(line, delims, words, &wordCount, stopwords, stopwordCount);
			// Add cleaned line to big text buffer
			strcat(cleanedText, line);
			strcat(cleanedText, " ");   // space between lines
			tokenizeText(line, delims, uniqueWords, &uniqueCount, stopwords, stopwordCount);

		}


		// Now wordcount has the total words in the whole file
		printf("\nTotal words: %d\n", wordcount);
		printf("\nUnique words (excluding stopwords):\n");
		for (int i = 0; i < uniqueCount; i++) {
			printf("%s\n", uniqueWords[i]);
		}

		double avg = calculateAverageWordLength(cleanedText);
		printf("Average word length for entire file: %.2f\n", avg);

		double lexicalDiversity = 0.0;
		if (wordcount > 0) {
			lexicalDiversity = (double)uniqueCount / wordcount;
		}
		printf("Lexical Diversity Index: %.4f\n", lexicalDiversity);


		FILE* out = fopen("filtered_words.txt", "w");
		if (out == NULL) {
			printf("Cannot create filtered_words.txt\n");
		}

		for (int i = 0; i < wordCount; i++) {
			fprintf(out, "%s\n", words[i]);
		}

		fclose(out);
		printf("Filtered words saved to filtered_words.txt\n");
		
		fclose(file);

		//return 1;
	}


	//fopen() - open file
	//fgets() - read string form file
	//fclose() - close file
}
