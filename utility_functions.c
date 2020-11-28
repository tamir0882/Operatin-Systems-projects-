#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <errno.h>
#include "HardCodedData.h"

/*
char* buffer = "Hello World!\n";
DWORD bytes_to_write = 13;
LPDWORD bytes_written;

WriteFile(h_file, buffer, bytes_to_write, &bytes_written, NULL);
return h_file;
*/


HANDLE create_file_for_read(LPCSTR file_name)
{
	if (NULL == file_name)
	{
		printf("ARG ERROR - NULL pointer was given. ABORT.\n");
		return NULL;
	}

	HANDLE h_file = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (ERROR_FILE_NOT_FOUND == GetLastError() || INVALID_HANDLE_VALUE == h_file)
	{
		printf("FILE ERROR - could not open file. ABORT.\n");
		return NULL;
	}
	return h_file;
}


HANDLE create_file_for_write(LPCSTR file_name)
{
	if (NULL == file_name)
	{
		printf("ARG ERROR - NULL pointer was given. ABORT.\n");
		return NULL;
	}

	HANDLE h_file = CreateFileA(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (ERROR_FILE_NOT_FOUND == GetLastError() || INVALID_HANDLE_VALUE == h_file)
	{
		printf("ERROR - could not open file. ABORT\n");
		return NULL;
	}
	return h_file;
}


HANDLE open_file_and_count_lines(LPCSTR file_name, int* count)
{
	*count = MIN_LINES_NUMBER;

	HANDLE h_file = create_file_for_read(file_name);
	if (NULL == h_file)
		return NULL;
	
	char buffer = 0;
	LPDWORD bytes_read = 0;
	BOOL ret_read = FALSE;
	do
	{
		ret_read = ReadFile(h_file, &buffer, MAX_BUFFER_SIZE, &bytes_read, NULL);
		if (FALSE == ret_read)
		{
			printf("FILE ERROR - ReadFile failed.\n");
			return NULL;
		}
		if (0 == bytes_read)
		{
			break; //reached end of file.
		}

		if (buffer == '\n')
		{
			*count = *count + 1;
		}
	} while (TRUE);

	SetFilePointer(h_file, 0, NULL, FILE_BEGIN);
	return h_file;
}


void set_same_data(Data* array_of_thread_data,int number_of_threads, int number_of_lines, char* input_file_name, 
	char* output_file_name, char mode, int key, HANDLE h_semaphore, HANDLE h_mutex)
{
	for (int i = 0; i < number_of_threads; i++)
	{
		snprintf(array_of_thread_data[i].input_file_name, _MAX_PATH, "%s", input_file_name);
		snprintf(array_of_thread_data[i].output_file_name, _MAX_PATH, "%s", output_file_name);
		array_of_thread_data[i].mode = mode;
		array_of_thread_data[i].key = key;
		array_of_thread_data[i].is_last = FALSE;
		array_of_thread_data[i].semaphore = h_semaphore;
		array_of_thread_data[i].mutex = h_mutex;
	}
	if (number_of_threads > number_of_lines)
	{
		array_of_thread_data[number_of_lines - 1].is_last = TRUE;
	}
	else
	{
		array_of_thread_data[number_of_threads - 1].is_last = TRUE;
	}
}


int set_indexes(HANDLE h_file, Data* array_of_thread_data, int number_of_threads, int lines_per_thread, int residual)
{
	array_of_thread_data[0].start_index = 0;
	for (int i = 0; i < number_of_threads; i++)
	{
		if (residual > 0)
		{
			if ((array_of_thread_data[i].end_index = get_end_index(h_file, lines_per_thread + 1, array_of_thread_data[i].start_index)) == FAILURE)
				return FAILURE;
			residual--;
		}
		else
			if ((array_of_thread_data[i].end_index = get_end_index(h_file, lines_per_thread, array_of_thread_data[i].start_index)) == FAILURE)
				return FAILURE;

		if (i + 1 < number_of_threads)
			array_of_thread_data[i + 1].start_index = array_of_thread_data[i].end_index + 1;
	}
	return SUCCESS;
}


int get_end_index(HANDLE h_file, int lines_for_thread, int offset)
{

	int remaining_lines = lines_for_thread;
	int count = -1;

	char buffer = 0;
	LPDWORD bytes_read = 0;
	int ret_read = 0;
	do
	{
		ret_read = ReadFile(h_file, &buffer, MAX_BUFFER_SIZE, &bytes_read, NULL);
		if (FALSE == ret_read)
		{
			printf("FILE ERROR - ReadFile failed. ABORT\n");
			return FAILURE;
		}
		if (0 == bytes_read)
		{
			break; //reached end of file.
		}

		count++;
		if (buffer == '\n')
			remaining_lines--;
	} while (remaining_lines > 0);

	return count + offset;
}


char operate_on_character_help(char lower_end, int ch_read, int key, int base)
{
	int temp = ch_read - key - lower_end;
	if (temp < 0)
	{
		temp += base;
	}
	return lower_end + temp;
}

int set_key(int key, int base)
{
	int res = key % base;
	if (res < 0)
	{
		res += base;
	}
	return res;
}

char operate_on_character(char ch_read, int key, char mode)
{
	if (mode == 'e') key = -key;

	int letter_key = set_key(key, LETTER_BASE);
	int number_key = set_key(key, DECIMAL_BASE);

	if ('a' <= ch_read && ch_read <= 'z')
	{
		return operate_on_character_help('a', ch_read, letter_key, LETTER_BASE);
	}
		
	else if ('A' <= ch_read && ch_read <= 'Z')
	{
		return operate_on_character_help('A', ch_read, letter_key, LETTER_BASE);
	}
		
	else if ('0' <= ch_read && ch_read <= '9')
	{
		return operate_on_character_help('0', ch_read, number_key, DECIMAL_BASE);
	}
		
	return ch_read;
}


HANDLE create_thread_simple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, Data* thread_data)
{
	HANDLE thread_handle;

	if (NULL == p_start_routine || NULL == p_thread_id)
	{
		printf("THREAD ERROR - Received null pointer.\n");
		return NULL;
	}

	thread_handle = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		(LPTHREAD_START_ROUTINE)p_start_routine, /*  thread function */
		thread_data,     /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("THREAD ERROR - CreateThread failed.\n");
		return NULL;
	}

	return thread_handle;
}

