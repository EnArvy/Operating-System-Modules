#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

void test_my_malloc_and_free() {
    // Test my_malloc and my_free for various scenarios
    int* int_ptr = (int*)my_malloc(sizeof(int));
    if (int_ptr != NULL) {
        *int_ptr = 42;
        printf("Allocated an int with my_malloc: %d\n", *int_ptr);
        my_free(int_ptr);
    } else {
        printf("my_malloc failed to allocate memory.\n");
    }

    // Test freeing a null pointer (should not cause issues)
    int* null_ptr = NULL;
    my_free(null_ptr);


    // Test double-free (should result in an error)
    int* double_free_ptr = (int*)my_malloc(sizeof(int));
    my_free(double_free_ptr);
    my_free(double_free_ptr); // Double-free should produce an error message

    // Test free on a pointer not obtained from my_malloc
    int stack_var = 10;
    int* stack_ptr = &stack_var;
    my_free(stack_ptr); // This should produce an error message
    

    // Test my_malloc and my_free for large memory blocks
    size_t large_size = 1000000;
    char* large_char_array = (char*)my_malloc(large_size);
    if (large_char_array != NULL) {
        printf("Allocated a large char array with my_malloc.\n");
        memset(large_char_array, 'A', large_size); // Initialize with 'A's
        my_free(large_char_array);
    } else {
        printf("my_malloc failed to allocate a large memory block.\n");
    }
}

void test_my_calloc() {
    // Test my_calloc for various scenarios
    size_t num_elements = 5;
    size_t element_size = sizeof(int);
    int* int_array = (int*)my_calloc(num_elements, element_size);
    
    if (int_array != NULL) {
        printf("Allocated an int array with my_calloc:\n");
        for (size_t i = 0; i < num_elements; i++) {
            printf("%d ", int_array[i]);
        }
        printf("\n");
        info();
        my_free(int_array);
        printf("Freed\n");
    } else {
        printf("my_calloc failed to allocate memory.\n");
    }
}

int main() {
    printf("Testing my_malloc:\n");
    test_malloc();

    printf("\nTesting my_calloc:\n");
    test_calloc();

    printf("\nTesting my_malloc and my_free:\n");
    test_my_malloc_and_free();

    printf("\nTesting my_calloc:\n");
    info();
    test_my_calloc();
    info();

    int *str = (int *)my_malloc(16);
    info();
    // printf("Allocated a string with my_malloc: %s\n", str);
    // strcpy(str, "Hello");
    // printf("Allocated a string with my_malloc: %s\n", str);
    my_free(str);
    info();

    return 0;
}
