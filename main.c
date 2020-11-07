#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS 1


#include "father_utility.h"

int main(int argc, const char* argv[])
{
	if (2 != argc)
	{
		printf("Error, one argument expected.\n");
		return(-1);
	}

	FILE* p_file;
	errno_t err;

	// Open for read (will fail if file  in argv[1] does not exist)
	//-----------------
	err = fopen_s(&p_file, "C:\test.txt", "r");
	if (err)
	{
		printf("could't open file. ABORT");
		return -1;
	}
	//------------------

	int forest_size = 0, *p_forest_size = &forest_size;
	int generations = 0, *p_generations = &generations;

	//read first two lines in file and obtain parameters of forest.
	get_forest_parameters(p_file, p_forest_size, p_generations);


	/*notice that forest_size is in fact the number of columns in the forest*/

	int cur_max = (forest_size + 1) * 2;

	char* line = (char*)malloc(sizeof(char) * cur_max);
	char* forest = (char*)malloc(sizeof(char) * cur_max);
	char* tmp;

	int line_size = cur_max;
	if (NULL == line || NULL == forest)
	{
		printf("FATAL ERROR: memory alloaction failed. ABORT");
		return -1;
	}

	while (!feof(p_file))
	{
		fgets(line, line_size, p_file);
		rsize_t strmax = sizeof line;
		char* token;
		char* next_token;
		token = strtok_s(line, &strmax, ",", &next_token);
		while (token != NULL)
		{
			if (NULL != forest)
			{
				if (strlen(forest) >= cur_max - 1)
				{
					cur_max *= 2;
					tmp = realloc(forest, cur_max);
					if (NULL != tmp)
					{
						forest = tmp;
					}
					else
					{
						printf("memory allocation faliure");
						return -1;
					}
				}
			}
			else
				return -1;
			

			/*create string of letters defining forest*/
			strcat(forest, token);

			/* walk through other tokens */
			token = strtok_s(NULL, &strmax, ",", &next_token);
		}
	}
	/*free space allocated for string line*/
	free(line);
	fclose(argv[1]);

	/*open outputfile for writing*/
	/*FILE* out_file;
	err = fopen_s(&out_file, "output", "w");
	if (err)
	{
		printf("could't open file. ABORT");
		return -1;
	}
	fputs(forest, out_file);*/
	free(forest);

	//close files
	//fclose(out_file);
	
	return(0);
}