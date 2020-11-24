#pragma once

HANDLE create_file_for_read(LPCSTR file_name);
HANDLE create_file_for_write(LPCSTR file_name);


HANDLE open_file_and_count_lines(LPCSTR file_name, int* count);

int get_end_index(HANDLE h_file, int lines_for_thread, int offset);
int set_indexes_for_threads(int* start_index, int* end_index, int number_of_threads, int lines_per_thread, int residual, HANDLE h_file);

char decrypt_character(char ch_read, int key);