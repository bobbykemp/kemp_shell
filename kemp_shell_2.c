#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
	char curr_direc[256], cmd[256], my_file[256], target_direc[256];
	DIR *d;
	struct dirent *dir;
	struct stat st;
	int list_count, target, k, i, num, file_size, file_name_len;
	pid_t child_process_id;
	time_t t;

	char file_names[256][256];
	int file_sizes[256];
	char dir_names[256][256];

	system(chdir(argv[1]));

	while(1){

		printf("\n-------------------------------------\n");

		printf("----------K E M P S H E L L----------\n\n");

		t = time(NULL);
		printf("The current time is : %s", ctime(&t));

		getcwd(curr_direc, 200);
		printf("\nCurrent working directory: %s\n", curr_direc);

		printf("Contents of current working directory:\n");
		
		/*d = opendir(".");
		list_count = 0;*/

		printf("\nE/e - Edit; R/r - Run; C/c - Change Directory\n");

		/*printf("\n=====================================\n");
		printf("DIRECTORIES:\n");
		printf("=====================================\n");

		while ((dir = readdir(d)) != NULL) {
			if((dir->d_type) & DT_DIR){
				printf("%d   %s \n", list_count, dir->d_name);
				strcpy(dir_names[list_count++], dir->d_name);
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

				stat(dir->d_name, &st);
				file_size = st.st_size;
				printf("%-5d%-*sSize (bytes): %d\n", list_count, file_name_len, dir->d_name, file_size);
				strcpy(file_names[list_count], dir->d_name);
				strcpy(file_sizes[list_count++], file_size);
			}
			if (list_count % 8 == 0) { 
				printf("Hit N for Next\n");
				k = getchar();
			}
		}

		closedir(d);*/

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
			execl("/bin/ls", "ls", "-a", "-l", "--sort=size", (char *)NULL);

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
				strcat(target_direc, file_names[num]);

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
					strcat(target_direc, file_names[num]);

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

				strcat(curr_direc, "/");
				printf("Change to which directory?:\n");
				scanf("%d", &num);
				strcat(curr_direc, dir_names[num]);
				printf("Changing to directory: %s\n", curr_direc);
				system(chdir(curr_direc));

				break;
		}
	}

	return 0;
}