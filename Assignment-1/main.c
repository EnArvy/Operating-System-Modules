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
		char command[1024];
		fgets(command,1024,stdin);

		char *pch;
		pch = strtok(command," \n");
		int words=0;
		while (pch != NULL) {
			words++;
			pch = strtok(NULL, " \n");
		}
		
		char *execfile,*args[words];
		if(words==0) continue;
		if(words==1){
			execfile = strtok(command," \n");
		}
		if(words>1){
			pch = strtok(command," \n");
			execfile = pch;
			int i=0;
			while(pch != NULL){
				pch = strtok(NULL," \n");
				if(pch!=NULL)
				printf("%s\n",pch);
				args[i] = pch;
				i++;
			}
			// args[i] = NULL;
		}
		// printf("%s",args[0]);

		// int process = fork();
		// if(process==0){	
		// 	execvp(execfile,args);
		// }
		// else{
		// 	wait(NULL);
		// }

		// int process = fork();
		// if(process==0){

		// }
	}
	return 0;
}