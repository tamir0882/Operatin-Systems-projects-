//Authors: Tamir Dray, Yehonatan Engel
//Project - EX2_318852738_315233650

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "HardCodedData.h"
#include "utility_functions.h"
#include "Thread.h"




int main(int argc, char** argv)
{	

	//check if command line parameters required are given.
	if (argc != EXPECTED_ARGC)
	{
		printf("ARG ERROR - program can not run. %d arguments are required.\n"
			"program shutting down.\n", EXPECTED_ARGC);
		return FAILURE;
	}

	int key = (int)(strtol(argv[KEY_POS], NULL, DECIMAL_BASE));
	if (key == 0)
	{
		if (argv[KEY_POS][KEY_FIRST_DIGIT] != '0')
		{
			printf("ARG ERROR - given key(2nd argument) is invalid, must be a non-zero integer.\n"
				"program shutting down.\n");
			return FAILURE;
		}
	}

	int number_of_threads = (int)(strtol(argv[NUMBER_OF_THREADS_POS],NULL,DECIMAL_BASE));
	if (0 >= number_of_threads)
	{
		printf("ARG ERROR - number of threads(3rd argument) must be a positive integer!\n" 
			"program shutting down.\n");
		return FAILURE;
	}

	char mode = argv[MODE_POS][MODE_SUB_POS];
	if (mode != 'e' && mode != 'd')
	{
		printf("ARG ERROR - given mode(4th argument) is invalid. must be \"-e\" or \"-d\".\n"
		"program shutting down.\n");
		return FAILURE;
	}

	int number_of_lines = 0;
	HANDLE h_input_file = open_file_and_count_lines(argv[INPUT_FILE_PATH_POS], &number_of_lines);

	if (NULL == h_input_file)
	{
		printf("open_file_and_count_lines has failed. ABORT.\n");
		return FAILURE;
	}
	
	
	Data* array_of_thread_data = (Data*)malloc(sizeof(Data) * number_of_threads);
	if (NULL == array_of_thread_data)
	{
		printf("MEMORY ERROR - allocation failed. attempting to close input file.\n");
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close file. ABORT.\n");
			return FAILURE;
		}
		printf("file successfully closed.\n"
			"program shutting down.\n");
		return FAILURE;
	}

	DWORD p_thread_ids[MAXIMUM_WAIT_OBJECTS] = { 0 };
	HANDLE p_thread_handles[MAXIMUM_WAIT_OBJECTS] = { 0 };

	DWORD exit_code;
	DWORD wait_code;

	HANDLE h_initiation_semaphore = NULL;
	h_initiation_semaphore = CreateSemaphoreA(NULL, // default sec. Attr.
		0, // initial count
		number_of_threads, // max count
		Thread); // no name)
	
	if (NULL == h_initiation_semaphore)
	{
		printf("SEMAPHORE ERROR - CreateSemaphore failed. ABORT.\n");
		free(array_of_thread_data);
		return FAILURE;
	}

	HANDLE h_mutex = NULL;
	h_mutex = CreateMutexA(NULL, FALSE, NULL);

	if (NULL == h_mutex)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		free(array_of_thread_data);
		return FAILURE;
	}

	int lines_per_thread = (int)(number_of_lines / number_of_threads);
	int residual = number_of_lines % number_of_threads;

	set_same_data(array_of_thread_data, number_of_threads, number_of_lines, argv[INPUT_FILE_PATH_POS],
		OUTPUT_FILE_NAME, mode, key, h_initiation_semaphore, h_mutex);

	if (FAILURE == set_indexes(h_input_file, array_of_thread_data, number_of_threads, lines_per_thread, residual))
	{
		printf("set_indexes failed. attempting to close input file.\n");
		free(array_of_thread_data);
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close file handle. ABORT.\n");
			return FAILURE;
		}
		printf("file was close successfully. program shutting down.\n");
		return FAILURE;
	}

	if (0 == CloseHandle(h_input_file))
	{
		printf("FILE ERROR - couldn't close file handle. ABORT.\n");
		free(array_of_thread_data);
		return FAILURE;
	}


	for (int i = 0; i < number_of_threads; i++)
	{
		p_thread_handles[i] = create_thread_simple(Thread, &p_thread_ids[i], &array_of_thread_data[i]);
		
		if (NULL == p_thread_handles[i])
		{
			printf("THREAD ERROR - create_thread_simple returned NULL pointer. ABORT.\n");
			free(array_of_thread_data);;
			return FAILURE;
		}

		if (0 == GetExitCodeThread(p_thread_handles[i], &exit_code))
		{
			printf("THREAD ERROR - GetExitCodeThread failed. ABORT.\n");
			free(array_of_thread_data);
			return FAILURE;
		}

		if (exit_code == FAILURE)
		{
			printf("THREAD ERROR - thread operation failed\n");
			free(array_of_thread_data);
			return FAILURE;
		}
	}

	if (0 == ReleaseSemaphore(h_initiation_semaphore, number_of_threads, NULL))
	{
		printf("SEMAPHORE ERROR - RealeseSemaphore failed. ABORT.\n");
		free(array_of_thread_data);
		return FAILURE;
	}
	

	wait_code = WaitForMultipleObjects(number_of_threads, p_thread_handles, TRUE, WAIT_TIME);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("THREAD ERROR - Error when waiting.\n");
		free(array_of_thread_data);
		return FAILURE;
	}

	for (int i = 0; i < number_of_threads; i++)
	{
		if (0 == CloseHandle(p_thread_handles[i]))
		{
			printf("THREAD ERROR - couldn't close thread handle. ABORT.\n");
			free(array_of_thread_data);
			return FAILURE;
		}
	}

	if (0 == CloseHandle(h_mutex))
	{
		printf("MUTEX ERROR - couldn't close mutex handle. ABORT.\n");
		free(array_of_thread_data);
		return FAILURE;
	}
	
	if (0 == CloseHandle(h_initiation_semaphore))
	{
		printf("SEMAPHORE ERROR - couldn't close semaphore handle. ABORT.\n");
		free(array_of_thread_data);
		return FAILURE;
	}


	free(array_of_thread_data);

	printf("Great Success!\n");

	return SUCCESS;
}
#endif