#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

HANDLE create_file_for_read(LPCSTR file_name);
HANDLE create_file_for_write(LPCSTR file_name);


HANDLE open_file_and_count_lines(LPCSTR file_name, int* count_lines);

int get_end_index(HANDLE h_file, int lines_for_thread, int offset);
int set_indexes(HANDLE h_file, Data* thread_data, int number_of_threads, int lines_per_thread, int residual);
void set_identical_data(Data* array_of_thread_data, int number_of_threads, int number_of_lines, char* input_file_name,
	char* output_file_name, char mode, int key, HANDLE semaphore);

char operate_on_character(char ch_read, int key, char mode);

HANDLE create_thread_simple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, Data* thread_data);

#endif // UTILITY_FUNCTIONS_H