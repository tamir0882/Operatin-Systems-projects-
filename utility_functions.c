

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <errno.h>
#include "HardCodedData.h"




/* HANDLE create_file_for_read: a wrapper for CreateFileA.

* Parameters:
* LPCTR file_name - path for a file to open for reading.

* Return value:
* on success - a handle to a file with GENERIC_READ permission
* on failure - NULL
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

/* HANDLE create_file_for_write: a wrapper for CreateFileA.

* Parameters:
* LPCTR file_name - path for a file to create for writing.

* Return value:
* on success - a handle to a file with GENERIC_WRITE permission
* on failure - NULL
*/
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


/* HANDLE open_file_and_count_lines:

* Parameters:
* LPCTR file_name - path for a file to create for reading.
* int* count - pointer to an integer that saves the number of lines in the file.

* Return Value:
* on success - a handle to a file with GENERIC_READ permission
* on failure - NULL

* note:
* this function uses SetFilePointer to reset the file pointer back
	to the beginning of the file for later use.
*/
HANDLE open_file_and_count_lines(LPCSTR file_name, int* count_lines)
{
	*count_lines = MIN_LINES_NUMBER;

	HANDLE h_file = create_file_for_read(file_name);
	if (NULL == h_file)
		return NULL;

	char buffer = 0;
	LPDWORD bytes_read = 0;
	BOOL ret_read = FALSE;
	do
	{
		ret_read = ReadFile(h_file, &buffer, BUFFER_SIZE_ONE, &bytes_read, NULL);
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
			*count_lines = *count_lines + 1;
		}
	} while (TRUE);

	SetFilePointer(h_file, 0, NULL, FILE_BEGIN);
	return h_file;
}

/* void set_identical_data:

* Parameters:
* Data* array_of_thread_data - pointer to an array of struct Data.
* int number_of_threads - integer that is the number of threads to be created.
* int number_of_lines - number of lines in input file.
* char* input_file_name - path to an input file to read from.
* char* output_file_name - path to an output file to write to.
* char mode - either d or e. represents mode of operation - decrypting or encrypting.
* int key - key of decryption / encryption.
* HANDLE h_semaphore - handle to a semaphore that would be used to make all threads start at the same time.

* Return Value:
* does not return any value.

* note:
* the field is_last is the only field in this function which differs
	between the threads. it represents the thread that operate on the last
	line in the file.
*/
void set_identical_data(Data* array_of_thread_data, int number_of_threads, int number_of_lines, char* input_file_name,
	char* output_file_name, char mode, int key, HANDLE h_semaphore)
{
	for (int i = 0; i < number_of_threads; i++)
	{
		snprintf(array_of_thread_data[i].input_file_name, _MAX_PATH, "%s", input_file_name);
		snprintf(array_of_thread_data[i].output_file_name, _MAX_PATH, "%s", output_file_name);
		array_of_thread_data[i].mode = mode;
		array_of_thread_data[i].key = key;
		array_of_thread_data[i].is_last = FALSE;
		array_of_thread_data[i].semaphore = h_semaphore;
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

/* int set_indexes:
* Description - This function sets the start and end positions in the file for each thread.

* Parameters:
* HANDLE h_file - handle to a file with GENERIC_READ permission.
* Data* array_of_thread_data - pointer to an array of struct Data.
* int number_of_threads - integer that is the number of threads to be created.
* int lines_per_thread - number of lines from the file for each thread to operate on.
* int residual - in case where (number of lines)/(number of threads) is not a whole number
	this variable represents how many threads should be given an additional line to operate on.

* Return Value:
* on success - returns SUCCESS (integer value 0)
* on failure - returns FAILURE (integer value -1)
*/
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

/* int get_end_index:
* Description - This function finds the end positions in the file for a specific thread.

* Parameters:
* HANDLE h_file - handle to a file with GENERIC_READ permission.
* int lines_for_thread - number of lines from the file for each thread to operate on.
* int start_index - the start position of the thread.

* Return Value:
* on success - returns the end position of a thread (a positive integer).
* on failure - returns FAILURE (integer value -1),
*/
int get_end_index(HANDLE h_file, int lines_for_thread, int start_index)
{

	int remaining_lines = lines_for_thread;
	int count = -1;

	char buffer = 0;
	LPDWORD bytes_read = 0;
	int ret_read = 0;
	while (remaining_lines > 0)
	{
		ret_read = ReadFile(h_file, &buffer, BUFFER_SIZE_ONE, &bytes_read, NULL);
		if (FALSE == ret_read)
		{
			printf("FILE ERROR - ReadFile failed.\n");
			return FAILURE;
		}
		if (0 == bytes_read)
		{
			break; //reached end of file.
		}

		count++;
		if (buffer == '\n')
			remaining_lines--;
	} 

	return count + start_index;
}

/* char operate_on_character_help:
* Description - helper for operate_on_character.

* Parameters:
* char lower_end - lower end of range of relevant characters - either 'a', 'A' or '0'.
* char ch_read - the character that was recently read by ReadFile.
* int key - key for decryption / encryption.
* int base - base of ch_read - either DECIMAL_BASE(10) or LETTER_BASE(26).

* Return Value:
* returns the decrypted / encrypted characte.
*/
char operate_on_character_help(char lower_end, char ch_read, int key, int base)
{
	char temp = ch_read - key - lower_end;
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

/* char operate_on_character:
* Description - helper for operate_on_character.

* Parameters:
* char ch_read - the character that was recently read by ReadFile.
* int key - key for decryption / encryption.
* int base - base of ch_read - either DECIMAL_BASE(10) or LETTER_BASE(26).

* Return Value:
* returns the decrypted / encrypted character.
*/
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

/* HANDLE create_thread_simple:
* Description - creates a thread using CreateThread.

* Parameters:
* LPTHREAD_START_ROUTINE p_start_routine - the start index of the thread in file.
* LPDWORD p_thread_id - thread's id to use in CreateThread function.
* Data* thread_data - pointer to an array of struct Data containg each thread's data.

* Return Value:
* returns a handle to the thread.
*/
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

