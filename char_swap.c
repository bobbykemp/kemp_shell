#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_CAP 2056

typedef struct line_item{
	char name[1024];
	int size;
}line;

int compare(const void *x, const void *y){

	int a = ((struct line_item *)x)->size;
	int b = ((struct line_item *)y)->size;

	return a - b;
}

int main(int argc, char const *argv[])
{
	line *data = malloc(MAX_CAP * sizeof(*data));

	strcpy(data[0].name, "Hello");
	strcpy(data[1].name, "World");

	data[0].size = 1;
	data[1].size = 0;

	for (int i = 0; i < 2056; ++i)
	{
		printf("%s %d\n", data[i].name, data[i].size);
	}

	printf("\n");

	qsort(data, 2056, sizeof(line), compare);

	for (int i = 0; i < 2056; ++i)
	{
		printf("%s %d\n", data[i].name, data[i].size);
	}

	printf("Sorting %ld entries in the array\n", 2056);
	printf("Each entry has a size of %ld\n", sizeof(line));

	// free(data);
	return 0;
}