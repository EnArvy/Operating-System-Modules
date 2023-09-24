#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
// Include your Headers below
#include <unistd.h>
#include <errno.h>


// You are not allowed to use the function malloc and calloc directly .

//Initialize freelist
struct header{
    size_t size;
    struct header* next;
};
static struct header* freelist=NULL;
void init(){
    if(freelist==NULL){
        freelist=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        freelist->size=4096-sizeof(struct header);
        freelist->next=NULL;
    }
}

//Function to check int overflow on multiplication
int checkoverflow(int m, int n){
    if(m==0)return 0;
    int p=m*n;
    if(p/n==m)return 0;
    else return 1;
}

// Function to allocate memory using mmap
void* my_malloc(size_t size) {
    // Your implementation of my_malloc goes here
    if(size==0)return NULL;
    init();
    
    struct header* prev = NULL;
    struct header* current = freelist;

    while (current) {
        if (current->size >= size) {
            if (current->size > size) {
                // Split the block if it's larger than needed.
                struct header* new_block = (struct header*)(current + size);
                new_block->size = current->size - size;
                new_block->next = current->next;
                current->size = size;
                current->next = NULL;
                if (prev)
                    prev->next = new_block;
                else
                    freelist = new_block;
            }
            else{
                if (prev) {
                    prev->next = current->next;
                } 
                else {
                    freelist = current->next;
                }
                current->next = NULL;
            }
            return (void*)(current + 1);
        }
        prev = current;
        current = current->next;
    }

    // If no suitable block is found, request more memory.
    struct header* new_block = sbrk(size);
    if (new_block == (void*)-1) {
        errno = ENOMEM;
        return NULL; // Out of memory
    }
    new_block->size = size;
    new_block->next = NULL;
    return (void*)(new_block + 1);
}

// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
    if(checkoverflow(nelem,size)==1){
        errno=EOVERFLOW;
        return NULL;
    }
    size_t finalsize = nelem * size;
    void* ptr = my_malloc(finalsize);
    if(ptr==NULL)return NULL;
    memset(ptr, 0, finalsize);
    return ptr;
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr) {
    // Your implementation of my_free goes here
    if(ptr==NULL)return;
    struct header* block = (struct header*)ptr - 1;
    block->next = freelist;
    freelist = block;
}

void info(){
    struct header* current = freelist;
    int block_number = 1;

    fprintf(stderr,"------------------------------------\n");
    fprintf(stderr,"Free List Contents:\n");
    while (current) {
        fprintf(stderr,"Block %d: Address=%p, Size=%zu\n", block_number, current, current->size);
        current = current->next;
        block_number++;
    }
    fprintf(stderr,"------------------------------------\n");
}