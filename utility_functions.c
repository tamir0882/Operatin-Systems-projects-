#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <errno.h>
#include "HardCodedData.h"


HANDLE create_file_for_read(LPCSTR file_name)
{
	HANDLE h_file = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (ERROR_FILE_NOT_FOUND == GetLastError() || INVALID_HANDLE_VALUE == h_file)
	{
		printf("ERROR - could not open file. ABORT");
		return NULL;
	}
	return h_file;
}


HANDLE create_file_for_write(LPCSTR file_name)
{
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
	*count = 1;

	HANDLE h_file = create_file_for_read(file_name);
	if (NULL == h_file)
		return NULL;

	char buffer[MAX_BUFFER_SIZE] = { 0 };
	LPDWORD bytes_read = 0;
	int ret_read = 0;
	do
	{
		ret_read = ReadFile(h_file, buffer, MAX_BUFFER_SIZE - 1, &bytes_read, NULL);
		if (FALSE == ret_read)
		{
			printf("FILE ERROR - ReadFile failed.\n");
			return NULL;
		}
		if (0 == bytes_read)
		{
			break; //reached end of file.
		}
		
		buffer[MAX_BUFFER_SIZE - 1] = '\0';

		if (buffer[0] == '\n')
		{
			*count = *count + 1;
		}
	} while (TRUE);
	SetFilePointer(h_file, 0, NULL, FILE_BEGIN);
	return h_file;
}


int get_end_index(HANDLE h_file, int lines_for_thread, int offset)
{
	
	int remaining_lines = lines_for_thread;
	int count = -1;
	
	char buffer[MAX_BUFFER_SIZE] = { 0 };
	LPDWORD bytes_read = 0;
	int ret_read = 0;
	do
	{
		ret_read = ReadFile(h_file, buffer, MAX_BUFFER_SIZE - 1, &bytes_read, NULL);
		if (FALSE == ret_read)
		{
			printf("FILE ERROR - ReadFile failed. ABORT\n");
			return FAILURE;
		}
		if (0 == bytes_read)
		{
			break; //reached end of file.
		}

		buffer[MAX_BUFFER_SIZE - 1] = '\0';
		count++;
		if (buffer[0] == '\n')
			remaining_lines--;
	} while (remaining_lines > 0);

	return count + offset;
}


int set_indexes_for_threads(int* start_index, int* end_index, int number_of_threads, int lines_per_thread, int residual, HANDLE h_file)
{
	start_index[0] = 0;
	for (int i = 0; i < number_of_threads; i++)
	{
		if (residual > 0)
		{
			if ((end_index[i] = get_end_index(h_file, lines_per_thread + 1, start_index[i])) == FAILURE)
				return FAILURE;
			residual--;
		}
		else
			if ((end_index[i] = get_end_index(h_file, lines_per_thread, start_index[i])) == FAILURE)
				return FAILURE;


		if (i + 1 < number_of_threads)
			start_index[i + 1] = end_index[i] + 1;
	}
}


char operate_on__character(char ch_read, int key, char flag)
{
	if (flag == 'e') key = -key;

	char c = ch_read;
	if ('a' <= ch_read <= 'z')
		c = 'a' + (ch_read - 'a' - key) % 26;
	else if ('A' <= ch_read <= 'Z')
		c = 'A' + (ch_read - 'A' - key) % 26;
	else if ('0' <= ch_read <= '9')
		c = '0' + (ch_read - '0' - key) % 10;
	return c;
}


/*
char* buffer = "Hello World!\n";
DWORD bytes_to_write = 13;
LPDWORD bytes_written;

WriteFile(h_file, buffer, bytes_to_write, &bytes_written, NULL);
return h_file;
*/