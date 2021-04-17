/*
 * sh360.c
 * CSC 360, Spring 2021
 * Jack Cooper, V00920368
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>


#define MAX_INPUT_LINE 80
#define MAX_PROMPT_LENGTH 10
#define MAX_DIR_LENGTH 25
#define MAX_PATH_DIRS 10
#define MAX_ARGS 7


//This function tokenizes the given input, and places the results in tokens[]
//The function returns the position of the last element of tokens[] 
int tokenize_input(char* input, char* tokens[]){
	int count = 0;
	tokens[0] = strtok(input, " ");
	while(tokens[count] != NULL){
		tokens[++count] = strtok(NULL, " ");
		if(count > MAX_ARGS + 1){
			fprintf(stderr, "error: too many arguments\n");
			return -1;
		}
	}
	return count;
}
//This function takes a found command, an array of tokens, and an argument vector.
//tokens[0] must be the command corresponding to command_path
//The command path is set as argv[0], and then tokens are placed into the argv after.
//If a token is NULL or '->' then null is appended to the argv. If NULL, return -1 to signal that we are done, if '->' return the position of it.
int set_up_command(char* tokens[], char* command_path, char* argv[]){
	argv[0] = command_path;
	for (int i = 1; i < MAX_ARGS + 1; i++){
		if(tokens[i] == NULL){
			argv[i] = NULL;
			return -1;
		} else if((strcmp(tokens[i],"->")==0)) {
			argv[i] = NULL;
			return i+1;
		} else {
			argv[i] = tokens[i];
		}	
	}	
	return -1;
}

//Takes a char* of a command, and attempts to find it in the path. 
//If found, return the path
//Else, print error and return NULL
int seek_command(char* command, char* command_path, char pathnames[][MAX_DIR_LENGTH+1] ){
	for(int i =0; i<MAX_PATH_DIRS; i++){
		strncpy(command_path, pathnames[i], strlen(pathnames[i]) + 1);
		strcat(command_path, "/");
		strcat(command_path, command);
		if(access(command_path, F_OK) == 0){
			return 1;
		}	
	}
	fprintf(stderr, "%s: command not found\n", command);
	command_path = NULL;
	return 0;
}


int main(int agrc, char* argv[]){
	char input[MAX_INPUT_LINE];
	char* child_argv[MAX_ARGS+1], *child2_argv[MAX_ARGS+1], *child3_argv[MAX_ARGS+1];
	char command_path[MAX_INPUT_LINE], command_path2[MAX_INPUT_LINE], command_path3[MAX_INPUT_LINE];
	char *child_envp[] = { 0 };
	int pid_head, pid_mid, pid_tail, status, fdout;
	int double_pipe_flag = 0;

	FILE *fdrc;					//rc file to be read
	char prompt[MAX_PROMPT_LENGTH+1];		//string which shall be displayed as shell prompt
	char pathnames[MAX_PATH_DIRS][MAX_DIR_LENGTH+1];//string array containing pathnames

	//attempt to open rc file
	fdrc = fopen(".sh360rc", "r");			
	if(fdrc == NULL){
		fprintf(stderr, "error: cannot find rc file for reading\n");
		exit(1);
	}

	//acquiring prompt
	fgets(prompt, MAX_PROMPT_LENGTH, fdrc);
	if (prompt[strlen(prompt) -1] == '\n'){
		prompt[strlen(prompt) -1] = '\0';
	}

	//acquiring pathnames
	for(int i = 0; i<MAX_PATH_DIRS; i++){
		fgets(pathnames[i],MAX_DIR_LENGTH, fdrc);
		if(pathnames[i][strlen(pathnames[i])-1] == '\n'){
			pathnames[i][strlen(pathnames[i])-1]='\0';
		}
	}
	//close the rc file, we have read all data we reqiure
	fclose(fdrc);

	//start of infinite loop
	for(;;){

		fprintf(stdout, "%s", prompt);			//print prompt
		fflush(stdout);
		fgets(input, MAX_INPUT_LINE, stdin);	//get input
		fflush(stdin);

		//if empty or '\n' restart the loop
		if(strlen(input) == 0 || strlen(input) == 1){
			continue;
		}

		//remove '\n' and replace with '\0'
		if(input[strlen(input)-1] == '\n'){
			input[strlen(input)-1] = '\0';
		}

		//check for exit
		if (strcmp(input, "exit") == 0){
			exit(0);
		}
/* FROM BELOW WE MUST EDIT TO ACCOMADATE PIPING */
		char* tokens[MAX_ARGS+1];
		int token_count = tokenize_input(input, tokens);
		int arrow_index= 0;
		int fd1[2], fd2[2];


		if (token_count < 0){
			continue;
		}
		//Stream redirection
		if (strcmp(tokens[0], "OR") == 0){
			token_count--;
			if(token_count == 0){
				fprintf(stderr, "error: no command following 'OR'\n");
				continue;
			}
			if(!seek_command(tokens[1], command_path, pathnames)){
				continue;
			}
			
			if ((arrow_index = set_up_command(&tokens[1], command_path, child_argv))>0){
				if(token_count <= arrow_index){
					fprintf(stderr, "error: no file given after '->'\n");
					continue;
				} else if ((fdout = open(tokens[arrow_index+1], O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR))<0) {
					fprintf(stderr, "error: could not open \"%s\" for writing\n",tokens[arrow_index+1]);
					continue;
				}
						
				if((pid_head = fork()) == 0){
					dup2(fdout, 1);
					dup2(fdout, 2);
					execve(child_argv[0], child_argv, child_envp);
				} else {
					wait(&status);
				}
			} else {
				fprintf(stderr, "error: OR specified, but no '->' found\n");
			}

		//Piping case
		} else if (strcmp(tokens[0], "PP") == 0){

			// reduce token count to ignore 'PP'
			token_count--;
			
			// check to make sure there are args following 'PP'
			if(token_count == 0){
				fprintf(stderr, "error: no command following 'PP'\n");
				continue;
			}
			arrow_index = 0;

			// try to find command, if fails, error printed within seek_command
			if(!seek_command(tokens[arrow_index+1],command_path, pathnames)){
				continue;
			}


			if((arrow_index = set_up_command(&tokens[arrow_index+1], command_path, child_argv))>0){
				if(token_count <= arrow_index){
					fprintf(stderr, "error: no command given after '->'\n");
					continue;
				}
			} else {
				fprintf(stderr, "error: PP specified, but no '->' found\n");
				continue;
			}
			if(!seek_command(tokens[arrow_index+1],command_path2, pathnames)){
				continue;
			}
			

			arrow_index = set_up_command(&tokens[arrow_index+1], command_path2, child2_argv);
		
			pipe(fd1);

			if(arrow_index >= 0){
				if(!seek_command(tokens[arrow_index+1],command_path3,pathnames)){
					continue;
				}


				arrow_index = set_up_command(&tokens[arrow_index+1],command_path3,child3_argv);
	
				double_pipe_flag = 1;
				pipe(fd2);
			}


			if((pid_head = fork()) == 0){
				dup2( fd1[1],1);
				close(fd1[0]);
				if(double_pipe_flag){
					close(fd2[0]);
					close(fd2[1]);
				}
				execve(child_argv[0], child_argv, child_envp);
			}
			if((pid_mid = fork()) == 0){
				dup2(fd1[0],0);
				close(fd1[1]);
				if(double_pipe_flag){
					dup2( fd2[1],1);
					close(fd2[0]);
				}
				execve(child2_argv[0], child2_argv, child_envp);
			}
			if( double_pipe_flag){
				if((pid_tail = fork()) == 0){
					dup2(fd2[0],0);
					close(fd2[1]);
					close(fd1[0]);
					close(fd1[1]);
					execve(child3_argv[0], child3_argv, child_envp);
				}
			}

			//now running in parent
			close(fd1[0]);
			close(fd1[1]);
			if (double_pipe_flag){
				close(fd2[0]);
				close(fd2[1]);
			}

			waitpid(pid_head, &status, 0);
			waitpid(pid_mid, &status, 0);
			if(double_pipe_flag){
				waitpid(pid_tail, &status, 0);
			}
			double_pipe_flag = 0;
			



		//No piping or stream redirection, just execute command
		} else {
			if(!seek_command(tokens[0], command_path, pathnames)){
				continue;
			}
	
			if ((set_up_command(tokens, command_path, child_argv))<0){
				if ((pid_head = fork()) == 0){
					execve(child_argv[0], child_argv, child_envp);
				} else {
					wait(&status);
				}
			} else {
				fprintf(stderr, "Something went wrong setting up the command\n");
			}
		}
		
	}
}
		
		
/* END OF NEW CODE, START OF OLD */

