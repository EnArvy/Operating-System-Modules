#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<time.h>
#include<assert.h>
#include<signal.h>
#include<setjmp.h>
#include<stdarg.h>
#include<errno.h>
#include<unistd.h>
#include<sys/wait.h>
#include<linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// <fenv.h> - Floating-point environment
// <float.h> - Limits of floating-point types
// <inttypes.h> - Format conversion of integer types
// <limits.h>- Ranges of integer types
// <setjmp.h>- Nonlocal jumps
// <signal.h> - Signal handling
// <stdalign.h> - alignas and alignof convenience macros
// <uchar.h> - UTF-16 and UTF-32 character utilities
// <wchar.h> - Extended multibyte and wide character utilities
// <wctype.h>
#define max_cmd_length 2048


int wordcount(char *command, char *delim){
	char *pch;
	char commandcopy[max_cmd_length];
	strcpy(commandcopy,command);
	pch = strtok(commandcopy,delim);
	int words=0;
	while (pch != NULL) {
		words++;
		pch = strtok(NULL, delim);
	}
	return words;
}

int startswith(char *str,const char *prefix){
	int len = strlen(prefix);
	for(int i=0;i<len;i++){
		if(str[i]!=prefix[i]) return 0;
	}
	return 1;
}

void createArgs(char **args, char *command){
	int words = wordcount(command," \n");
	if(words==0) return;

	//Create args array
	char *save_ptr2;
	char *pch = strtok_r(command," \n",&save_ptr2);
	args[0] = pch;
	int i=1;
	while(pch != NULL){
		pch = strtok_r(NULL," \n",&save_ptr2);
		args[i] = pch;
		i++;
	}
	args[i] = NULL;
}

int main(){
	while(1){
		printf("MTL458>");
		char command[max_cmd_length];
		fgets(command,max_cmd_length,stdin);


		////////PIPE////////
		if(strchr(command,'|')!=NULL){
			char command1[max_cmd_length];
			char command2[max_cmd_length];
			char *pch = strtok(command,"|");
			strcpy(command1,pch);
			pch = strtok(NULL,"|");
			strcpy(command2,pch);

			if(command1 == NULL || command2 == NULL) continue;

			char *args1[max_cmd_length],*args2[max_cmd_length];
			createArgs(args1,command1);
			createArgs(args2,command2);

			int pipefd[2];
			pipe(pipefd);

			int child1 = fork();
			if (child1 == 0) {
				dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe write end
				close(pipefd[0]);
				close(pipefd[1]);
				execvp(args1[0],args1);
				perror(args1[0]);
			}

			int child2 = fork();
			if (child2 == 0) {
				dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the pipe read end
				close(pipefd[0]);
				close(pipefd[1]);
				execvp(args2[0],args2);
				perror(args2[0]);
			}

			close(pipefd[0]);
			close(pipefd[1]);
			waitpid(child1, NULL, 0);
			waitpid(child2, NULL, 0);
			continue;
		}
		//////PIPE END//////


		if(startswith(command,"cd")){
			char path[PATH_MAX];
			strncpy(path,command+3,strlen(command)-4);
			int status=chdir(path);
			if(status!=0) perror("cd");
			continue;
		};

		if(startswith(command,"history")){
			printf("History\n");
			continue;
		};

		char *args[max_cmd_length];
		createArgs(args,command);

		int process = fork();
		if(process==0){
			execvp(args[0],args);
			perror(args[0]);
		}
		else if(process>0){
			wait(NULL);
		}
		else{
			printf("Error\n");
		}
	}
	return 0;
}