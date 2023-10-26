#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct node{
    int data;
    struct node *left;
    struct node *right;
    int height;
};

struct node* newNode(int);
int height(struct node*);
int balance(struct node*);
struct node* leftRotate(struct node*);
struct node* rightRotate(struct node*);
void inorder(struct node*);
void preorder(struct node*);
struct node* minNode(struct node*);
struct node* delete(struct node*,int);
struct node* insert(struct node*,int);
int max(int, int);
void contains(struct node*,int);
int startsWith(const char *, const char *);


int main(){
    struct node *root=NULL;
    int x;

    while(1){
        char command[30];
		fgets(command,30,stdin);
        if(startsWith("insert",command)){
            sscanf(command,"insert %d",&x);
            root=insert(root,x);
        }
        else if(startsWith("delete",command)){
            sscanf(command,"delete %d",&x);
            root=delete(root,x);
        }
        else if(startsWith("in order",command)){
            inorder(root);
            printf("\n");
        }
        else if(startsWith("contains",command)){
            sscanf(command,"contains %d",&x);
            contains(root,x);
        }
        else{
            break;
        }
    }
    preorder(root);
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
    struct node* t2 = y->left;
    y->left = z;
    z->right = t2;
    z->height = 1+max(height(z->left),height(z->right));
    y->height = 1+max(height(y->left),height(y->right));
    return y;
}
struct node* rightRotate(struct node* z){
    struct node* y = z->left;
    struct node* t3 = y->right;
    y->right = z;
    z->left = t3;
    z->height = 1+max(height(z->left),height(z->right));
    y->height = 1+max(height(y->left),height(y->right));
    return y;
}
void inorder(struct node* root){
    if(root!=NULL){
        inorder(root->left);
        printf("%d ",root->data);
        inorder(root->right);
    }
}
struct node* minNode(struct node* node){
	while(node->left!=NULL) node = node->left;
	return node;
}
struct node* delete(struct node* root,int data){
    if(root==NULL) return root;
    else if(data<root->data) root->left = delete(root->left,data);
    else if(data>root->data) root->right = delete(root->right,data);
    else{
        if(root->right==NULL && root->left==NULL){
			free(root);
			root = NULL;
		}
		else if(root->left!=NULL && root->right==NULL){
			struct node* temp = root->left;
			root = root->left;
			free(temp);
		}
		else if(root->right!=NULL && root->left==NULL){
			struct node* temp = root->right;
			root = root->right;
			free(temp);
		}
		else{
			struct node* temp = minNode(root->right);
			root->data = temp->data;
			root->right = delete(root->right,temp->data);
		}
    }
    if(root==NULL) return root;
    root->height = 1+max(height(root->left),height(root->right));
    int bal = balance(root);
    if(bal > 1 && balance(root->left) >=0) return rightRotate(root);
	if(bal < -1 && balance(root->right) <=0) return leftRotate(root);
	if(bal > 1 && balance(root->left) < 0){
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}
	if(bal < -1 && balance(root->right) > 0){
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	return root;
}
struct node* insert(struct node* root,int data){

    if(root==NULL) return newNode(data);
	if(data < root->data) root->left = insert(root->left,data);
	else if(data > root->data) root->right = insert(root->right,data);
	else return root;

	root->height = 1+max(height(root->left),height(root->right));

	int bal = balance(root);

	if(bal > 1 && data < root->left->data) return rightRotate(root);
	if(bal < -1 && data > root->right->data) return leftRotate(root);
	if(bal > 1 && data > root->left->data){
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}
	if(bal < -1 && data < root->right->data){
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	return root;
}
void contains(struct node* root,int data){
    if(root==NULL){
        printf("no\n");
        return;
    }
    if(root->data==data){
        printf("yes\n");
        return;
    }
    if(data<root->data) contains(root->left,data);
    else contains(root->right,data);
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