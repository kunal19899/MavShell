/*
    Name: Kunal Samant
    ID: 1001534662
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_ARG 11 //Max number of arguments that can be passed by the user


//Input is:     gcc -Wall msh.c -o msh


//splits the input command in to an array 
//returns array of char* 
//char *in - the command which is entered by the user
char **split_input(char *in)
{
    char **command = malloc(200 * sizeof(char *));
    if (command == NULL)
    {
        perror("Malloc Failed");
        exit(1);
    }
    char *space = " ";
    char *token;
    int index = 0;

    if (in == NULL)
    {
        return command;
    }

    token = strtok(in, space);
    
    while (token != NULL) {    
        command[index]=token;
        index++;
        if (index > 8)
        {
            command[index] = NULL;
            return command;
        }
        token = strtok(NULL, space);
    }
    command[index] = NULL;
    return command;
}

//function which reads input and stores it 
//returns input given by user
//read_input - read the input; const char *line - the "msh> " prompt
char *read_input(const char *line)
{
    char *input = malloc(8*sizeof(char));
    printf("%s", line);
    gets(input); //using gets as it is accepting the user input (fgets was throwing a command not found error)
    return input;
}

//changes the current directory being accessed to that of the 'path'
// path is the intended 
int cd_call(char *path)
{
    return chdir(path);
}

//split a string into an array of characters
//return the split array of characters
//char *call - pointer to call[n] where n is the index of the parameter in the command
char *split_call(char *call)
{
    char *split_call = malloc(8*sizeof(char));
    int index = 0;
    if (split_call == NULL)
    {
        perror("Malloc Failed");
        exit(1);
    }

    if (call == NULL)
    {
        return split_call;
    }

    while(call[index] != '\000')
    {
        split_call[index] = call[index];
        index++;
    }

    split_call[index] = '\000';
    return split_call;
}

//returns the length of the *call array
//char **call - the array which holds a parameter of the command inputed in each index
int count_incr(char **call)
{
    int count = 0;
    while (call[count] != NULL)
    {
        count++;
    }
    return count;
}

//returns the number of characters in call[n] where n is the index being checked
//char *splitcall - the array which holds the string of call[n] with each character in the string stored in each index
int count_incr2(char *splitcall)
{
    int count = 0;
    while (splitcall[count] != '\000')
    {
        count++; 
    }
    return count;
}

//function which handles CTRL-Z and CTRL-C signals
void handle_signal(int sig)
{
    int status;
        switch(sig){
        case SIGCHLD:
            waitpid(-1, &status, WNOHANG);
            printf("\n");
            break;
        case SIGTSTP:
            waitpid(-1, &status, WNOHANG);
            printf("\n");
            break;
        case SIGINT:
            waitpid(-1, &status, WNOHANG);
            printf("\n");
            break;
        }
}

int main()
{
    char **call;
    char *input = malloc(8*sizeof(char));
    pid_t child;
    char* input_arr[10000];
    int index = 0;
    pid_t pids[15];
    int pid_count = 0;
    char *splitcall = malloc(8*sizeof(char));

    system("clear");

    signal(SIGINT, SIG_IGN);
    struct sigaction act;
    while (1)
    {
        input = read_input("msh> ");  //accepts the input given by the user
        call = split_input(input);  //puts input into the "split_input" function to seperate the command into an array of strings by the " "  
        splitcall = split_call(call[0]);// splits the first part of the command prompt into an array of characters and stores it into splitcall

        //function checks whether the number of pids generated goes above and then re-initialises if it does
        //requiremtn for "listpids" command
        if (pid_count > 15)
        {
            pid_count = 0;
        }
        
        //checks if the command begins with a "!"
        //used for the "!n" command which prints out the commands from history
        if (splitcall[0] == '!')
        {
            int count = count_incr2(splitcall); // deterimes whether 'n' is single (returns 2) or double digits (returns 3)
            int comb_int;

            //if 'n' is double digits
            if (count > 2)
            {
                comb_int = 10*((int)splitcall[1]-48) + ((int)splitcall[2]-48); // stores n into comb_int after doing math: if n = 10 (splitcall[1] contains 1 but has returns 49, splitcall[2] contains 0 but has returns 48)
            }
            //if 'n is single digits
            else{
                comb_int = (int)splitcall[1]-48; // if n = 1 (splitcall[1] contains 1 but has returns 49)
            }

            // if the command history exeeds 50, this function ensures that the correct command runs after '!n'
            if (index >= 50)
            {
                comb_int+=(index-50);
            }

            // prints out error if user tries to print out a command outside of the history
            if (index-1 < comb_int) 
            {
                printf("Command not in history...\n");
                int a;
                for (a = 0; a < index; a++)
                {
                    printf("%d: %s\n", a, input_arr[a]);
                }
                continue;
            }

            // ensures 0 < n < 16
            if ((comb_int < 16) && (comb_int > 0))
            {
                input = input_arr[comb_int]; 
                call = split_input(input);
            }
            else{
                printf("\n**********************************\n");
                printf("!n, where n should be between 1-15");
                printf("\n**********************************\n\n");
                continue;
            }
        }

        //if a non-empty command is entered, runs through if function
        if (strcmp(input, "") != 0)
        {
            int count = count_incr(call); // counts the number of command line parameters including the command
            int input_index = 0;
            char *command123 = malloc(8 * sizeof(char)); 
            for (input_index = 0; input_index < count; input_index++)
            {
                command123 = strcat(command123, call[input_index]); 
                if (call[input_index+1] != NULL)
                {
                    command123 = strcat(command123, " ");
                }
            } 
            input_arr[index] = command123; // stores the command into 'input_arr' for the history function at the 'index' position
            index++;
        }
        else{
            continue;
        }
        
        //if command is empty, nothing happens, loop restarts
        if (*input=='\000')
        {
            continue;
        }

        
        if ((strcmp(call[0], "quit") == 0) || strcmp(call[0], "exit") == 0) // This IF statement is used to exit the shell using "exit" or "quit"
        {  
            exit(0);
        }

        if (strcmp(call[0], "cd") == 0) //used to check if the first command prompt is "cd"
        {
            if (cd_call(call[1]) < 0) // if the location doesnt exist an error is thrown
            {
                perror(call[1]);
            }  
            continue;
        }


        if (strcmp(call[0], "listpids") == 0)
        {
            int c;
            for (c = 0; c < pid_count; c++)
            {
                printf("%d: %d\n", c, pids[c]);
            }
            continue;
        }

        if (strcmp(call[0], "history") == 0)
        {
            int a;
            if (index <= 50)
            {
                for (a = 0; a < index; a++)
                {
                    printf("%d: %s\n", a, input_arr[a]);
                }
                continue;
            }
            else{
                for (a = index - 50; a < index; a++)
                {
                    printf("%d: %s\n", a-index+50, input_arr[a]);
                }
                continue;
            }
        }

        if (strcmp(call[0], "bg") == 0)
        {
            kill(child, SIGCONT);
            continue;
        }
        
        child = fork(); //Forks the process and creates a child process
        pids[pid_count] = child; //add the retuen child pid to the array of pids
        pid_count++;

        if (child < 0)
        {
            perror("Fork failed");
            exit(1);
        }

        if (child == 0)
        {
            signal(SIGINT, SIG_DFL);
            execvp(call[0], call);
            if (execvp(call[0], call) < 0)
            {
                printf("%s: command not found\n\n", call[0]);
                _exit(1);
            }
        }
        else{
        //Zero out the sigaction struct
        memset (&act, '\0', sizeof(act));
        //Set the handler to use the function handle_signal() 
        act.sa_handler = &handle_signal;
        
        //Install the handler and check the return value.
        
        if (sigaction(SIGTSTP, &act, NULL) < 0)
        {
            perror ("sigaction: ");
            return 1;
        }
        if (sigaction(SIGINT, &act, NULL) < 0)
        {
            perror("sigaction: ");
            return 1;
        }
            int status;
            waitpid(child, &status, 0);
            fflush(NULL);
        }

    }
    free(input);
    free(input_arr);
    free(call);
    return 0;
}
