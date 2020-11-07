#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SIZE_OF_STR 20



//*** REMEMBER TO CHECK POSSIBLE FALIURES***

void get_forest_parameters(FILE* file, int* size, int* generations)
{
	char str_size[SIZE_OF_STR], str_ger[SIZE_OF_STR];
	*size = atoi(fgets(str_size, sizeof(str_size), file)); //read first line from file and convert it to int.
	*generations = atoi(fgets(str_ger, sizeof(str_ger), file));//read second line from file and convert it to int.
}

//*** REMEMBER TO CHECK POSSIBLE FALIURES***

/*
int get_forest_line(FILE* file, int CUR_MAX, char* str)
{
	char* line = (char*)malloc(sizeof(char) * CUR_MAX); // allocate memory for line.
	/* The code we will now use to read the lines from the file is taken from stack overflow, address below:
	https://stackoverflow.com/questions/2532425/read-line-from-file-without-knowing-the-line-length 

	int length = 0;
	char ch = getc(file);//read from stream

	while ((ch != '\n') && (ch != EOF))
	{
		if (length == CUR_MAX) { // time to expand ?
			CUR_MAX *= 2; // expand to double the current size of anything similar.
			str = realloc(str, CUR_MAX); // re allocate memory.
		}
		str[length] = ch;// stuff in buffer.
		ch = getc(file); // read from stream. 
		length++;
	}
	return length;
}
*/