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
	pch = strtok(command,delim);
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

int main(){
	while(1){
		// fclose(fopen("/tmp/mtl458_pipe.txt", "w"));
		printf("MTL458>");
		char command[max_cmd_length];
		fgets(command,max_cmd_length,stdin);

		char *save_ptr1;
		char *subcommand = strtok_r(command,"|",&save_ptr1);
		int pipefd[2];
		pipe(pipefd);
		int pipe=0;

		while(subcommand!=NULL){
			char commandcopy[max_cmd_length];
			strcpy(commandcopy,subcommand);

			if(startswith(subcommand,"cd")){
				char path[PATH_MAX];
				strncpy(path,command+3,strlen(command)-4);
				chdir(path);
				subcommand = strtok_r(NULL,"|",&save_ptr1);
				continue;
			};
			if(startswith(subcommand,"history")){
				FILE *fp = fopen("/tmp/mtl458_pipe.txt","w");
				fprintf(fp,"History\n");
				fclose(fp);
				subcommand = strtok_r(NULL,"|",&save_ptr1);
				continue;
			};

			int words = wordcount(subcommand," \n");
			if(words==0) continue;
			
			//Create args array
			char *args[words+2];
			char *save_ptr2;
			char *pch = strtok_r(commandcopy," \n",&save_ptr2);
			args[0] = pch;
			int i=1;
			while(pch != NULL){
				pch = strtok_r(NULL," \n",&save_ptr2);
				args[i] = pch;
				i++;
			}
			if(pipe)
				args[i-1] = "/tmp/mtl458_pipe.txt";
			args[i] = NULL;

			int process = fork();
			if(process==0){
					close(pipefd[0]);
					dup2(pipefd[1],STDOUT_FILENO);
					execvp(args[0],args);
			}
			else if(process>0){
				close(pipefd[1]);
				char readbuf[1024];
				int bytes_read = 0;
				wait(NULL);
				FILE *fp = fopen("/tmp/mtl458_pipe.txt","w");
				while ((bytes_read = read(pipefd[0], readbuf, sizeof readbuf) > 0)) {
					fprintf(fp,"%s",readbuf);
					printf("%s",readbuf);
				}
				fclose(fp);
			}
			else{
				printf("Error\n");
			}

			subcommand = strtok_r(NULL,"|",&save_ptr1);
			pipe = 1;
		}
		FILE *fp = fopen("/tmp/mtl458_pipe.txt","r");
		char readbuf[1024];
		while(fgets(readbuf,1024,fp)!=NULL){
			printf("%s",readbuf);
		}
		fclose(fp);
	}
	return 0;
}