
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

	if (wait_code_semaphore != WAIT_OBJECT_0)
	{
		printf("SEMAPHORE ERROR - wait code is not WAIT_OBJECT_0.\n ABORT.\n");
		return FAILURE;
	}


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
		return FAILURE;
	}
	//---------------------------------------------************************

	if (INVALID_SET_FILE_POINTER == SetFilePointer(h_input_file, p_thread_data->start_index, NULL, FILE_BEGIN))
	{
		printf("SetFilePointer failed. ABORT");
		return FAILURE;
	}

	if (INVALID_SET_FILE_POINTER == SetFilePointer(h_output_file, p_thread_data->start_index, NULL, FILE_BEGIN))
	{
		printf("SetFilePointer failed. ABORT");
		return FAILURE;
	}
	
	char buffer_read = 0;
	LPDWORD bytes_read = 0;
	BOOL ret_read = FALSE;
	
	char buffer_write = 0;
	LPDWORD bytes_written = 0;
	BOOL ret_write = FALSE;
	
	int count = p_thread_data->start_index;
	

	do
	{
		ret_read = ReadFile(h_input_file, &buffer_read, MAX_BUFFER_SIZE, &bytes_read, NULL);
		if (FALSE == ret_read)
		{
			printf("FILE ERROR - ReadFile failed.\n");
			return FAILURE;
		}

		count++;

		
		buffer_write = operate_on_character(buffer_read, p_thread_data->key, p_thread_data->mode);

		ret_write = WriteFile(h_output_file, &buffer_write, 1, bytes_written, NULL);
		if (FALSE == ret_write)
		{
			printf("FILE ERROR - WriteFile failed.\n");
			return FAILURE;
		}
	} while (count <= p_thread_data->end_index);


	if (p_thread_data->is_last)
	{
		if (0 == SetEndOfFile(h_output_file))
		{
			printf("SetEndOfFile failed. ABORT.\n");
			return FAILURE;
		}
	}


	if (0 == CloseHandle(h_input_file))
	{
		printf("FILE ERROR - couldn't close input file. ABORT.\n");
		return FAILURE;
	}

	if (0 == CloseHandle(h_output_file))
	{
		printf("FILE ERROR - couldn't close output file. ABORT.\n");
		return FAILURE;
	}

	return SUCCESS;
}





