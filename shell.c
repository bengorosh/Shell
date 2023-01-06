// PID: 730470759
// I pledge the COMP211 honor code.

// ----------------------------------------------
// These are the only libraries that can be
// used. Under no circumstances can additional
// libraries be included
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "shell.h"

const char *PATH_SEPARATOR = ":";

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist
// --------------------------------------------
const char* BUILT_IN_COMMANDS[] = { "cd", "exit", NULL };


/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * Allocate memory for the command. The last element in `p_cmd->argv` should be
 * NULL.
 *
 * The length of `p_cmd->argv` should be `argc+1`: the first `argc` slots are
 * used to store the arguments and the last one is set to NULL.
 *
 * Arguments:
 *      p_cmd : pointer to the command need to allocate memory.
 *      argc :  the number of arguments.
 *
 * Return:
 *      None
 */
void alloc_mem_for_command(command_t* p_cmd, int argc){
    p_cmd->argv = malloc((argc) * sizeof(char*));
    for(int i = 0; i < argc; i++) {
        p_cmd->argv[i] = malloc(MAX_ARG_LEN * sizeof(char));
    }
    // make an array, initialize it, assign a pointer to it (in the struct?)
} // end alloc_mem_for_command function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function is used to free memory that may be malloc'd for the name and
 * argv fields in the command_t structure.  To be safe, you should also set the
 * pointer values to NULL.
 *
 * HINT(s): See man page for more information about free function
 a*
 * Arguments:
 *         p_cmd : pointer to a command_t structure
 * Return: 
 *        None
 *
 */
void cleanup(command_t* p_cmd) {
    for (int i = 0; i < p_cmd->argc; i++){
        free(p_cmd->argv[i]);
    }
    free(p_cmd->argv);
} // end cleanup function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function will parse the command (cmd for short) and its arguments
 * specified by the user.
 *
 * HINT(s): This function is "essentially" an exercise in string parsing.
 *
 *         EXAMPLE#1, if the user enters
 *
 *                                 cd /mnt/cdrom
 *
 *                 at the shell prompt, line would contain "cd /mnt/cdrom".
 *                 Using the space character as the delimiter, the fields in
 *                 the command struct would be:
 *
 *                                 - argc = 2
 *                                 - argv = {"cd", "/mnt/cdrom", NULL}
 *
 *         EXAMPLE#2, if the user enters
 *
 *                                 ls -la
 *
 *                 at the shell prompt, line would contain "ls -la". Using the
 *                 space character as the delimiter, the fields in the command
 *                 struct would be:
 *
 *                                 - argc = 2
 *                                 - argv = {"ls", "-la", NULL}
 *
 *         EXAMPLE#3, if the user enters nothing at the shell prompt (i.e.
 *         simply hits the return key), line would NULL, and the fields in
 *         the command struct would be:
 *
 *                                 - argc = 0
 *                                 - argv = {NULL}
 *
 *  Arguments:
 *      line: pointer to the string containing the cmd.
 *      p_cmd: pointer to the command_t structure
 *
 *  Return:
 *      N/A
 *
 */
