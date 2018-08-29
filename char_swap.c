#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

void char_swap(char *p1, char *p2, int n){
	char temp[n];
	strcpy(temp, p1);
	strcpy(p1, p2);
	strcpy(p2, temp);
}

void int_swap(int *p1, int *p2){
	int temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

void bubble_sort(int main_arr[], char* dep_arr[], int arrlen){
	for (int i = 0; i < arrlen - 1; ++i)
	{
		for (int j = 0; j < arrlen - i - 1; ++j)
		{
			if(main_arr[j] > main_arr[j+1]){
				int_swap(&main_arr[j], &main_arr[j+1]);
				char_swap(&dep_arr[j], &dep_arr[j+1], arrlen);
			}
		}
	}
}

int main(int argc, char const *argv[])
{
	char words[256][256];
	int numbers[256];

	numbers[0] = 10;
	numbers[1] = 1;

	strcpy(words[0], "candy");
	strcpy(words[1], "cane");

	/*for (int i = 0; i < sizeof(words); ++i)
	{
		printf("%s %d\n", words[i], numbers[i]);
	}

	bubble_sort(numbers, &words, sizeof(numbers));
	
	for (int i = 0; i < sizeof(words); ++i)
	{
		printf("%s %d\n", words[i], numbers[i]);
	}*/

	printf("%s\n", &words);

	return 0;
}