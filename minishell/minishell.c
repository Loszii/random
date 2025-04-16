#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

volatile sig_atomic_t interrupted = 0;

//for quick sort
int compare_ints(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return x - y;
}

//sets interrupted to true when control c is received
void sigint_handler(int sig) {
    interrupted = 1;
    printf("\n");
}

//takes in directory name and return 1 if it is fully numeric
int lp_helper(char* s) {
    while (*s) {
        if (!isdigit(*s)) {
            return 0;
        }
        s++;
    }
    return 1;
}


int main() {  
    struct sigaction action = {0}; //handle SIGINT
    action.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &action, NULL) == 01) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
    }

    while (1) {
        interrupted = 0;

        char* cur_dir = getcwd(NULL, 0); //malloc cur dir
        if (cur_dir == NULL) {
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            free(cur_dir);
            return 1;
        } else {
            printf("%s[%s]%s> ", BLUE, cur_dir, DEFAULT); //print directory in blue then waits for command
        }

        char line[256];
        if (fgets(line, 256, stdin) == NULL) { //EOF received or control c or failure
            if (interrupted) {
                free(cur_dir);
                continue;
            } else if (feof(stdin)) {
                printf("EOF received.\n");
                free(cur_dir);
                return 0;
            } else {
                fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
            }
        }
        int arg_size = 100;
        int arg_count = 0;
        char* args[arg_size+1]; //allocate room for atleast arg_size args (+1 to null terminate)

        char* token = strtok(line, " \n"); //split line by space and \n
        while (token != NULL && arg_count < arg_size) { //token will be null when no more args to parse
            args[arg_count] = token;
            arg_count++;
            token = strtok(NULL, " \n");
        }
        args[arg_count] = NULL; //make the args null terminated

        if (arg_count == 0) {
            free(cur_dir);
            continue; //didnt enter anything so restart loop
        }

        if (strcmp(args[0], "cd") == 0) { //cd

            struct passwd* pw = getpwuid(getuid()); //get user and find their home dir
            if (pw == NULL) {
                fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno)); //passwd error
                free(cur_dir);
                continue;
            }
            char* home_dir = pw->pw_dir;

            if (arg_count == 1 || (strcmp(args[1], "~") == 0)) { //to home directory
                if (chdir(home_dir) != 0) {
                    fprintf(stderr, "Error: Cannot change directory to %s. %s.\n", home_dir, strerror(errno));
                }
            } else if (arg_count == 2) { //custom directory in second arg spot
                if (chdir(args[1]) != 0) {
                    fprintf(stderr, "Error: Cannot change directory to %s. %s.\n", args[1], strerror(errno));
                }
            } else {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
            }

        } else if (strcmp(args[0], "exit") == 0) { //exit
            free(cur_dir);
            return 0;
        } else if (strcmp(args[0], "pwd") == 0) { //pwd
            printf("%s\n", cur_dir);
        } else if (strcmp(args[0], "lf") == 0) { //lf
            DIR* dir = opendir("."); //open directory object to read its entries
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) { //loop over entries and print their names
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                printf("%s\n", entry->d_name); //print each entry
            }

        } else if (strcmp(args[0], "lp") == 0) { //lp
            //first count directories to place pid in array
            int p_count = 0;
            int max_pid = 0; //length of longest to align
            DIR* dir = opendir("/proc/");
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                if (lp_helper(entry->d_name)) {
                    p_count++;
                    if (strlen(entry->d_name) > max_pid) {
                        max_pid = strlen(entry->d_name);
                    }
                }
            }
            closedir(dir);

            int* p_array = malloc(p_count * sizeof(int));
            if (p_array == NULL) {
                fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
            }

            dir = opendir("/proc/");
            int i = 0;
            while ((entry = readdir(dir)) != NULL) {
                if (lp_helper(entry->d_name)) {
                    p_array[i] = atoi(entry->d_name); //add each pid to array
                    i++;
                }
            }
            closedir(dir);
            qsort(p_array, p_count, sizeof(int), compare_ints); //sort the pid

            for (int j=0; j < p_count; j++) {
                //first make the full path by adding the name
                char cur_name[256] = {0};
                sprintf(cur_name, "%d", p_array[j]); //turn back into string now that it is sorted

                char abs_path[6+strlen(cur_name)+1]; //"/proc/pid"
                strcpy(abs_path, "/proc/");
                strcat(abs_path, cur_name); //create the path for it so we can get stats

                struct stat st; //get stats about the process
                stat(abs_path, &st);

                struct passwd* pw = getpwuid(st.st_uid); //get passwd of user that invoked

                char cmd_path[6+strlen(cur_name)+8+1];
                strcpy(cmd_path, abs_path);
                strcat(cmd_path, "/cmdline"); //make path to the cmd used to start the proc

                char cmd[256];
                FILE* f = fopen(cmd_path, "r"); //open to read the command

                //before printing we print the white space if needed
                for (int k=0; k < (max_pid - strlen(cur_name)); k++) {
                    printf(" ");
                }
                //read from file and print
                if (f == NULL || fgets(cmd, 256, f) == NULL) { //no command
                    printf("%s %s\n", cur_name, pw->pw_name);
                } else {
                    printf("%s %s %s\n", cur_name, pw->pw_name, cmd); //print all 3
                }
                fclose(f);
            }
            free(p_array);

        } else {
            //fork and run any other command given
            pid_t pid = fork();

            if (pid < 0) {
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            } else if (pid == 0) { //child
                struct sigaction sa = {0}; //restore to default
                sa.sa_handler = SIG_DFL;
                sigaction(SIGINT, &sa, NULL);

                execvp(args[0], args); //child runs command
                fprintf(stderr, "Error: exec() failed. %s\n", strerror(errno));
                free(cur_dir);
                return 1;
            } else { //parent
                int wait_return = wait(NULL); //wait for child
                if (interrupted) {
                    wait(NULL); //wait for the child to exit
                } else if (wait_return == -1) {
                    fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                }
            }

        }
        free(cur_dir); //free cur dir after each loop
    }
    return 0;
}