#include <stdio.h>
#include <errno.h>

#define _CRT_SECURE_NO_WARNINGS 1


#include "father_utility.h"

#define _CRT
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
	err = fopen_s(&p_file, argv[1], "r");
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
	if (NULL == line || NULL == forest)
	{
		printf("FATAL ERROR: memory alloaction failed. ABORT");
		return -1;
	}

	while (!feof(p_file))
	{
		fgets(line, cur_max, p_file);
		char* token;
		token = strtok(line, ",");
		while (token != NULL)
		{
			if (strlen(forest) == cur_max)
			{
				cur_max *= 2;
				forest = realloc(forest, cur_max);
			}

			/*create string of letters defining forest*/
			strcat(forest, token);

			/* walk through other tokens */
			token = strtok(NULL, ',');
		}
	}
	/*free space allocated for string line*/
	free(line);
	fclose(argv[1]);

	/*open outputfile for writing*/
	FILE* out_file;
	err = fopen_s(&out_file, "output", "w");
	if (err)
	{
		printf("could't open file. ABORT");
		return -1;
	}
	fputs(forest, out_file);
	free(forest);

	//close files
	fclose(out_file);
	
	return(0);
}