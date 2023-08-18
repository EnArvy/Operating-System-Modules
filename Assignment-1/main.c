#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
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
#define INITIAL_CAPACITY 10

typedef struct {
    char **data;
	int *id;
    int size;
    int capacity;
} History;

void initHistory(History *history) {
    history->data = (char **)malloc(INITIAL_CAPACITY * sizeof(char *));
	history->id = (int *)malloc(INITIAL_CAPACITY * sizeof(int));
    history->size = 0;
    history->capacity = INITIAL_CAPACITY;
}

void addCommand(History *history, const char *input, int id) {
    if (history->size >= history->capacity) {
        history->capacity += INITIAL_CAPACITY;
        history->data = (char **)realloc(history->data, history->capacity * sizeof(char *));
		history->id = (int *)realloc(history->id, history->capacity * sizeof(int));
    }

    history->data[history->size] = strdup(input);
	history->id[history->size] = id;
    history->size++;
}

int Digits(int n){
	int count=0;
	while(n>0){
		n/=10;
		count++;
	}
	return count;
}

void printHistory(const History *history, int n) {
    int start = (history->size > n) ? history->size - n : 0;
	if(n==0)start = 0;
	int digits = Digits(history->size);
    for (int i = start; i < history->size; i++) {
		int curDigits = Digits(history->id[i]);
		for(int j=0;j<digits-curDigits;j++) printf(" ");
		printf("%d  ",history->id[i]);
        printf("%s", history->data[i]);
    }
}

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

int isNumber(char s[]){
    for (int i = 0; s[i]!= '\n' && s[i]!='\0' && s[i]!=' '; i++){
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

int main(){
	
	History history;
	initHistory(&history);
	int count=1;

	while(1){
		printf("MTL458>");
		fflush(stdout);
		char command[max_cmd_length];
		fgets(command,max_cmd_length,stdin);

		addCommand(&history, command, count++);

		////////PIPE////////
		int pipefd[2];
		pipe(pipefd);
		int piping = 0;
		int pipepid;
		int stdout_copy = dup(STDOUT_FILENO);
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

			// if(startswith(subcommand,"cd")){
			// 	char path[PATH_MAX];
			// 	strncpy(path,subcommand+3,strlen(subcommand)-4);
			// 	path[strlen(subcommand)-4]='\0'; //Remove \n from path
			// 	int status=chdir(path);
			// 	if(status!=0) perror("cd");
			// }

			// else if(startswith(subcommand,"history")){
			// 	char *pch = strtok(subcommand," ");
			// 	pch = strtok(NULL," ");
			// 	dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe write end
			// 	close(pipefd[0]);
			// 	close(pipefd[1]);
			// 	if(pch==NULL){
			// 		printHistory(&history,0);
			// 	}
			// 	else if(isNumber(pch)){
			// 		int n = atoi(pch);
			// 		printHistory(&history,n);
			// 	}
			// 	else{
			// 		strchr(pch,'\n')[0]='\0';
			// 		printf("history: %s: numeric argument required\n",pch);
			// 	}
			// 	// dup2(stdout_copy, STDOUT_FILENO);
			// }

			// else{
				char *args[max_cmd_length];
				createArgs(args,subcommand);

				pipepid = fork();
				if (pipepid == 0) {
					dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe write end
					close(pipefd[0]);
					close(pipefd[1]);
					execvp(args[0],args);
					perror(args[0]);
					exit(1);
				// }
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
			char *pch = strtok(command," ");
			pch = strtok(NULL," ");
			if(pch==NULL){
				printHistory(&history,0);
			}
			else if(isNumber(pch)){
				int n = atoi(pch);
				printHistory(&history,n);
			}
			else{
				strchr(pch,'\n')[0]='\0';
				printf("history: %s: numeric argument required\n",pch);
			}
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
			exit(1);
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