void parse(char* line, command_t* p_cmd) {
    if(line[0] == NULL){
        //empty command. Setup p_cmd and return
        p_cmd->argc = 0;
        p_cmd->argv = malloc(sizeof(char*));
        p_cmd->argv[0] = NULL;
        return;
    }else{
        char *token;
        int num_args = 0;
        char *cp_line = strdup (line);
   /* get the first token */
   token = strtok(cp_line, " ");
   /* walk through other tokens */
   while( token != NULL ) {
      num_args++;
      token = strtok(NULL, " ");
   }
   p_cmd->argc = num_args;
   alloc_mem_for_command (p_cmd, num_args);

    num_args = 0;
    char *cp_line1 = strdup (line);

   /* get the first token */
   token = strtok(cp_line1, " ");

   /* walk through other tokens */
   while( token != NULL ) {
      p_cmd->argv[num_args] = strdup (token);
      num_args++;
      token = strtok(NULL, " ");
   }
   p_cmd->argv[num_args] = NULL;
    }
    
} // end parse function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function is used determine if the named command (cmd for short) entered
 * by the user in the shell can be found in one of the folders defined in the
 * PATH environment (env or short) variable. If the file exists, then the name
 * of the executable at argv[0] is replaced with the location (fully qualified
 * path) of the executable.
 *
 * For example,
 *
 * if command_t.argv[0] is "ls", and "ls" is in the "/usr/bin" folder,
 * then command_t.argv[0] would be changed to "/usr/bin/ls"
 *
 * HINT(s): Use getenv() system function to retrieve the folders defined in the
 *                 PATH variable. An small code segment is shown below that
 * demonstrates how to retrieve folders defined in your PATH.
 *
 *                         char* path_env_variable;
 *                         path_env_variable = getenv( "PATH" );
 *                         printf("PATH = %s\n", path_env_variable );
 *
 *                 The format of the PATH is very simple, the ':' character is
 * delimiter, i.e. used to mark the beginning and end of a folder defined in the
 * path.
 *
 *                 Write a loop that parses each folder defined in the path,
 * then uses this folder along with the stat function (see "File/Directory
 * Status" section in the assignment PDF).
 *
 *  Arguments:
 *         p_cmd: pointer to the command_t structure
 *  Return:
 *      TRUE: if cmd is in the PATH
 *      FALSE:  if not in the PATH.
 *
 */
int find_fullpath( command_t* p_cmd ) {
   /* get the first token */
   if (p_cmd->argc < 1) {
        return FALSE; // TODO: 0 args.
    }
    char *command = (p_cmd->argv)[0];
    char* cmd_pointer = command;
    char *path = getenv("PATH");
    char *temp = malloc(sizeof(char) * MAX_ARG_LEN + 1);
    char *pointer = temp;

    int flag = FALSE;
    struct stat s;
    int x = 0;

    for (; *path; path++) {
        if(*path != ' '){
            flag = TRUE;}
        if(flag == TRUE){
            if (*path == ':') {
                temp[x] = '/';
                while (*cmd_pointer) {
                    x++;
                    temp[x] = *cmd_pointer;
                    cmd_pointer++;}
                if (stat(pointer, &s) == SUCCESS) {
                    strcpy(p_cmd->argv[0], pointer); // Replace first arg w / command path.
                    free(temp);
                    return TRUE;}
                free(temp);
                temp = NULL;
                temp = calloc(MAX_ARG_LEN + 1, sizeof(int));
                pointer = temp;
                cmd_pointer = command;
                x = 0;
            }else{
                temp[x] = *path;
                x++;}}}
    temp[x] = '/';
    while (*cmd_pointer) {
        x++;
        temp[x] = *cmd_pointer;
        cmd_pointer++;
    }
    if (stat(pointer, &s) == SUCCESS) {
        strcpy(p_cmd->argv[0], pointer);
        free(temp);
        return TRUE;} 
    free(temp);
    return FALSE;
} // end find_fullpath function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function will execute external commands (cmd for short).
 *
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structor
 *
 * Return:
 *      SUCCESS: successfully execute the command.
 *      ERROR: error occurred.
 *
 */
int execute( command_t* p_cmd ) {

    int status = SUCCESS;
    int child_process_status;

    if(p_cmd->argv[0] == NULL){
        return ERROR;
    }

    if (is_builtin(p_cmd)) {
            do_builtin(p_cmd);
    }
    else{
        if (!find_fullpath(p_cmd)) {
            printf("%s", "Command '");
            printf("%s", p_cmd->argv[0]);
            printf("%s\n", "' not found!");
            return SUCCESS;
        }
        else{
            pid_t child_pid = fork();
            if (child_pid == 0){
                execv(p_cmd->argv[0], p_cmd->argv);
                exit(EXIT_SUCCESS);
            }
            wait(0);
            return SUCCESS;
        }
    }
    return status;
  
} // end execute function

/* ------------------------------------------------------------------------------
 *
 * This function will determine if command (cmd for short) entered in the shell
 * by the user is a valid builtin command.
 *
 * HINT(s): Use BUILT_IN_COMMANDS array defined in shell.c
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      TRUE:  the cmd is in array `valid_builtin_commands`.
 *      FALSE: not in array `valid_builtin_commands`.
 *
 */
int is_builtin(command_t* p_cmd) {

    int cnt = 0;

    while ( BUILT_IN_COMMANDS[cnt] != NULL ) {

        if ( strcmp( p_cmd->argv[0], BUILT_IN_COMMANDS[cnt] ) == 0 ) {
            return TRUE;
        }

        cnt++;
    }

    return FALSE;

} // end is_builtin function

/* ------------------------------------------------------------------------------
 *
 * This function is used execute built-in commands such as change directory (cd)
 *
 * HINT(s): See man page for more information about chdir function
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      SUCCESS: no error occurred during chdir operation.
 *      ERROR: an error occured during chdir operation.
 *
 */
int do_builtin(command_t* p_cmd) {
    // If the command was 'cd', then change directories
    // otherwise, tell the program to EXIT

    struct stat buff;
    int status = SUCCESS;

    // exit
    if (strcmp( p_cmd->argv[0], "exit") == 0) {
        exit(status);
    }

    // cd
    if (p_cmd->argc == 1) {
        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental
        // variable

        status = chdir(getenv("HOME"));
    } else if ( (status = stat(p_cmd->argv[1], &buff)) == 0 && (S_IFDIR & buff.st_mode) ) {
        // -----------------------
        // cd with one arg
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir(p_cmd->argv[1]);
    }

    return status;

} // end do_builtin function
