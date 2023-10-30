#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>

int startsWith(const char*,const char*);
struct threadArgs{
    struct status* Status;
    int choice;
    char data[4096];
};

struct status{
    char filename[50];
    int readingRreaders;
    int waitingReaders;
    int writingWriters;
    int waitingWriters;
    pthread_mutex_t Lock;
    pthread_cond_t readStart,writeStart;
};
char* readFile(struct status*);
void writeFile(struct status*,char*);
void startReading(struct status*);
void finishReading(struct status*);
void startWriting(struct status*);
void finishWriting(struct status*);
int shouldReaderWait(struct status*);
int shouldWriterWait(struct status*);
int getReaders();
int getWriters();

struct node{
    struct status Status;
    struct node* next;
};
struct node* head;
struct status* findNode(char* file){
    struct node* temp;
    struct node* found=NULL;
    temp=head;
    while(temp!=NULL){
        if(strcmp(temp->Status.filename,file)==0){
            found=temp;
            break;
        }
        temp=temp->next;
    }
    if(found==NULL){
        found = malloc(sizeof(struct node));
        found->next = head;
        head = found;
        found->Status.readingRreaders = 0;
        found->Status.waitingReaders = 0;
        found->Status.writingWriters = 0;
        found->Status.waitingWriters = 0;
        pthread_mutex_init(&found->Status.Lock,NULL);
        pthread_cond_init(&found->Status.readStart,NULL);
        pthread_cond_init(&found->Status.writeStart,NULL);
        strcpy(found->Status.filename,file);
    }
    return &found->Status;
}

void* mythread(void *arg){
    struct threadArgs* args = (struct threadArgs*)arg;
    if(args->choice==1){
        readFile(args->Status);
    }
    else{
        if(args->choice==2){
            char* input = readFile(findNode(args->data));
            writeFile(args->Status,input);
        }
        else{
            writeFile(args->Status,args->data);
        }
    }
    free(args);
    return NULL;
}


int main(){
    char command[2000];
    pthread_t p;
    head = NULL;

    while(1){
        fgets(command,2000,stdin);
        if(startsWith("read",command)){
            struct threadArgs* args = (struct threadArgs*)malloc(sizeof(struct threadArgs));
            char filename[50];
            sscanf(command,"read %s",filename);
            args->choice = 1;
            args->Status = findNode(filename);
            pthread_create(&p,NULL,mythread,args);
        }
        else if(startsWith("write",command)){
            struct threadArgs* args = (struct threadArgs*)malloc(sizeof(struct threadArgs));
            int choice;
            char filename[50];
            sscanf(command,"write %d %s %[^\n]",&choice,filename,args->data);
            if(choice==1){
                findNode(args->data);
                args->choice = 2;
            }
            else{
                args->choice = 3;
            }
            args->Status = findNode(filename);
            pthread_create(&p,NULL,mythread,args);
        }
        else break;
    }
    pthread_exit(NULL);
    return 0;
}

char* readFile(struct status* Status){
    startReading(Status);
    FILE *f = fopen(Status->filename, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    string[fsize] = 0;
    printf("read %s of %ld bytes with %d readers and %d writers present\n",Status->filename,fsize,getReaders(),getWriters());
    finishReading(Status);
    return string;
}
void writeFile(struct status* Status, char* input){
    startWriting(Status);
    FILE* file = fopen(Status->filename, "rb");
    if (file == NULL) {
        return; // error opening file
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    int empty=0;
    FILE *f = fopen(Status->filename, "ab");
    if(size!=0){
        empty=1;
        fwrite("\n", 1, 1, f);
    }
    fwrite(input, strlen(input), 1, f);
    fclose(f);
    printf("writing to %s added %ld bytes with %d readers and %d writers present\n",Status->filename,strlen(input)+empty,getReaders(),getWriters());
    finishWriting(Status);
}
void startReading(struct status* Status){
    pthread_mutex_lock(&Status->Lock);

    Status->waitingReaders++;
    while(shouldReaderWait(Status))
        pthread_cond_wait(&Status->readStart,&Status->Lock);
    Status->waitingReaders--;
    Status->readingRreaders++;

    pthread_mutex_unlock(&Status->Lock);
}
void finishReading(struct status* Status){
    pthread_mutex_lock(&Status->Lock);

    Status->readingRreaders--;
    if(Status->readingRreaders==0 && Status->waitingWriters>0) pthread_cond_signal(&Status->writeStart);

    pthread_mutex_unlock(&Status->Lock);
}
void startWriting(struct status* Status){
    pthread_mutex_lock(&Status->Lock);
    Status->waitingWriters++;
    while(shouldWriterWait(Status))
        pthread_cond_wait(&Status->writeStart,&Status->Lock);
    Status->waitingWriters--;
    Status->writingWriters++;
    pthread_mutex_unlock(&Status->Lock);
}
void finishWriting(struct status* Status){
    pthread_mutex_lock(&Status->Lock);

    Status->writingWriters--;
    if(Status->waitingWriters>0) pthread_cond_signal(&Status->writeStart);
    else pthread_cond_broadcast(&Status->readStart);

    pthread_mutex_unlock(&Status->Lock);
}
//Following two functions ensure bounded waiting for writers
int shouldReaderWait(struct status* Status){
    return Status->writingWriters>0 || Status->waitingWriters>0;
}
int shouldWriterWait(struct status* Status){
    return Status->writingWriters>0 || Status->readingRreaders>0;
}
int startsWith(const char *a, const char *b){
   if(strncmp(a, b, strlen(a)) == 0) return 1;
   return 0;
}
int getReaders(){
    int count=0;
    struct node* temp = head;
    while(temp!=NULL){
        count+=temp->Status.readingRreaders;
        temp=temp->next;
    }
    return count;
}
int getWriters(){
    int count=0;
    struct node* temp = head;
    while(temp!=NULL){
        count+=temp->Status.writingWriters;
        temp=temp->next;
    }
    return count;
}