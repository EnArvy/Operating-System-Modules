#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_mmu.h"

void test_malloc() {
    size_t size = 10;
    void *ptr = my_malloc(size);

    if (ptr == NULL) {
        printf("my_malloc failed.\n");
    } else {
        printf("my_malloc successful. Allocated %zu bytes at address %p.\n", size, ptr);
        info();
        // You should have a corresponding free function for your custom malloc.
        // Replace the following line with your custom free function.
        my_free(ptr);
        printf("Freed\n");
        info();
    }
}

void test_calloc() {
    size_t num_elements = 5;
    size_t element_size = sizeof(int);
    int *ptr = (int *)my_calloc(num_elements, element_size);

    if (ptr == NULL) {
        printf("my_calloc failed.\n");
    } else {
        printf("my_calloc successful. Allocated %zu bytes at address %p.\n", num_elements * element_size, ptr);

        for (size_t i = 0; i < num_elements; i++) {
            printf("ptr[%zu] = %d\n", i, ptr[i]);
        }
        printf("\n");
        // Initialize and print the allocated memory
        for (size_t i = 0; i < num_elements; i++) {
            ptr[i] = i;
            printf("ptr[%zu] = %d\n", i, ptr[i]);
        }
        info();
        
        // You should have a corresponding free function for your custom calloc.
        // Replace the following line with your custom free function.
        my_free(ptr);
        printf("Freed\n");
        info();
    }
}

int main() {
    printf("Testing my_malloc:\n");
    test_malloc();

    printf("\nTesting my_calloc:\n");
    test_calloc();

    return 0;
}
