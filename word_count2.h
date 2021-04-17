/*
 * linkedlist.h
 *
 * This header file contains examples of how to:
 *
 * a) Dynamically allocate memory in a function
 * b) Free memory used by a linked list
 * c) Print the contents of a linked list 
 * d) Apply a function to a linked list
 *
 * All the linked lists are set to void*.  You may replace them
 * with your own struct typedefs.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define EXAMPLE_ERROR_CODE 1
/* Node which contains word length and
    count of that length. Contains reference to the next node
    and to the head and tail of list of words of this length.
*/
typedef struct count_node {
    int word_length;
    int word_count;
    count_node* next;
    word_node* head;
    word_node* tail;
} count_node;
/* Node which contains a word and
    a reference to the next element in the list
*/
typedef struct word_node {
    char* word;
    word_node* right;
    word_node* left;
} word_node;

/* Dynamically allocate memory of size_t n and return the 
 * pointer to the memory address
 *
 * exit with return code on allocation error 
 */
void *emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(EXAMPLE_ERROR_CODE);
    }

    return p;
}
/* Example of how to free memory used by a double-linked list
 */
void cleanup_list(void *list) {
    if(list == NULL)
       return;
    cleanup_list(list->left);
    free(list->word);
    cleanup_list(list->right);
    free(list);
}

/* Example of how to print a linked list
 */
void print_list(void* list)
{    
    if (list == NULL) {
        return;
    }
    
    print_list(list->left);
    fprintf(stderr, "%s ", list->word); 
    print_list(list->right);
}

/* Example of how to apply a function to a each node in a linked list.  This is made so that different functions/arguments can be used.
 *
 * void *list : head of linked list to apply function
 * void (*fn)(void *list, void*) : function that takes in two arguments.
 * void *arg : second argument to pass to argument
 */
void apply(void *list,
           void (*fn)(void *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}

