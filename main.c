//Authors: Tamir Dray, Yehonatan Engel
//Project - EX2_318852738_315233650

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include "HardCodedData.h"
#include "utility_functions.h"



int main(int argc, char** argv)
{

	/*HANDLE h_out_file = create_output_file("output.txt");
	if (h_out_file == NULL)
	{
		printf("FILE ERROR - ouput file was no opened. ABORT.\n");
		return FAILURE;
	}
	
	if (0 == CloseHandle(h_out_file))
	{
		printf("FILE ERROR - couldn't close file. ABORT.\n");
		return FAILURE;
	}
	return SUCCESS;
	*/


	//check if command line parameters required are give.
	if (argc != EXPECTED_ARGC)
	{
		printf("ARG ERROR - program can not run. %d arguments are required. ABORT.\n", EXPECTED_ARGC);
		return FAILURE;
	}

	int number_of_threads = (int)(argv[3][0] - '0');
	int number_of_lines = 0;
	HANDLE h_file = open_file_and_count_lines(argv[1], &number_of_lines);

	if (NULL == h_file)
	{
		printf("FILE ERROR - input file failed. ABORT.\n");
		return FAILURE;
	}


	//check number of threads and divide the lines between them.
	
	int lines_per_thread = (int)(number_of_lines / number_of_threads);
	int residual = number_of_lines % number_of_threads;
	
	
	//create arrays of indexes in file for threads

	int array_size = sizeof(int) * number_of_threads;

	int* start_index = (int)malloc(array_size);

	if (NULL == start_index)
	{
		printf("MEMORY ERROR - memory allocation failed. ABORT.\n");
		return FAILURE;
	}
	
	int* end_index = (int)malloc(array_size);
	if (NULL == end_index)
	{
		printf("MEMORY ERROR - memory allocation failed. ABORT.\n");
		free(start_index);
		return FAILURE;
	}


	set_indexes_for_threads(start_index, end_index, number_of_threads, lines_per_thread, residual, h_file);

	
	
	if (0 == CloseHandle(h_file))
	{
		printf("FILE ERROR - couldn't close file. ABORT.\n");
		return FAILURE;
	}
	
	free(start_index);
	free(end_index);

	printf("great success!\n");

	return SUCCESS;
}
#endif