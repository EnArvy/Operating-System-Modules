#include <stdio.h>
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
int main(){
	while(1){
		printf("MTL458>");
		char command[1024];
		fgets(command,1024,stdin);
		char commandcopy[1024];
		strcpy(commandcopy,command);

		//Count words
		char *pch;//for processing words in sentence
		pch = strtok(command," \n");
		int words=0;
		while (pch != NULL) {
			words++;
			pch = strtok(NULL, " \n");
		}

		//Create args array
		char *args[words+1];
		args[words]=NULL;
		if(words==0) continue;
		if(words==1){
			args[0] = strtok(command," \n");
		}
		if(words>1){
			pch = strtok(commandcopy," \n");
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
			if(strcmp(args[0],"ls")||strcmp(args[0],"echo")||strcmp(args[0],"cat")||strcmp(args[0],"sleep"))
				execvp(args[0],args);
		}
		else{
			wait(NULL);
		}
	}
	return 0;
}