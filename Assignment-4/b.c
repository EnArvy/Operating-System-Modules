#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>

struct node{
    int data;
    struct node *left;
    struct node *right;
    int height;
};
struct tree{
    struct node *root;
    int readingRreaders;
    int waitingReaders;
    int writingWriters;
    int waitingWriters;
    pthread_mutex_t Lock;
    pthread_cond_t readStart,writeStart;
};
struct tree Tree;
struct threadArgs{
    int data;
    int choice;
};

struct node* newNode(int);
int startsWith(const char *, const char *);
int height(struct node*);
int balance(struct node*);
struct node* leftRotate(struct node*);
struct node* rightRotate(struct node*);
void preorder(struct node*);
struct node* minNode(struct node*);
int max(int, int);
void startReading(struct tree*);
void finishReading(struct tree*);
void startWriting(struct tree*);
void finishWriting(struct tree*);
int shouldReaderWait(struct tree*);
int shouldWriterWait(struct tree*);

void contains(struct node*,int);
struct node* delete(struct node*,int);
struct node* insert(struct node*,int);
void inorder(struct node*,char*);

void* mythread(void* arg){
    struct threadArgs* args = (struct threadArgs*)arg;
    if(args->choice==1){
        startWriting(&Tree);
        Tree.root = insert(Tree.root,args->data);
        finishWriting(&Tree);
    }
    else if(args->choice==2){
        startWriting(&Tree);
        Tree.root = delete(Tree.root,args->data);
        finishWriting(&Tree);
    }
    else if(args->choice==3){
        startReading(&Tree);
        char* result = malloc(sizeof(char)*4096);
        strcpy(result,"");
        inorder(Tree.root,result);
        printf("%s\n",result);
        finishReading(&Tree);
    }
    else if(args->choice==4){
        startReading(&Tree);
        contains(Tree.root,args->data);
        finishReading(&Tree);
    }
    free(args);
    return NULL;
}
pthread_t inserts[300],deletes[300];
int insertcs,deletecs,clearcs;


