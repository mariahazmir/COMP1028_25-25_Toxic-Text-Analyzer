#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <direct.h> 
#include <string.h>  
#define BUFFER_SIZE 5000 

int main() {

	int numFiles;
	char fileName[260]; //260 is max path length in windows

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

		char buffer[BUFFER_SIZE]; //buffer is just a variable for an array of characters
	

		// Check csv files
		char* extension = strrchr(fileName, '.');
		int isCSV = (extension != NULL && strcmp(extension, ".csv") == 0);
		int columnToRead = 0;

		if (isCSV) {
			printf("Which column number to analyze? (1, 2, 3, etc.): ");
			scanf("%d", &columnToRead);
		}
	
		// Reading and word counting
		char ch;
		int wordcount = 0;
		int inword = 0;
		int currentColumn = 1;
		while ((ch = fgetc(file)) != EOF)  //Format of fgets(fgets(where to put the data, size, file_pointer) // ! = NULL means it has vaild variable
		{
			printf("%c", ch); //%s means string //why print? showed that the reading wokred & you acn see what was read

				if (isCSV && ch == ',') {
					currentColumn++;  // Move to next column
					inword = 0;
				}
				else if (ch == '\n') {
					currentColumn = 1;  // New line, reset to column 1
					inword = 0;
				}
				else if (isCSV && currentColumn != columnToRead) {
					// Skip characters not in our target column
					continue;
				}
				else if (ch == ' ' || ch == '\t') {
					inword = 0;
				}
				else if (inword == 0) {
					inword = 1;
					wordcount++;
				}
			
		}

		
		printf("\nTotal words: %d\n", wordcount);

		fclose(file); //close the file

		//return 0;
	}
	//fopen() - open file
	//fgets() - read string form file
	//fclose() - close file
}
