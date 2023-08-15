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
	char PATH[PATH_MAX],PATH_Temp[PATH_MAX];
	strcpy(PATH,getenv("PATH"));
	strcpy(PATH_Temp,PATH);
	int PATH_size= wordcount(PATH_Temp,":");

	while(1){
		printf("MTL458>");

		char command[max_cmd_length];
		fgets(command,max_cmd_length,stdin);

		char *save_ptr1;
		char *subcommand = strtok_r(command,"|",&save_ptr1);
		while(subcommand!=NULL){

			char commandcopy[max_cmd_length];
			strcpy(commandcopy,subcommand);

			if(startswith(subcommand,"cd")){
				char path[PATH_MAX];
				strncpy(path,command+3,strlen(command)-4);
				chdir(path);
				continue;
			};
			if(startswith(subcommand,"history")){
				printf("true\n");
				continue;
			};

			int words = wordcount(subcommand," \n");
			if(words==0) continue;
			
			//Create args array
			char *args[words+1];
			char *save_ptr2;
			char *pch = strtok_r(commandcopy," \n",&save_ptr2);
			args[0] = pch;
			int i=1;
			while(pch != NULL){
				pch = strtok_r(NULL," \n",&save_ptr2);
				args[i] = pch;
				i++;
			}

			int process = fork();
			if(process==0){
				int found = 0;
				if(access(args[0],X_OK)==0){
					found = 1;
				}
				else if(strchr(args[0],'/')==NULL){
					strcpy(PATH_Temp,PATH);
					char *save_ptr3;
					char *Pch = strtok_r(PATH_Temp,":",&save_ptr3);
					for(int j=1;j<PATH_size;j++){
						char temp[PATH_MAX];
						strcpy(temp,Pch);
						strcat(temp,"/");
						strcat(temp,args[0]);
						if(access(temp,X_OK)==0){
							found = 1;
							break;
						}
						Pch = strtok_r(NULL,":",&save_ptr3);
					}
				}
				
				if(found==1){
					execvp(args[0],args);
				}
				else{
					printf("Command not found\n");fflush(stdout);
				}
			}
			else if(process>0){
				wait(NULL);
			}
			else{
				printf("Error\n");
			}
			subcommand = strtok_r(NULL,"|",&save_ptr1);
		}
	}
	return 0;
}