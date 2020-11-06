//The function count_fire gets a string pointer as input and returns the burnt trees' number.
//Counts the number of 'F' letters inside a given forest string to find the number of burnt trees.

int count_fire(const char* forest)
{

	int burnt_trees = 0;
	while (*forest != '\0')
		//While loop is finished when the string ends when we get to '\0'.
	{
		//If found a burnt tree, update the counter by 1 and continue the loop.
		if (*forest == 'F' || *forest == 'f')
		{
			//Update the counter of burnt trees.			
			burnt_trees++;
		}
		//Update the loop counter.
		forest++;
	}
	return burnt_trees;
}

