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

int wordcount(char *command, char *delim){
	char *pch;//for processing words in sentence
	pch = strtok(command,delim);
	int words=0;
	while (pch != NULL) {
		words++;
		pch = strtok(NULL, delim);
	}
	return words;
}

int main(){
	while(1){
		printf("MTL458>");

		char command[1024];
		fgets(command,1024,stdin);
		char commandcopy[1024];
		strcpy(commandcopy,command);

		int words = wordcount(command," \n");
		if(words==0) continue;
		//Create args array
		char *args[words+1];
		args[words]=NULL;
		if(words==1){
			args[0] = strtok(commandcopy," \n");
		}
		if(words>1){
			char *pch = strtok(commandcopy," \n");
			args[0] = pch;
			int i=1;
			while(pch != NULL){
				pch = strtok(NULL," \n");
				args[i] = pch;
				i++;
			}
		}

		int process = fork();
		if(process==0){

			int found = 0;
			if(access(args[0],X_OK)==0){
				found = 1;
			}
			else{
				char path[PATH_MAX],path2[PATH_MAX];
				strcpy(path,getenv("PATH"));
				strcpy(path2,path);
				int pathsize= wordcount(path,":");
				char paths[pathsize][PATH_MAX];
				char *pch = strtok(path2,":");
				strcpy(paths[0],pch);
				int i=1;
				while(i!=pathsize){
					pch = strtok(NULL,":");
					strcpy(paths[i],pch);
					i++;
				}
				for(int j=0;j!=pathsize;j++){
					char temp[(int)strlen(paths[j])+1+strlen(args[0])];
					strcpy(temp,paths[j]);
					strcat(strcat(temp,"/"),args[0]);
					if(access(temp,X_OK)==0){
						found = 1;
						break;
					}
				}
			}
			
			if(found==1){
				execvp(args[0],args);
			}
			else{
				printf("Command not found\n");fflush(stdout);
			}
		}
		else{
			wait(NULL);
		}
	}
	return 0;
}