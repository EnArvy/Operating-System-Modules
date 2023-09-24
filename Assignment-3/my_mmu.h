#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>



//Initialize freelist
struct header{
    size_t size;
    struct header* next;
};
struct allocatedheader{
    size_t size;
    int magic;
};
static struct header* freelist=NULL;
void init(){
    if(freelist==NULL){
        freelist=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        freelist->size=4096-sizeof(struct header);
        freelist->next=NULL;
    }
}
//Function to show state of freelist
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



//Function to check int overflow on multiplication
int checkoverflow(int m, int n){
    if(m==0)return 0;
    int p=m*n;
    if(p/n==m)return 0;
    else return 1;
}



// Own implemetnation of malloc
void* my_malloc(size_t size) {
    // Your implementation of my_malloc goes here
    if(size==0)return NULL;
    init();
    
    struct header* prev = NULL;
    struct header* current = freelist;

    while (current) {
        if (current->size + sizeof(struct header) >= size + sizeof(struct allocatedheader)) {
            if (current->size+ sizeof(struct header) > size + sizeof(struct allocatedheader)) {
                // Split the block if it's larger than needed.
                struct header* new_block = (struct header*)((char*)current + size + sizeof(struct allocatedheader));
                new_block->size = current->size - size + sizeof(struct header) - sizeof(struct allocatedheader);
                new_block->next = current->next;
                struct allocatedheader* allocatedmem = (struct allocatedheader*)current;
                allocatedmem->size = size;
                allocatedmem->magic = 123456;
                if (prev)
                    prev->next = new_block;
                else
                    freelist = new_block;
                return (void*)(allocatedmem + 1);
            }
            else{
                if (prev)
                    prev->next = current->next;
                else
                    freelist = current->next;
                struct allocatedheader* allocatedmem = (struct allocatedheader*)current;
                allocatedmem->magic = 123456;
                allocatedmem->size = size;
                return (void*)(allocatedmem + 1);
            }
        }
        prev = current;
        current = current->next;
    }

    // If no suitable block is found, request more memory.
    struct allocatedheader* new_block = sbrk(size+sizeof(struct allocatedheader));
    if (new_block == (void*)-1) {
        errno = ENOMEM;
        return NULL; // Out of memory
    }
    new_block->size = size;
    new_block->magic = 123456;
    return (void*)(new_block + 1);
}

// Own implementation of calloc
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

// Own implementation of free
void my_free(void* ptr) {
    // Your implementation of my_free goes here
    if(ptr==NULL)return;
    struct allocatedheader* block = (struct allocatedheader*)ptr - 1;
    if(block->magic!=123456) return;
    size_t size = block->size;
    struct header* freemem = (struct header*)block;
    freemem->next = freelist;
    freelist = freemem;
    freemem->size = size - sizeof(struct header) + sizeof(struct allocatedheader);
}