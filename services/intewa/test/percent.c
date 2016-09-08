#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void main (void) {

	int min = 0;
	int max = 1023;

	int level_value = 50;


	int level_percent = (((level_value - min) * 1000U) / (max - min));
 
	printf("\n%d\n", level_percent);

}