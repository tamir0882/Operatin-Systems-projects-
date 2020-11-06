// Authors: Yehonatan Engel - 315233650, Tamir Dray - 318852738.
// Project: Son project in Homework #1 in course "Introduction to Operating Systems".
// Description:
#include "SonUtility.h"
#include <stdio.h>

int main(int argc, char** argv) {
	if (argc != 2)
	{
		printf("Error, one argument expected.\n");
		return -1;
	}
	//initialize inputs and local parameters.
	return count_fire(argv[1]);
}
	
	
	  

	