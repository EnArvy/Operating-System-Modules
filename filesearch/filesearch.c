#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>

int found = 0;
char myfile[1000];

void search_directory(char* dir){
    char fullpath[1000];
    DIR *dp;
    struct dirent *ep;     
    dp = opendir(dir);
    struct stat path_stat;
    if (dp != NULL){
        //iterating through files and folders in dir
        while((ep = readdir(dp)) != NULL){
            strcpy(fullpath, dir);strcat(fullpath, ep->d_name);strcat(fullpath,"/");
            //Recusrive search in subfolders
            if (lstat(fullpath, &path_stat) == 0 && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0 && S_ISDIR(path_stat.st_mode)){
                search_directory(fullpath);
            }
            if(strcmp(ep->d_name, myfile) == 0){
                lstat(fullpath, &path_stat);
                strcpy(fullpath, dir);strcat(fullpath, ep->d_name);
                if(S_ISDIR(path_stat.st_mode)) printf("Directory found: %s\n", realpath(fullpath, NULL));
                else if(S_ISLNK(path_stat.st_mode)) printf("Link found: %s\n", realpath(fullpath, NULL));
                else printf("File found: %s\n", realpath(fullpath, NULL));
                found = 1;
            }
        }
        closedir(dp);
    }
    return;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Invalid number of arguments\n");
        return 0;
    }
    strcpy(myfile, argv[1]);
    search_directory("./");
    if(found == 0) printf("The target '%s' was not found in the current directory.\n", argv[1]);  
    return 0;
}