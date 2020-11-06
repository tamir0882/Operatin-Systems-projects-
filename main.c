#include <stdio.h>
#include <errno.h>


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
	err = fopen_s(&p_file, argv[1], "r");
	if (err == 0)
	{
		printf("The file 'forest.txt' was opened\n");
	}
	else
	{
		printf("The file 'forest.txt' was not opened\n");
		return(-1);
	}


	
	int forest_size = 0, *p_forest_size = &forest_size;
	int generations = 0, *p_generations = &generations;

	//read first two lines in file and obtain parameters of forest.
	get_forest_parameters(p_file, p_forest_size, p_generations);

	int CUR_MAX = (forest_size + 1) * 2;
	
	char* line = (char*)malloc(sizeof(char) * CUR_MAX); // allocate memory for line.
	
	char* line_without_comma;


	//obtain first line and its length(return value of the function is the length of the line)
	int length = get_forest_line(p_file, CUR_MAX, line);


	// allocate memory for forest, which is meant to be the full line of letters defining the forest.
	char* forest = (char*)malloc(sizeof(char) * length); 
	int i = 0;

	while (!feof(p_file))
	{
		 //return value of this function is the length of the obtained line.

		/*up to this point we read one line from the file, which contains commas in between the letters.*/
		/*now we would like to remove the commas and leave only the letters in the line*/

		char* token;
		line_without_comma = (char*)malloc(sizeof(char) * length);

		/* get the first token */
		token = strtok(line, ',');
		strcpy(line_without_comma, token);
		
		while (token != NULL) 
		{	
			/* walk through other tokens */
			token = strtok(NULL, ',');
			strcat(line_without_comma, token);
		}


		if (0 == i)
		{
			strcpy(forest, line_without_comma);
			i++;
		}
		int length = get_forest_line(p_file, CUR_MAX, line);

		forest = realloc(forest, (strlen(forest) + length) * sizeof(char));

		strcat(forest, line);




	}
	/*free spac allocated*/
	free(line);
	free(line_without_comma);



	//close files
	fclose(argv[1]);
	
	return(0);
}