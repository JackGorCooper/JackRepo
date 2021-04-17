#define _GNU_SOURCE

//#include "word_count.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_t node_t;
struct node_t {
    int length;
    int count;
    char* word;
    node_t* next;
    node_t* prev;
};
void* emalloc(size_t n);
node_t* create_node(char* word, int length);
node_t* add_inorder(node_t* list, node_t* new);
void free_list(node_t* list);
//Creates new node. If given a non-null word, adds it, if given non-zero length, adds it
node_t* new_node(char* word, int length){
    node_t* temp = (node_t*)emalloc(sizeof(node_t));  
    
    if(word){
        temp->word = (char*)emalloc((strlen(word)+1) *sizeof(char));
        strncpy(temp->word, word, strlen(word)+1);
    } else {
        temp->word = NULL;
    }
    if (length){
        temp->length = length;
        temp->count = 1;
    } else {
        temp->length = 0;
        temp->count = 0;
    }
    temp->next = NULL;
    temp->prev = NULL;
    
    return temp;
}

node_t* add_inorder(node_t* list, node_t* new){
    node_t* prev = NULL;
    node_t* curr = NULL;
    
    if(list == NULL){
        return new;
    }
   /* iterate through the list,
    if lengths are equal and not zero, then inc that length and free new node
    if new is longer than curr, iterate to the next loop
    if lengths are equal and zero, compare strings
        if strings are the same, free new and return list
        
    */
    if(new->length){ 
        for( curr = list; curr != NULL; curr = curr->next){
            if((new->length == curr->length) && curr->length != 0){
                curr->count++;
                free_list(new);
                return list;
            } else if(new->length > curr->length){
                 prev = curr;

            } else {
                 break;
            }
        }
    } else {
        for ( curr = list; curr != NULL; curr = curr->next){
             if (strlen(new->word) > strlen(curr->word)){
                prev = curr;
            } else if (strlen(new->word) == strlen(curr->word)){
                if (strcmp(new->word, curr->word) == 0){
                    free_list(new);
                    return list;
                }
                else if (strcmp(new->word, curr->word) > 0){
                    prev = curr;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    new->next = curr;
    if(curr){
        curr->prev = new;
    }
    
    if(prev == NULL){
        return new;
    } else {
        prev->next = new;
        new->prev = prev;
        return list;
    } 
}

void* emalloc(size_t n){
    void *p;
    
    p = malloc(n);
    if(p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed",n);
        exit(3);
    }
     
    return p;
}
void free_list(node_t* list){
    node_t* curr= list;
    node_t* prev = NULL;
    
    while(curr){
        prev = curr;
        curr = curr->next;   
        free(prev->word);
        free(prev);
        
    }  
    return;
}


void print_output(node_t* count_list, node_t* word_list, int sort_flag){
    node_t* countp;
    node_t* wordsp = word_list;
    int loops;       
    //Print off the counts
    for (countp = count_list; countp != NULL; countp = countp->next){
        printf("Count[%02d]=%02d; (words:", countp->length, countp->count);
        loops = 0;
        if(sort_flag){
            int last_flag = 0;
            node_t* temp_node = NULL;
            while(strlen(wordsp->word) == countp->length){
                if (wordsp->next){
                    wordsp = wordsp->next;
                } else {
                    last_flag = 1;
                    break;
                }
            } 
            temp_node = wordsp;
            if(wordsp->prev && !last_flag){
                wordsp = wordsp->prev;
            }
            
            for( ;wordsp != NULL; wordsp = wordsp->prev){
               if( strlen(wordsp->word) == countp->length){
                   
                   if((wordsp->prev != NULL) && loops>0){
                       if(strlen(wordsp->prev->word) != countp->length){
                           printf(" and");
                       } else {
                           printf(",");
                       }
                   } else if (loops > 0){
                       printf(" and");
                   }
                   printf(" \"%s\"",wordsp->word);
                   loops++;
                } else {
                   break;
               }   
            }
            wordsp = temp_node;        
        } else {
            for( ;wordsp != NULL; wordsp = wordsp->next){
            
                if( strlen(wordsp->word) == countp->length){
                    if((wordsp->next != NULL)&& loops>0){
                        if(strlen(wordsp->next->word) != countp->length){
                            printf(" and");
                        } else {
                            printf(",");
                        }
                    } else if (loops > 0){
                        printf(" and");
                    }
                    printf(" \"%s\"",wordsp->word);
                    loops++;
                } else {
                    break;
                }   
            }
        }
        printf(")\n"); 
    }
    return;
}
int 
main(int argc, char *argv[]) {
    
    FILE* fp = NULL;
    char* line = NULL;
    char* t = NULL;
    char* filename = NULL;
    size_t len = 0;
    size_t read = 0;
    int sort_flag = 0;

    if(argc<3){
        fprintf(stderr, "Invalid args.\n");
        exit(1);
    }
    for(int i = 1; i<argc; i++){
        if(strcmp(argv[i], "--infile") == 0){
            filename = argv[i+1];
            i++;
            continue;
        }
        if(strcmp(argv[i], "--sort") == 0){
            sort_flag = 1;
        }
    }

    fp = fopen(filename,"r");
    if (fp == NULL){
        fprintf(stderr, "Unable to open file: %s.\n", filename);
        free(fp);
        exit(2);
    }

    node_t* list_of_words = NULL;
    node_t* list_of_lengths = NULL;
    node_t* new_word = NULL;
    node_t* new_length = NULL;

    while ((read = getline(&line, &len, fp)) != -1){
        t = strtok(line, " .,;()\n");
        while(t){
            new_word = new_node(t, 0);
            new_length = new_node(NULL, strlen(t));
            list_of_words = add_inorder(list_of_words, new_word);
            list_of_lengths = add_inorder(list_of_lengths, new_length); 
           t = strtok(NULL, " .,;()\n");
        }
    }
    print_output(list_of_lengths,list_of_words,sort_flag);
    free_list(list_of_lengths);
    free_list(list_of_words);
    free(line);
    fclose(fp);
    
    return 0;
}
        /*
        //case where only one word, simplest case  
        if(countp->count == 1){
            printf(" \"%s\")\n",wordsp->word);
            wordsp = wordsp->next;
            break;
        }
    
        else if (countp->count == 2){
            if( sort_flag){
                printf(" \"%s\" and \"%s\")\n", wordsp->next->word, wordsp->word);
            } else {
                printf(" \"%s\" and \"%s\")\n", wordsp->word, wordsp->next->word);
            }
            wordsp = wordsp->next->next;
        }
        */

/*
So we need to figure out how to implement the sort flag. We have our word list constructed such that
the words are sorted first by length, and secondly by alphanumeric order. We need to print our words out in 
order of length, and by descending alphanumeric order. 

a->b->c->aa->bb->cc->dd->aaa->four->zzzzzzz

Need to print   c b a
                dd cc bb aa
                aaa
                four
                zzzzzzz

without double linked list, we have to implement recursively. 
However, with double links, this problem becomes much simpler, so perhaps I should implement that?
*/


//    while(list_of_lengths){
//        printf("Word length %i has count: %i\n", list_of_lengths->length, list_of_lengths->count);
//        list_of_lengths = list_of_lengths->next;
//    }
//    while(list_of_words){
//        printf("%s\n", list_of_words->word);
//        list_of_words = list_of_words->next;
//   1 }

//            wordp = list_of_words;
//            while(wordp){
//                printf("%s\n",wordp->word);
//                wordp = wordp->next;
//            }
//            printf("\n");
 
// Node which contains a char* and reference to previous and next nodes
/*
typedef struct word_node word_node;
struct word_node {
    char* word;
    word_node* next;
    word_node* prev;
} ;

typedef struct count_node count_node;
struct count_node {
    int word_length;
    int word_count;
    count_node* next;
    count_node* prev;
    word_node* head;
    word_node* tail;
};

word_node* insert_into_word_list(word_node* prev,word_node* new);
*/
/*Start of version 2
//Simple node, one type, contains info for both linked lists.
/ We aim for a struct like:
      w w w w w
      | | | | |
      w w w w w
      | | | | |
      w w w w w
      | | | | | 
list->c-c-c-c-c

 Where c is a count node, and w is a word node. Both share one type.
*/

/* Behold Version 1! A true nightmare.
word_node* new_word_node(char* word){
    printf("new_word_node with word: %s\n",word);
    word_node* temp;
    
    temp = (word_node*)emalloc(sizeof(word_node));
    temp->word = (char *)emalloc((strlen(word)+1) *sizeof(char));
    strncpy(temp->word,  word, strlen(word)+1);
    temp->next = NULL;
    temp->prev = NULL;
    return temp;
}

count_node* new_count_node(int length){
    printf("new_count_node\n");
    count_node* temp;
    
    temp = (count_node*)emalloc(sizeof(count_node));
    
    temp->word_length = length;
    temp->word_count = 1;
    temp->next = NULL;
    temp->prev = NULL;
    temp->head = NULL;
    temp->tail = temp->head;

    return temp;
}
word_node* add_word_node(word_node* listp, word_node* newp){
    printf("add_word_node\n");
    word_node* prev;
    word_node* curr;
    curr = listp;

    if(curr == NULL){
        return newp;
    }
    
    while(curr){
        
        if (strcmp(curr->word, newp->word) == 0){       //Word is in list, return without changes
            return listp;
        } else if (strcmp(curr->word, newp->word) < 0 ){ //Curr is greater than new, add new before
            prev = curr;
            curr = curr->next;   
        } else if (strcmp(curr->word, newp->word) > 0){ 
            break;
        } else {
            break;
        }
    }
    prev = (word_node*)insert_into_word_list(prev,newp);
    printf("can we get here?\n");
    if (listp->next == NULL){
        printf("returning prev\n");
        return prev;
    } else {
        printf("returning listp\n"); 
        return listp;
    }
}
//inserts generic node into doubly linked list.  
word_node* insert_into_word_list(word_node* prev, word_node* new){
        printf("insert_into_word_list\n");
        new->next = prev->next;
        prev->next = new;
        

        if(prev == NULL){
            return new;
        }
        if(prev->next){
             prev->next->prev = new;
        }
        new->next = prev->next;
        new->prev = prev;
        prev->next = new;
        
      printf("exiting insert_into_word\n");  
        return prev;
}
count_node* insert_into_count_list(count_node* prev, count_node* new){
        printf("insert_into_count_list\n");
        new->next = prev->next;
        prev->next = new;
        if(prev == NULL){
            return new;
        }
        if(prev->next){
             prev->next->prev = new;
        }
        new->next = prev->next;
        new->prev = prev;
        prev->next = new;
        
      printf("exiting insert_into_count\n");  
        return prev;
}

count_node* add_count_node(count_node* listp, count_node* newp){
    printf("add_count_node\n");
    count_node* prev;
    count_node* curr;
    curr = listp;

    if(curr == NULL){
        return newp;
    }
    
    while(curr){
        
        if (curr->word_length == newp->word_length){       //Word is in list, return without changes
            return listp;
        } else if (curr->word_length < newp->word_length ){ //Curr is greater than new, add new before
            prev = curr;
            curr = curr->next;   
        } else if (curr->word_length > newp->word_length){ 
            break;
        } else {
            break;
        }
    }
    prev = (count_node*)insert_into_count_list(prev,newp);
    printf("scoobert\n");
    if (listp->next == NULL){
        printf("returning from add_count_node\n");
        return prev;
    } else {
        printf("returning from add_count_node\n"); 
        return listp;
    }
}
      

count_node* update_count_list(count_node* listp, int length, char* word){
    printf("update_count_list\n");
    count_node* p = listp;
    //iterate throught the list, if you find the length in the list, inc it, add word to list
   // printf("%s\n",word);
    while(p){
        if (p->word_length == length){
            p->word_count++;
            p->head = add_word_node(p->head, new_word_node(word));
            return listp;
        }
        p = p->next;
    }
    //we have failed to find a node of that length, we must create a new one, and sort it into list
    printf("Step 1\n");
    count_node* new = (count_node*) new_count_node(length);
    printf("Step 2\n");
    new->head = (word_node*)add_word_node(new->head, (word_node*)new_word_node(word));
    printf("step 3\n");
    listp = (count_node*)add_count_node(listp, new);
    
    printf("I am quite confused\n"); 
    
    return listp;
}
     
void print_words(count_node* listp){
    
    count_node* p;
    word_node* p2;
    if (listp == NULL){
        return;
    }

    for(p = listp; p != NULL; p=p->next){
        printf("Word length %i count %i\n",p->word_length, p->word_count);
        for(p2 = listp->head; p2!=NULL; p2= p2->next){
            printf("%s\n", p2->word);
        }
    } 
    return;
}
*/

