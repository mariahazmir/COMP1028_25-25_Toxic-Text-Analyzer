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

void tokenizeText(char str[], const char* delimiters) {
	char* token = strtok(str, delimiters);
	while (token != NULL) {
		printf("%s\n", token);
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

	printf("How many files do you want to read?\n");
	scanf("%d", &numFiles);

	for (int i = 0; i < numFiles; i++) {


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
			tokenizeText(line, delims); // Tokenize and print
		}

		// Now wordcount has the total words in the whole file
		printf("\nTotal words: %d\n", wordcount);

		fclose(file);

		//return 1;
	}


	//fopen() - open file
	//fgets() - read string form file
	//fclose() - close file
}