/*
		//parse input and set up for execve
		child_argv[0] = strtok(input, " ");
		
		//Check for OR
		if (strcmp(child_argv[0], "OR") == 0){
			OR_Flag = 1;
			child_argv[0] = strtok(NULL, " ");	
		}
		
		//Check for PP
		if (strcmp(child_argv[0], "PP") == 0){
			PP_Flag = 1;
			child_argv[0] = strtok(NULL, " ");
		}

		int child_argc = 0;
		while(child_argv[child_argc] != NULL){
			child_argv[++child_argc] = strtok(NULL, " ");
			if(child_argv[child_argc] == NULL){
				continue;
			}
			if((strcmp(child_argv[child_argc], "->") == 0) && OR_Flag){
				filename = strtok(NULL, " ");
				if(filename == NULL){
					fprintf(stderr,"error: no file given following \"->\"\n");
					ERROR_Flag = 1;
					break;
				}

			}

		}
		if(ERROR_Flag){
			ERROR_Flag = 0;
			continue;
		}
		if(OR_Flag){

		}
		//seek command
		for(int i = 0; i<MAX_PATH_DIRS; i++){
			strncpy(command_path, pathnames[i], strlen(pathnames[i]) + 1);
			strcat(command_path, "/");
			strcat(command_path, child_argv[0]);
			if(access(command_path, F_OK) == 0){
				child_argv[0] = command_path;
				pid = fork();
				if((pid == 0) && OR_Flag){
					if((fdout = open(filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) < 0){
						fprintf(stderr, "error: could not open \"%s\" for writing\n",filename); 
						break;
					}
					dup2(fdout, 1);
					dup2(fdout, 2);
					execve(child_argv[0], child_argv, child_envp);
				} else if (pid == 0){
					execve(child_argv[0], child_argv, child_envp);
				}
				while(wait(&status)>0){
				}

				break;
			}
			if (i == MAX_PATH_DIRS-1){
				printf("%s: command not found\n", child_argv[0]);
			}
		}		

	}


}
*/
