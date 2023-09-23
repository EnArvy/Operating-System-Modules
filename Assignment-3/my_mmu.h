#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
// Include your Headers below


// You are not allowed to use the function malloc and calloc directly .

// Initialize freelist
// struct header{
//     size_t size;
//     struct header* next;
// };
// static struct header* head=NULL;
// void init(){
//     if(head==NULL){
//         head=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
//         head->size=4096-sizeof(struct header);
//         head->next=NULL;
//     }
// }

// Function to allocate memory using mmap
void* my_malloc(size_t size) {
    // Your implementation of my_malloc goes here
    if(size==0)return NULL;
    init();
    
}

// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
    if(nelem==0||size==0)return NULL;
    init();
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr) {
    // Your implementation of my_free goes here
    if(ptr==NULL)return;
    
}

void info(){
    printf("This is a custom memory allocator\n");
}