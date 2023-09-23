 #include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define your custom malloc, calloc, and realloc functions here
void* my_malloc(size_t size);
void* my_calloc(size_t num_elements, size_t element_size);
void* my_realloc(void* ptr, size_t new_size);

void test_malloc() {
    size_t size = 10;
    void *ptr = my_malloc(size);

    if (ptr == NULL) {
        printf("my_malloc failed.\n");
    } else {
        printf("my_malloc successful. Allocated %zu bytes at address %p.\n", size, ptr);
        // You should have a corresponding free function for your custom malloc.
        // Replace the following line with your custom free function.
        free(ptr);
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

        // Initialize and print the allocated memory
        for (size_t i = 0; i < num_elements; i++) {
            ptr[i] = i;
            printf("ptr[%zu] = %d\n", i, ptr[i]);
        }
        
        // You should have a corresponding free function for your custom calloc.
        // Replace the following line with your custom free function.
        free(ptr);
    }
}

void test_realloc() {
    size_t size1 = 5;
    size_t size2 = 10;
    int *ptr = (int *)my_malloc(size1 * sizeof(int));

    if (ptr == NULL) {
        printf("my_malloc failed.\n");
        return;
    }

    printf("my_malloc successful. Allocated %zu bytes at address %p.\n", size1 * sizeof(int), ptr);

    // Reallocate the memory using your custom realloc function.
    ptr = (int *)my_realloc(ptr, size2 * sizeof(int));

    if (ptr == NULL) {
        printf("my_realloc failed.\n");
    } else {
        printf("my_realloc successful. Now allocated %zu bytes at address %p.\n", size2 * sizeof(int), ptr);

        // Initialize and print the reallocated memory
        for (size_t i = size1; i < size2; i++) {
            ptr[i] = i;
            printf("ptr[%zu] = %d\n", i, ptr[i]);
        }

        // You should have a corresponding free function for your custom realloc.
        // Replace the following line with your custom free function.
        free(ptr);
    }
}

int main() {
    printf("Testing my_malloc:\n");
    test_malloc();

    printf("\nTesting my_calloc:\n");
    test_calloc();

    printf("\nTesting my_realloc:\n");
    test_realloc();

    return 0;
}
