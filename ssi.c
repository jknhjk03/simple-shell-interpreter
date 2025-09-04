#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

typedef struct {
    pid_t *pid;
    int size;
    
} pidlist;

void user_info(){
    char *user;
    char host_name[128];
    char cwd[128];
    
    // getting username
    user = getlogin();

    // getting the name of the host running the program
    gethostname(host_name, sizeof(host_name));

    // getting the current directory
    getcwd(cwd, sizeof(cwd));

    printf("%s@%s: %s > ", user, host_name, cwd);

}

void change_directory(char *directory){
    
    // if user inputed "cd" or "cd ~ ", the program returs to the home directory
    if(directory == NULL || strcmp(directory, "~") == 0){
        directory = getenv("HOME");
    } 

    if(chdir(directory) != 0){
        printf("cd: No such file or directory: %s\n", directory);
    }
}

int main(){

    int max_input = 1024;
    char *prompt = "";
    pidlist *bg_list = malloc(sizeof(pidlist));
    bg_list->size = 0;
    bg_list->pid = malloc(sizeof(pid_t));

    // ignores if user presses Ctrl+C keys to stop the program
    signal(SIGINT, SIG_IGN);

    read_history(".ssi_history");

    while(1) {

        // prints user information
        user_info();
        
        // reads one line of input from user
        char *user_input = readline(prompt);
        
        // if user inputs "exit", the program ends
        if(strcmp("exit", user_input) == 0){
            free(user_input);
            break;
        }

        char *args[max_input];
        int arg_index = 0;
        char *token = strtok(user_input, " ");

        while(token && arg_index < max_input - 1){
            args[arg_index++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_index] = NULL;

        // user inputed no characters (pressed the Enter key without any command line)
        if(arg_index == 0){
            free(user_input);
            printf("\n");
            continue;
        }

        add_history(user_input);

        // user inputed "cd" as the first input
        if(strcmp("cd", args[0]) == 0){
            change_directory(args[1]);
            continue;   
        }else if(strcmp("bglist", args[0]) == 0){
            int new_size = 0;
            for(int i = 0; i < bg_list->size; ++i){
                int status;
                pid_t result = waitpid(bg_list->pid[i], &status, WNOHANG);
                if (result == 0) {
                    printf("[%d] Running PID: %d\n", i + 1, bg_list->pid[i]);
                    bg_list->pid[new_size++] = bg_list->pid[i]; // Keep active process
                } else {
                    printf("[%d] Finished PID: %d\n", i + 1, bg_list->pid[i]);
                }
                continue;
                //printf("%d: PID %d\n", i + 1, bg_list->pid[i]);
            }
            bg_list->size = new_size;
        }else{
            char **background_arguments = &args[1];
            pid_t pid = fork();
            if(strcmp("bg", args[0]) == 0 && arg_index > 1){
                if(pid == 0){
                    for (int i = 0; args[i] != NULL; i++) {
                        if (strcmp(args[i], ">") == 0) {
                            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd < 0) {
                                perror("open");
                                exit(1);
                            }
                            dup2(fd, STDOUT_FILENO); // Redirect stdout
                            close(fd);
                            args[i] = NULL; // Cut off command at redirection symbol
                            break;
                        }
                        else if (strcmp(args[i], ">>") == 0) {
                            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                            if (fd < 0) {
                                perror("open");
                                exit(1);
                            }
                            dup2(fd, STDOUT_FILENO);
                            close(fd);
                            args[i] = NULL;
                            break;
                        }
                        else if (strcmp(args[i], "<") == 0) {
                            int fd = open(args[i + 1], O_RDONLY);
                            if (fd < 0) {
                                perror("open");
                                exit(1);
                            }
                            dup2(fd, STDIN_FILENO); // Redirect stdin
                            close(fd);
                            args[i] = NULL;
                            break;
                        }
                    }                    
                    execvp(background_arguments[0], background_arguments);
                    exit(1);
                } else if(pid > 1){
                    bg_list->size++;
                    bg_list->pid = realloc(bg_list->pid, sizeof(pid_t) * bg_list->size);
                    bg_list->pid[bg_list->size - 1] = pid;
                    continue;
                }else{
                    waitpid(pid, NULL, 0);
                }
            }
                
            if(pid == 0){
                signal(SIGINT, SIG_DFL);    // responds if user pressed Ctrl + C keys to stop certain functions
                execvp(args[0], args);
                exit(1);
            } else if(pid > 0){
                wait(NULL);
            }
        }

        free(user_input);
    }

    write_history(".ssi_history");
    
    free(bg_list->pid);
    free(bg_list);

    return 0;
}