int main(){
    char command[30];
    pthread_t p;
    Tree.root = NULL;
    Tree.readingRreaders = Tree.waitingReaders = Tree.writingWriters = Tree.waitingWriters = 0;
    pthread_mutex_init(&Tree.Lock,NULL);
    pthread_cond_init(&Tree.readStart,NULL);
    pthread_cond_init(&Tree.writeStart,NULL);
    insertcs=deletecs=clearcs=0;

    while(1){
		fgets(command,30,stdin);
        if(startsWith("insert",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            sscanf(command,"insert %d",&args->data);
            args->choice = 1;
            pthread_create(&p,NULL,mythread,args);
            inserts[insertcs++] = p;
        }
        else if(startsWith("delete",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            sscanf(command,"delete %d",&args->data);
            args->choice = 2;
            for(;clearcs<insertcs;clearcs++){
                pthread_join(inserts[clearcs],NULL);
            }
            pthread_create(&p,NULL,mythread,args);
            deletes[deletecs++] = p;
        }
        else if(startsWith("in order",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            args->choice = 3;
            pthread_create(&p,NULL,mythread,args);
            deletes[deletecs++] = p;
        }
        else if(startsWith("contains",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            sscanf(command,"contains %d",&args->data);
            args->choice = 4;
            pthread_create(&p,NULL,mythread,args);
            deletes[deletecs++] = p;
        }
        else{
            break;
        }
    }
    for(;clearcs<insertcs&&clearcs<300;clearcs++){
        pthread_join(inserts[clearcs],NULL);
    }
    for(int i=0;i<deletecs&&clearcs<300;i++){
        pthread_join(deletes[i],NULL);
    }
    preorder(Tree.root);
    printf("\n");
    return 0;
}

struct node* newNode(int data){
    struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->data = data;
	temp->left = NULL;
	temp->right = NULL;
	temp->height = 1;
	return temp;
}
int height(struct node* node){
    if(node==NULL) return 0;
    return node->height;
}
int balance(struct node* node){
    if(node==NULL) return 0;
    return height(node->left)-height(node->right);
}
int max(int a, int b){
    return (a>b)?a:b;
}
struct node* leftRotate(struct node* z){
    struct node* y = z->right;
    // pthread_mutex_lock(&y->lock);//write
    struct node* t2 = y->left;
    y->left = z;
    z->right = t2;
    z->height = 1+max(height(z->left),height(z->right));
    y->height = 1+max(height(y->left),height(y->right));
    // pthread_mutex_unlock(&y->lock);//write
    return y;
}
struct node* rightRotate(struct node* z){
    struct node* y = z->left;
    // pthread_mutex_lock(&y->lock);//write
    struct node* t3 = y->right;
    y->right = z;
    z->left = t3;
    z->height = 1+max(height(z->left),height(z->right));
    y->height = 1+max(height(y->left),height(y->right));
    // pthread_mutex_unlock(&y->lock);//write
    return y;
}
struct node* minNode(struct node* node){
	while(node->left!=NULL) node = node->left;
	return node;
}
int startsWith(const char *a, const char *b){
   if(strncmp(a, b, strlen(a)) == 0) return 1;
   return 0;
}
void preorder(struct node* root){
    if(root!=NULL){
        printf("%d ",root->data);
        preorder(root->left);
        preorder(root->right);
    }
}  

void inorder(struct node* root, char* result){
    if(root!=NULL){
        inorder(root->left,result);
        char* temp = malloc(sizeof(char)*10);
        sprintf(temp,"%d",root->data);
        strcat(result,temp);
        strcat(result," ");
        inorder(root->right,result);
    }
}
struct node* delete(struct node* root,int data){
    if(root==NULL) return root;

    // pthread_mutex_lock(&root->lock);//read
    struct node* child;
    int left=0;
    if(data<root->data){
        left=1;
        child = root->left;
    }
    else if(data>root->data){
        child = root->right;
    }
    else{
        // pthread_mutex_unlock(&root->lock);//read
        // pthread_mutex_lock(&root->lock);//write
        if(root->left==NULL || root->right==NULL){
            struct node* temp = root->left?root->left:root->right;
            if(temp==NULL){
                temp = root;
                root = NULL;
            }
            else *root = *temp;
			free(temp);
            return root;
		}
		else{
			struct node* temp = minNode(root->right);
			root->data = temp->data;
            // pthread_mutex_unlock(&root->lock);//write
			root->right = delete(root->right,temp->data);
		}
    }
    // pthread_mutex_unlock(&root->lock);//read
    struct node* temp = delete(child,data);
    // pthread_mutex_lock(&root->lock);//write
    if(left==1) root->left = temp;
    else root->right = temp;
    if(root==NULL) return root;
    root->height = 1+max(height(root->left),height(root->right));
    int bal = balance(root);
    if(bal > 1 && balance(root->left) >=0){
        struct node* temp = rightRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal < -1 && balance(root->right) <=0){
        struct node* temp = leftRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal > 1 && balance(root->left) < 0){
		root->left = leftRotate(root->left);
		struct node*temp = rightRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
	if(bal < -1 && balance(root->right) > 0){
		root->right = rightRotate(root->right);
		struct node* temp = leftRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
    // pthread_mutex_unlock(&root->lock);//write
	return root;
}
struct node* insert(struct node* root,int data){
    if(root==NULL) return newNode(data);

    // pthread_mutex_lock(&root->lock);//write
    struct node* child;
    int left=0;
    if(data<root->data){
        left=1;
        child = root->left;
    }
    else if(data>root->data) child = root->right;
	else{
        // pthread_mutex_unlock(&root->lock);//write
        return root;
    }
    // pthread_mutex_unlock(&root->lock);//write
    child = insert(child,data);
    // pthread_mutex_lock(&root->lock);//write
    if(left==1) root->left = child;
    else root->right = child;

	root->height = 1+max(height(root->left),height(root->right));

	int bal = balance(root);

	if(bal > 1 && data < root->left->data){
        struct node* temp = rightRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal < -1 && data > root->right->data){
        struct node* temp = leftRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal > 1 && data > root->left->data){
		root->left = leftRotate(root->left);
		struct node* temp = rightRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
	if(bal < -1 && data < root->right->data){
		root->right = rightRotate(root->right);
		struct node* temp = leftRotate(root);
        // pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
    // pthread_mutex_unlock(&root->lock);//write
	return root;
}
void contains(struct node* root,int data){
    if(root==NULL){
        printf("no\n");
        return;
    }
    // pthread_mutex_lock(&root->lock);//read
    if(root->data==data){
        printf("yes\n");
        // pthread_mutex_unlock(&root->lock);//read
        return;
    }
    struct node* child;
    if(data<root->data) child = root->left;
    else child = root->right;
    // pthread_mutex_unlock(&root->lock);//read
    contains(child,data);
} 

void startReading(struct tree* Tree){
    pthread_mutex_lock(&Tree->Lock);

    Tree->waitingReaders++;
    while(shouldReaderWait(Tree))
        pthread_cond_wait(&Tree->readStart,&Tree->Lock);
    Tree->waitingReaders--;
    Tree->readingRreaders++;

    pthread_mutex_unlock(&Tree->Lock);
}
void finishReading(struct tree* Tree){
    pthread_mutex_lock(&Tree->Lock);

    Tree->readingRreaders--;
    if(Tree->readingRreaders==0 && Tree->waitingWriters>0) pthread_cond_signal(&Tree->writeStart);

    pthread_mutex_unlock(&Tree->Lock);
}
void startWriting(struct tree* Tree){
    pthread_mutex_lock(&Tree->Lock);
    Tree->waitingWriters++;
    while(shouldWriterWait(Tree))
        pthread_cond_wait(&Tree->writeStart,&Tree->Lock);
    Tree->waitingWriters--;
    Tree->writingWriters++;
    pthread_mutex_unlock(&Tree->Lock);
}
void finishWriting(struct tree* Tree){
    pthread_mutex_lock(&Tree->Lock);

    Tree->writingWriters--;
    if(Tree->waitingWriters>0) pthread_cond_signal(&Tree->writeStart);
    else pthread_cond_broadcast(&Tree->readStart);

    pthread_mutex_unlock(&Tree->Lock);
}
//Following two functions ensure bounded waiting for writers
int shouldReaderWait(struct tree* Tree){
    return Tree->writingWriters>0 || Tree->waitingWriters>0;
}
int shouldWriterWait(struct tree* Tree){
    return Tree->writingWriters>0 || Tree->readingRreaders>0;
}