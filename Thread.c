
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include "HardCodedData.h"
#include "utility_functions.h"



DWORD WINAPI Thread(LPVOID lpParam)
{
	Data* p_thread_data = (Data*) lpParam;

	DWORD wait_code_semaphore;

	wait_code_semaphore = WaitForSingleObject(p_thread_data->semaphore, WAIT_TIME);

	switch (wait_code_semaphore)
	{
	case WAIT_ABANDONED: 
		printf("semaphore wait abandoned\n");
		return FAILURE;
	case WAIT_TIMEOUT:
		printf("semaphore wait timeout\n");
		return FAILURE;
	case WAIT_FAILED:
		printf("semaphore wait failed\n");
		return FAILURE;
	}
	/*
	SYSTEMTIME start_time;
	GetSystemTime(&start_time);
	printf("%d:%d\n", start_time.wSecond, start_time.wMilliseconds);
	*/
	HANDLE h_input_file = create_file_for_read(p_thread_data->input_file_name);
	if (NULL == h_input_file)
	{
		printf("FILE ERROR - create_file_for_read returned NULL pointer");
		return FAILURE;
	}

	HANDLE h_output_file = CreateFileA(p_thread_data->output_file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (ERROR_FILE_NOT_FOUND == GetLastError() || INVALID_HANDLE_VALUE == h_output_file)
	{
		printf("ERROR - could not open file.\n");
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}
		return FAILURE;
	}
	//---------------------------------------------************************



	if (INVALID_SET_FILE_POINTER == SetFilePointer(h_input_file, p_thread_data->start_index, NULL, FILE_BEGIN))
	{
		printf("SetFilePointer failed. ABORT");
		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}
		return FAILURE;
	}

	if (INVALID_SET_FILE_POINTER == SetFilePointer(h_output_file, p_thread_data->start_index, NULL, FILE_BEGIN))
	{
		printf("SetFilePointer failed. ABORT");
		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}
		return FAILURE;
	}
	

	int number_of_bytes = (p_thread_data->end_index - p_thread_data->start_index + 1);


	char* buffer_read = (char*)malloc(sizeof(char) * (number_of_bytes + 1));
	if (NULL == buffer_read)
	{
		printf("memory allocation falied.\n");
		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}
		return FAILURE;
	}
	LPDWORD bytes_read = 0;
	BOOL ret_read = FALSE;
	
	char* buffer_write = (char*)malloc(sizeof(char) * (number_of_bytes + 1));
	if (NULL == buffer_write)
	{
		printf("memory allocation falied.\n");
		free(buffer_read);
		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}
		return FAILURE;
	}

	LPDWORD bytes_written = 0;
	BOOL ret_write = FALSE;
	
	ret_read = ReadFile(h_input_file, buffer_read, number_of_bytes, &bytes_read, NULL);
	if (FALSE == ret_read || bytes_read < number_of_bytes)
	{
		printf("FILE ERROR - ReadFile failed.\n");
		free(buffer_read);
		free(buffer_write);
		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}
		return FAILURE;
	}


	for (int i = 0; i < number_of_bytes; i++)
	{
		buffer_write[i] = operate_on_character(buffer_read[i], p_thread_data->key, p_thread_data->mode);
	}
	
	free(buffer_read);

	ret_write = WriteFile(h_output_file, buffer_write, number_of_bytes, bytes_written, NULL);

	free(buffer_write);

	if (FALSE == ret_write)
	{
		printf("FILE ERROR - WriteFile failed.\n");
		if (0 == CloseHandle(h_input_file))
		{
			printf("FILE ERROR - couldn't close input file. ABORT.\n");
		}

		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		return FAILURE;
	}

	if (p_thread_data->is_last)
	{
		if (0 == SetEndOfFile(h_output_file))
		{
			printf("SetEndOfFile failed. ABORT.\n");
			if (0 == CloseHandle(h_input_file))
			{
				printf("FILE ERROR - couldn't close input file. ABORT.\n");
			}

			if (0 == CloseHandle(h_output_file))
			{
				printf("FILE ERROR - couldn't close output file. ABORT.\n");
			}
			return FAILURE;
		}
	}


	if (0 == CloseHandle(h_input_file))
	{
		printf("FILE ERROR - couldn't close input file. ABORT.\n");
		if (0 == CloseHandle(h_output_file))
		{
			printf("FILE ERROR - couldn't close output file. ABORT.\n");
		}
		return FAILURE;
	}

	if (0 == CloseHandle(h_output_file))
	{
		printf("FILE ERROR - couldn't close output file. ABORT.\n");
		return FAILURE;
	}

	return SUCCESS;
}





