//Robert J.T. Kemp
//CSE 3320-001
//Lab 1

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
#include <errno.h>

//number of structs per array for files and dirs
#define MAX_IND 1024

//number of lines output per press of the enter key
#define PER_LINE 4

typedef struct line_item{
  char name[2048];
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
  char curr_direc[256], target_direc[256], sort_type, sort_direction, run_params, name[256];
  DIR *d;
  struct dirent *dir;
  struct stat st;
  int list_count, target, k, i, num, file_name_len, sort, print_to;
  pid_t child_process_id;
  time_t t;

  //Dynamically allocate arrays of structs that will
  //represent files, directories and their details
  line *files = (line*) malloc(MAX_IND * sizeof(line));
  line *dirs  = (line*) malloc(MAX_IND * sizeof(line));

  //Check if allocation above was successful for both arrays; if not, exit
  if(files == NULL || dirs == NULL){
    printf("Cannot allocate intial memory\n");
    exit(-1);
  }

  //User is allowed to pass the desired starting directory to the program
  //from the command line. This statement changes the active directory to
  //the arg passed in by the user
  system((const char *)chdir(argv[1]));

  //print_to will tell us what index to start printing at when we
  //print file info to the user below, allowing us to only display
  //a few lines at a time to keep things simple
  print_to = 0;

  //did the user ask for the information outputted to be sorted?
  sort = 0;

  //main operational loop
  while(1){

    t = time(NULL);
    printf("The current time is : %s", ctime(&t));

    //check for error in call to time() and exit if there is one
    if(t == -1){
      printf("Time acquirement failed\n");
      exit(-1);
    }

    //get the text associated with the cwd, and if that fails then
    //say so and exit
    if(getcwd(curr_direc, 1024) == NULL){
      printf("Could not get current working directory. Exiting.\n");
      exit(-1);
    }

    printf("\nCurrent working directory: %s\n", curr_direc);

    printf("Contents of current working directory:\n");

    d = opendir(".");

    //Exit if opendir() fails
    if(d == NULL){
      printf("Could not open directory. Exiting.\n");
      exit(-1);
    }

    //the index of the file/directory in the files/dirs array
    list_count = 0;

    //user options
    printf("\nE/e - Edit; R/r - Run; Q/q - Quit\n"
           "C/c - Change Directory\n"
           "S/s - Sort File Listings\n"
           "M/m - Move File to New Directory\n"
           "D/d - Delete File\n");

    printf("\nDIRECTORIES:");
    printf("\n------------\n");

    //look at every directory in the current directory
    while ((dir = readdir(d)) != NULL) {
      if((dir->d_type) & DT_DIR){

        //print this info to the user
        printf("%d   %s \n", list_count, dir->d_name);

        //copy this info into our arrays for later use
        strcpy(dirs[list_count++].name, dir->d_name);
      }
    }

    closedir(d);

    d = opendir(".");
    list_count = 0;

    printf("\nFILES:");
    printf("\n------\n");

    //this will allow us to dynamically change the length of our padding
    //in printf
    file_name_len = 0;
    
    while ((dir = readdir(d)) != NULL) {
      if((dir->d_type) & DT_REG){

        //Can't get realloc to work here and I'm not sure why,

        /*if(list_count == MAX_IND){
          files = realloc(files, (MAX_IND + 1) * sizeof(line));
        }*/

        //accomodate for the longest name in the directory and adjust
        //the padding for printf below accordingly
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
        exit(-1);
      }
    }
    else{
      sort = 0; //false
    }

    //if the index we start at is the same as the index we end at,
    //reset the starting index
    if(print_to == list_count){
      print_to = 0;
    }

    //after we've sorted (if necessary) we can now print for the user
    for (int i = print_to; i < list_count; ++i)
    {
      char *s;
      s = ctime(&files[i].time);

      printf("%-3d%-*sSize (bytes): %-8d%-24s\n", i, file_name_len, files[i].name, files[i].size, s);

      if(i % PER_LINE == 0 && i != 0){
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

      //quit the shell
      case 'q':
      case 'Q':

        exit(0);

      //edit a file
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

      //run a program
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

      //change to a different directory
      case 'c':
      case 'C':

        //get user input, store in appropriate buffers
        strcat(curr_direc, "/");
        printf("Change to which directory?:\n");
        scanf("%d", &num);
        strcat(curr_direc, dirs[num].name);
        printf("Changing to directory: %s\n", curr_direc);

        //carry out the chdir operation and check for errors
        if(system(chdir(curr_direc)) == -1){
          printf("Change directory operation failed. Exiting.\n");
          exit(-1);
        }

        break;

      //move a file to a different directory
      case 'm':
      case 'M':

        //get user input, store in appropriate buffers
        strcpy(target_direc, curr_direc);
        strcat(target_direc, "/");
        printf("Move which file?:\n");
        scanf("%d", &num);
        strcpy(name, files[num].name);
        strcat(target_direc, name);

        //get user input, store in appropriate buffers
        strcat(curr_direc, "/");
        printf("Move file to which directory?:\n");
        scanf("%d", &num);
        strcat(curr_direc, dirs[num].name);
        printf("Moving to directory: %s\n", curr_direc);
        strcat(curr_direc, "/");
        strcat(curr_direc, name);

        //carry out the file move operation and check for errors
        if(rename(target_direc, curr_direc) == -1){
          printf("File move operation failed. Exiting.\n");
          exit(-1);
        }

        break;

      case 's':
      case 'S':

        //initialize these values to something valid
        sort_type = 't';
        sort_direction = 'a';

        //user input and error checking
        do{
          printf("t for time; s for size:\n");
          scanf("%s", &sort_type);
        }
        while (sort_type != 't' && sort_type != 's');

        //user input and error checking
        do{
          printf("a for ascending sort d for descending:\n");
          scanf("%s", &sort_direction);
        }
        while (sort_direction != 'a' && sort_direction != 'd');

        sort = 1; //true

        break;

      case 'd':
      case 'D':

        //get user input, store in appropriate buffers
        strcpy(target_direc, curr_direc);
        strcat(target_direc, "/");
        printf("Delete which file?:\n");
        scanf("%d", &num);
        strcpy(name, files[num].name);
        strcat(target_direc, name);

        if(remove(target_direc) == -1){
          printf("File delete operation failed. Exiting.\n");
          exit(-1);
        }

        break;
    }
  }

  return 0;
  free(files);
  free(dirs);
}