#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct line_item{
	char name[1024];
	int size;
	time_t time;
}line;

int compare_size_asc(const void *x, const void *y){

	int a = ((line *)x)->size;
	int b = ((line *)y)->size;

	return a - b;
}

int compare_size_dsc(const void *x, const void *y){

	int a = ((line *)x)->size;
	int b = ((line *)y)->size;

	return b - a;
}

int compare_time_asc(const void *x, const void *y){

	time_t a = ((line *)x)->time;
	time_t b = ((line *)y)->time;

	return a - b;
}

int compare_time_dsc(const void *x, const void *y){

	time_t a = ((line *)x)->time;
	time_t b = ((line *)y)->time;

	return b - a;
}

int main(int argc, char const *argv[])
{
	char curr_direc[256], cmd[256], my_file[256], target_direc[256], sort_type, sort_direction;
	DIR *d;
	struct dirent *dir;
	struct stat st;
	int list_count, target, k, i, num, file_name_len, num_files, sort;
	pid_t child_process_id;
	time_t t;

	line files[256];
	line dirs[256];

	system(chdir(argv[1]));

	while(1){

		num_files = 0;

		printf("\n-------------------------------------\n");

		printf("----------K E M P S H E L L----------\n\n");

		t = time(NULL);
		printf("The current time is : %s", ctime(&t));

		//TODO: Account for longer directory names

		getcwd(curr_direc, 200);
		printf("\nCurrent working directory: %s\n", curr_direc);

		printf("Contents of current working directory:\n");
		
		//retrieve access to the current directory - "."

		//TODO: Catch errors from opendir
		d = opendir(".");
		list_count = 0;

		printf("\nE/e - Edit; R/r - Run; C/c - Change Directory\n");

		printf("\n=====================================\n");
		printf("DIRECTORIES:\n");
		printf("=====================================\n");

		while ((dir = readdir(d)) != NULL) {
			if((dir->d_type) & DT_DIR){
				printf("%d   %s \n", list_count, dir->d_name);
				strcpy(dirs[list_count++].name, dir->d_name);
			}
		}

		closedir(d);

		d = opendir(".");
		list_count = 0;

		printf("\n=====================================\n");
		printf("FILES:\n");
		printf("=====================================\n");

		file_name_len = 0;
		
		while ((dir = readdir(d)) != NULL) {
			if((dir->d_type) & DT_REG){

				if(strlen(dir->d_name) > file_name_len){
					file_name_len = strlen(dir->d_name) + 2;
				}

				//get some status info of the files we pull
				//including size and time of last status change
				stat(dir->d_name, &st);

				//save each file name to a struct object in our file array
				strcpy(files[list_count].name, dir->d_name);

				//save each file size to a struct object in our file array
				files[list_count].size = st.st_size;

				//save each file time of last edit to struct in file array
				files[list_count++].time = st.st_mtime;

				//increment our total number of files
				num_files++;
			}
			/*if (list_count % 8 == 0) { 
				printf("Hit N for Next\n");
				k = getchar();
			}*/
		}

		//has the user requested a sort?
		if(sort){
			if(sort_type == 't' && sort_direction == 'a'){
				qsort(files, num_files, sizeof(line), compare_time_asc);
			}

			else if(sort_type == 't' && sort_direction == 'd'){
				qsort(files, num_files, sizeof(line), compare_time_dsc);
			}

			else if(sort_type == 's' && sort_direction == 'a'){
				qsort(files, num_files, sizeof(line), compare_size_asc);
			}

			else if(sort_type == 's' && sort_direction == 'd'){
				qsort(files, num_files, sizeof(line), compare_size_dsc);
			}
			
			else{
				printf("Critical sort error, exiting\n");
				exit(0);
			}
		}
		else{
			sort = 0; //false
		}

		//after we've sorted (if necessary) we can now print for the user
		for (int i = 0; i < list_count; ++i)
		{
			char *s;
			s = ctime(&files[i].time);
			printf("%-3d%-*sSize (bytes): %-10d%-24s\n", i, file_name_len, files[i].name, files[i].size, s);
		}


		closedir(d);

		printf("-------------------------------------\n");

		list_count = getchar();

		switch (list_count){
			case 'q':
				exit(0);
			case 'e':

				strcpy(target_direc, curr_direc);
				strcat(target_direc, "/");
				printf("Edit what?:\n");
				scanf("%d", &num);
				strcat(target_direc, files[num].name);

				//fork into parent and child processes
				child_process_id = fork();

				//a negative (-) process id indicates a fork failure
				if(child_process_id < 0){
					perror("fork failed");
					exit(1);
				}

				//a process id of zero (0) indicates this is the child process
				//which is where we want to run pico
				else if(child_process_id == 0){

					//Run pico editor with execl(_path to pico_, _path to pico_, _path to file to edit_, _NULL pointer_)
					execl("/usr/bin/pico", "/usr/bin/pico", target_direc, (char *)NULL);

					//If execl throws an error, let us know about it
					perror("execl() has failed\n");

					//exit
					_exit(1);
				}

				//a positive (+) process id indicates this is the parent process
				//which we want to have wait for the child process to exit
				else if(child_process_id > 0){
					wait(NULL);
				}

				break;

			case 'r':

				//fork into parent and child processes
				child_process_id = fork();

				//a negative (-) process id indicates a fork failure
				if(child_process_id < 0){
					perror("fork failed");
					exit(1);
				}

				//a process id of zero (0) indicates this is the child process
				//which is where we want to run pico
				else if(child_process_id == 0){

					strcpy(target_direc, curr_direc);
					strcat(target_direc, "/");
					printf("Run what?:\n");
					scanf("%d", &num);
					strcat(target_direc, files[num].name);

					//Run pico editor with execl(_path to pico_, _path to pico_, _path to file to edit_, _NULL pointer_)
					execl(target_direc, 0, (char *)NULL);

					//If execl throws an error, let us know about it
					perror("execl() has failed\n");

					//exit
					_exit(1);
				}

				//a positive (+) process id indicates this is the parent process
				//which we want to have wait for the child process to exit
				else if(child_process_id > 0){
					wait(NULL);
				}

				break;

			case 'm':

			//TODO: Ensure directory exists, ensure user has permissions
			//to this directory

				strcat(curr_direc, "/");
				printf("Change to which directory?:\n");
				scanf("%d", &num);
				strcat(curr_direc, dirs[num].name);
				printf("Changing to directory: %s\n", curr_direc);
				system(chdir(curr_direc));

				break;

			case 's':

				sort_type = 't';
				sort_direction = 'a';

				do{
					printf("t for time; s for size:\n");
					scanf("%s", &sort_type);
				}
				while (sort_type != 't' && sort_type != 's');

				do{
					printf("a for ascending sort d for descending:\n");
					scanf("%s", &sort_direction);
				}
				while (sort_direction != 'a' && sort_direction != 'd');

				sort = 1; //true

				break;
		}
	}

	return 0;
}