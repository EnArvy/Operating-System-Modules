#include<stdio.h>

int main(int argc, char **argv){
	printf("%d\n", argc);

	FILE *input = fopen(argv[1],"r");
	FILE *output = fopen(argv[2],"w");

	for(int i=0;i<argc;i++)
		fprintf(output,"%s\n",argv[i]);

	return 0;
}