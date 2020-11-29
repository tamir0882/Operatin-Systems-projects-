#ifndef HARD_CODED_DATA_H
#define HARD_CODED_DATA_H

#define MAX_BUFFER_SIZE 1


#define FAILURE -1
#define SUCCESS 0

#define EXPECTED_ARGC 5

#define READ "r"
#define WRITE "w"

#define OUTPUT_FILE_NAME "output.txt"

#define INPUT_FILE_PATH_POS 1
#define KEY_POS 2
#define KEY_FIRST_DIGIT 0
#define NUMBER_OF_THREADS_POS 3
#define MODE_POS 4
#define MODE_SUB_POS 1
#define INITIALIZED_LENGTH 10

#define MIN_LINES_NUMBER 1

#define WAIT_TIME 5000 //wait 5 seconds

#define LETTER_BASE 26
#define DECIMAL_BASE 10


typedef struct Data
{
	char input_file_name[_MAX_PATH];
	char output_file_name[_MAX_PATH];
	int start_index, end_index;
	int key;
	char mode;
	BOOL is_last;
	HANDLE semaphore;
}Data;



#endif // HARD_CODED_DATA_H


