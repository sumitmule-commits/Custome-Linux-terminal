#include "my_shell.h"



// cd, pwd, echo, env, setenv, unsetenv, which, exit
int shell_builts(char** args, char** env, char* initial_directory)
{
    (void) env;
    (void) initial_directory;
    // printf("Arg[0]: %s", args[0]);
    // printf("\n");
    if (my_strcmp(args[0], "cd") == 0) {
        return command_cd(args, initial_directory);
    } else if (my_strcmp(args[0], "pwd") == 0) {
        command_pwd();
    } else if (my_strcmp(args[0], "echo") == 0) {
        command_echo(args, env);
    } else if (my_strcmp(args[0], "env") == 0) {
        command_env(env);
    } else if (my_strcmp(args[0], "which") == 0) {
        // command_which(args, env);
    } else if (my_strcmp(args[0], "exit") == 0 || my_strcmp(args[0], "quit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        // Not a builtin command
    }
    return 0;
}


void shell_loop(char** env)
{
    char* input = NULL;
    size_t input_size = 0;

    char** args;

    char* initial_directory = getcwd(NULL, 0);

    while (1)   //Without while (1), your shell would run exactly one command and then immediately exit back to Windows.       
    {           //The loop keeps the shell "alive" so you can keep working.
        printf("[my_shell]> ");
        if (getline(&input, &input_size, stdin) == -1) // End of the file (EOF), ctrl + D
        {
            perror("getline");   // The Pause (Getline): The shell waits at getline.
                                // It doesn't move until you type something and hit Enter.
            break;
        }    

        // printf("Input: %s", input);

        args = parse_input(input);      //parse_input: Since you type a full sentence (e.g., cd Desktop), 
                                        //the shell needs to "tokenize" it into separate words: args[0] becomes "cd"
                                        //and args[1] becomes "Desktop". such that borrow some memory from system.
        
        if (args[0]) //This if statement ensures that the shell only tries to execute something if the user actually typed at least one word.
        {
            shell_builts(args, env, initial_directory);
        }
        free_tokens(args);  // returns the memoery back to system 
    }

} 

int main (int argc, char** argv, char** env)
{
    (void)argc;
    (void)argv;

    shell_loop(env);


    return 0;
}
<<<<<<< HEAD



// why your Shell uses both:
// Look at how the data transforms in your shell_loop:
// Start: You have char* input → "mkdir my_folder" (One long string).
// After parse_input: You get char** args.
// args[0] points to "mkdir"
// args[1] points to "my_folder"
// args[2] is NULL (The "End of the list" marker).
=======
>>>>>>> d84239c62d47801f34f5dc2ea2e856234cf714d7
