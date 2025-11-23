#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <direct.h> 
#define BUFFER_SIZE 1000 

int main() {

	int numFiles;
	int filesNumber = 0;
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
		// print content
		while (fgets(buffer, sizeof(buffer), file) != NULL)  //Format of fgets(fgets(where to put the data, size, file_pointer) // ! = NULL means it has vaild variable
		{
			printf("%s", buffer); //%s means string //why print? showed that the reading wokred & you acn see what was read
		}

		rewind(file); // Reset file pointer to beginning for word count

		//Word Count// 
		char ch;
		int wordcount = 0;
		int inword = 0;

		while ((ch = fgetc(file)) != EOF)
		{
			if (ch == ' ' || ch == '\n' || ch == '\t') //check for space, new line, tab
			{
				inword = 0;
			}

			else if (inword == 0)
			{
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