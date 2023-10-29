// Multi threaded AVL Tree

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<pthread.h>

struct node{
    int data;
    struct node *left;
    struct node *right;
    int height;
    pthread_mutex_t lock;
};
struct tree{
    struct node *root;
    pthread_mutex_t lock;
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

void contains(struct node*,int);
struct node* delete(struct node*,int);
struct node* insert(struct node*,int);
void inorder(struct node*);

void* mythread(void* arg){
    struct threadArgs* args = (struct threadArgs*)arg;
    if(args->choice==1){
        if(Tree.root==NULL) pthread_mutex_lock(&Tree.lock);
        Tree.root = insert(Tree.root,args->data);
        pthread_mutex_unlock(&Tree.lock);
    }
    else if(args->choice==2) Tree.root = delete(Tree.root,args->data);
    else if(args->choice==3){
        inorder(Tree.root);
        printf("\n");
    }
    else if(args->choice==4) contains(Tree.root,args->data);
    free(args);
    return NULL;
}


int main(){
    char command[30];
    pthread_t p;
    Tree.root = NULL;
    pthread_mutex_init(&Tree.lock,NULL);

    while(1){
		fgets(command,30,stdin);
        if(startsWith("insert",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            sscanf(command,"insert %d",&args->data);
            args->choice = 1;
            pthread_create(&p,NULL,mythread,args);
        }
        else if(startsWith("delete",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            sscanf(command,"delete %d",&args->data);
            args->choice = 2;
            pthread_create(&p,NULL,mythread,args);
        }
        else if(startsWith("in order",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            args->choice = 3;
            pthread_create(&p,NULL,mythread,args);
        }
        else if(startsWith("contains",command)){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            sscanf(command,"contains %d",&args->data);
            args->choice = 4;
            pthread_create(&p,NULL,mythread,args);
        }
        else break;
    }
    preorder(Tree.root);
    printf("\n");
    pthread_exit(NULL);
    return 0;
}

struct node* newNode(int data){
    struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->data = data;
	temp->left = NULL;
	temp->right = NULL;
	temp->height = 1;
    pthread_mutex_init(&temp->lock,NULL);
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
    pthread_mutex_lock(&y->lock);//write
    struct node* t2 = y->left;
    y->left = z;
    z->right = t2;
    z->height = 1+max(height(z->left),height(z->right));
    y->height = 1+max(height(y->left),height(y->right));
    pthread_mutex_unlock(&y->lock);//write
    return y;
}
struct node* rightRotate(struct node* z){
    struct node* y = z->left;
    pthread_mutex_lock(&y->lock);//write
    struct node* t3 = y->right;
    y->right = z;
    z->left = t3;
    z->height = 1+max(height(z->left),height(z->right));
    y->height = 1+max(height(y->left),height(y->right));
    pthread_mutex_unlock(&y->lock);//write
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

void inorder(struct node* root){
    if(root!=NULL){
        pthread_mutex_lock(&root->lock);//read
        inorder(root->left);
        printf("%d ",root->data);
        pthread_mutex_unlock(&root->lock);//read
        inorder(root->right);
    }
}
struct node* delete(struct node* root,int data){
    if(root==NULL) return root;

    pthread_mutex_lock(&root->lock);//read
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
        pthread_mutex_unlock(&root->lock);//read
        pthread_mutex_lock(&root->lock);//write
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
            pthread_mutex_unlock(&root->lock);//write
			root->right = delete(root->right,temp->data);
		}
    }
    pthread_mutex_unlock(&root->lock);//read
    struct node* temp = delete(child,data);
    pthread_mutex_lock(&root->lock);//write
    if(left==1) root->left = temp;
    else root->right = temp;
    if(root==NULL) return root;
    root->height = 1+max(height(root->left),height(root->right));
    int bal = balance(root);
    if(bal > 1 && balance(root->left) >=0){
        struct node* temp = rightRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal < -1 && balance(root->right) <=0){
        struct node* temp = leftRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal > 1 && balance(root->left) < 0){
		root->left = leftRotate(root->left);
		struct node*temp = rightRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
	if(bal < -1 && balance(root->right) > 0){
		root->right = rightRotate(root->right);
		struct node* temp = leftRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
    pthread_mutex_unlock(&root->lock);//write
	return root;
}
struct node* insert(struct node* root,int data){
    if(root==NULL) return newNode(data);

    pthread_mutex_lock(&root->lock);//write
    struct node* child;
    int left=0;
    if(data<root->data){
        left=1;
        child = root->left;
    }
    else if(data>root->data) child = root->right;
	else{
        pthread_mutex_unlock(&root->lock);//write
        return root;
    }
    pthread_mutex_unlock(&root->lock);//write
    child = insert(child,data);
    pthread_mutex_lock(&root->lock);//write
    if(left==1) root->left = child;
    else root->right = child;

	root->height = 1+max(height(root->left),height(root->right));

	int bal = balance(root);

	if(bal > 1 && data < root->left->data){
        struct node* temp = rightRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal < -1 && data > root->right->data){
        struct node* temp = leftRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
    }
	if(bal > 1 && data > root->left->data){
		root->left = leftRotate(root->left);
		struct node* temp = rightRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
	if(bal < -1 && data < root->right->data){
		root->right = rightRotate(root->right);
		struct node* temp = leftRotate(root);
        pthread_mutex_unlock(&root->lock);//write
        return temp;
	}
    pthread_mutex_unlock(&root->lock);//write
	return root;
}
void contains(struct node* root,int data){
    if(root==NULL){
        printf("no\n");
        return;
    }
    pthread_mutex_lock(&root->lock);//read
    if(root->data==data){
        printf("yes\n");
        pthread_mutex_unlock(&root->lock);//read
        return;
    }
    struct node* child;
    if(data<root->data) child = root->left;
    else child = root->right;
    pthread_mutex_unlock(&root->lock);//read
    contains(child,data);
} 