#include <stdio.h>
#include <errno.h>


#define _CRT_SECURE_NO_WARNINGS




int main(int argc, const char* argv[])
{
	if (2 != argc)
	{
		printf("Error, one argument expected.\n");
		return(-1);
	}

	FILE* p_file;
	errno_t err;
	// Open for read (will fail if file argv[1] does not exist)
	err = fopen_s(&p_file, argv[1], "r");
	if (err == 0)
	{
		printf("The file 'crt_fopen_s.c' was opened\n");
	}
	else
	{
		printf("The file 'crt_fopen_s.c' was not opened\n");
		return(-1);
	}
	int forest_size = 0, *p_forest_size = &forest_size;
	int generations = 0, *p_generations = &generations;
	
	char input1[64], input2[64];
	fgets(input1, 3, p_file);
	fgets(input2, 3, p_file);
	printf("input1 is: %sinput 2 is: %sis this a new line?\n", input1, input2);



	//************IMPORTANT***********
	fclose(argv[1]);
	//************IMPORTANT***********
	return(0);
}