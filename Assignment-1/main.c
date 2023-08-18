#include<stdio.h>
#include<string.h>
// #include<stdlib.h>
// #include<ctype.h>
// #include<assert.h>
// #include<signal.h>
// #include<setjmp.h>
// #include<stdarg.h>
// #include<errno.h>
#include<unistd.h>
#include<sys/wait.h>
#include<linux/limits.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>

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
		int pipefd[2];
		pipe(pipefd);
		int piping = 0;
		int pipepid;
		if(strchr(command,'|')!=NULL){
			piping = 1;
			char subcommand[max_cmd_length];
			char *pch = strtok(command,"|");
			strcpy(subcommand,pch);
			pch = strtok(NULL,"|");
			strcpy(command,pch);

			if(subcommand == NULL || command == NULL){
				printf("Error\n");
				continue;
			}

			char *args[max_cmd_length];
			createArgs(args,subcommand);

			pipepid = fork();
			if (pipepid == 0) {
				dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe write end
				close(pipefd[0]);
				close(pipefd[1]);
				execvp(args[0],args);
				perror(args[0]);
			}
		}
		//////PIPE END//////


		if(startswith(command,"cd")){
			char path[PATH_MAX];
			strncpy(path,command+3,strlen(command)-4);
			path[strlen(command)-4]='\0'; //Remove \n from path
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
			if(piping){
				dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the pipe read end
				close(pipefd[0]);
				close(pipefd[1]);
			}
			execvp(args[0],args);
			perror(args[0]);
		}
		else if(process>0){
			close(pipefd[0]);
			close(pipefd[1]);
			if(piping)
				waitpid(pipepid, NULL, 0);
			waitpid(process, NULL, 0);
		}
		else{
			printf("Error\n");
		}
	}
	return 0;
}