#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define PRINT_GROUP_SIZE 4
#define MAX_IND 1024

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
  char curr_direc[256], cmd[256], my_file[256], target_direc[256], sort_type, sort_direction, buff, run_params, name[256];
  DIR *d;
  struct dirent *dir;
  struct stat st;
  line *loc;
  int list_count, target, k, i, num, file_name_len, sort, print_to;
  pid_t child_process_id;
  time_t t;

  line *files = (line*) malloc(MAX_IND * sizeof(*files));
  line *dirs  = (line*) malloc(MAX_IND * sizeof(*dirs));

  system((const char *)chdir(argv[1]));

  print_to = 0;

  while(1){

    t = time(NULL);
    printf("The current time is : %s", ctime(&t));

    //TODO: Account for longer directory names

    // printf("print to is %d\n", print_to);

    getcwd(curr_direc, 200);
    printf("\nCurrent working directory: %s\n", curr_direc);

    printf("Contents of current working directory:\n");
    
    //retrieve access to the current directory - "."

    //TODO: Catch errors from opendir
    d = opendir(".");
    list_count = 0;

    printf("\nE/e - Edit; R/r - Run; C/c - Change Directory; Q/q - Quit; S/s - Sort File Listings\n");

    printf("\nDIRECTORIES:\n");
    printf("\n------------\n");

    while ((dir = readdir(d)) != NULL) {
      if((dir->d_type) & DT_DIR){
        if(list_count > MAX_IND){
          dirs = (line*) realloc(dirs, (2 * sizeof(line)));
        }
        printf("%d   %s \n", list_count, dir->d_name);
        strcpy(dirs[list_count++].name, dir->d_name);
      }
    }

    closedir(d);

    d = opendir(".");
    list_count = 0;

    printf("\nFILES:");
    printf("\n------\n");

    file_name_len = 0;
    
    while ((dir = readdir(d)) != NULL) {
      if((dir->d_type) & DT_REG){

        if(list_count > MAX_IND){
          files = (line*) realloc(files, (2 * sizeof(line)));
        }

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
      }
    }

    /*int answer = num_files / 4;

    printf("num file is %d\n", num_files);

    printf("numfiles divided by 4 is %d\n", answer);*/

    //has the user requested a sort?
    if(sort){
      if(sort_type == 't' && sort_direction == 'a'){
        qsort(files, list_count, sizeof(line), compare_time_asc);
      }

      else if(sort_type == 't' && sort_direction == 'd'){
        qsort(files, list_count, sizeof(line), compare_time_dsc);
      }

      else if(sort_type == 's' && sort_direction == 'a'){
        qsort(files, list_count, sizeof(line), compare_size_asc);
      }

      else if(sort_type == 's' && sort_direction == 'd'){
        qsort(files, list_count, sizeof(line), compare_size_dsc);
      }

      else{
        printf("Critical sort error, exiting\n");
        exit(0);
      }
    }
    else{
      sort = 0; //false
    }

    printf("List count is %d\n", list_count);
    printf("Print to is %d\n", print_to);

    int num_div = list_count / PRINT_GROUP_SIZE;
    int leftovers = list_count % PRINT_GROUP_SIZE;

    if(print_to == list_count){
      print_to = 0;
    }

    //after we've sorted (if necessary) we can now print for the user
    for (int i = print_to; i < list_count; ++i)
    {
      char *s;
      s = ctime(&files[i].time);

      printf("%-3d%-*sSize (bytes): %-8d%-24s\n", i, file_name_len, files[i].name, files[i].size, s);

      if(i % 4 == 0 && i != 0){
        printf("Press enter to get more...\n");
        print_to = ++i;

        break;
      }

      if( (i == list_count - 1) || (i == list_count) ){
          printf("Press enter to cycle the list\n");
          print_to = 0;

          break;
      }

    }


    closedir(d);

    printf("-------------------------------------\n");

    list_count = getchar();

    switch (list_count){

      case 'q':
      case 'Q':

        exit(0);

      case 'e':
      case'E':

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
      case 'R':

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

          do{
            printf("Parameters (y/n)?:\n");
            scanf("%s", &run_params); 
          }while(run_params != 'y' && run_params != 'n');

          if(run_params == 'y'){
            execl(target_direc, target_direc, run_params, (char *)NULL); 
          }
          else{
            execl(target_direc, target_direc, (char *)NULL);
          }

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

      case 'c':
      case 'C':

      //TODO: Ensure directory exists, ensure user has permissions
      //to this directory

        strcat(curr_direc, "/");
        printf("Change to which directory?:\n");
        scanf("%d", &num);
        strcat(curr_direc, dirs[num].name);
        printf("Changing to directory: %s\n", curr_direc);
        system(chdir(curr_direc));

        break;

      case 'm':
      case 'M':

        strcpy(target_direc, curr_direc);
        strcat(target_direc, "/");
        printf("Move which file?:\n");
        scanf("%d", &num);
        strcpy(name, files[num].name);
        strcat(target_direc, name);

        strcat(curr_direc, "/");
        printf("Move file to which directory?:\n");
        scanf("%d", &num);
        strcat(curr_direc, dirs[num].name);
        printf("Moving to directory: %s\n", curr_direc);
        strcat(curr_direc, "/");
        strcat(curr_direc, name);
        rename(target_direc, curr_direc);

        break;

      case 's':
      case 'S':

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
  free(files);
  free(dirs);
}