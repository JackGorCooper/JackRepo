/*
 * Skeleton code for CSC 360, Spring 2021,  Assignment #4
 *
 * Prepared by: Michael Zastre (University of Victoria) 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Some compile-time constants.
 */

#define REPLACE_NONE 0
#define REPLACE_FIFO 1
#define REPLACE_LRU  2
#define REPLACE_SECONDCHANCE 3
#define REPLACE_OPTIMAL 4


#define TRUE 1
#define FALSE 0
#define PROGRESS_BAR_WIDTH 60
#define MAX_LINE_LEN 100


/*
 * Some function prototypes to keep the compiler happy.
 */
int setup(void);
int teardown(void);
int output_report(void);
long resolve_address(long, int);
void error_resolve_address(long, int);

long lru_replace(long, int);
long secondchance_replace(long, int);


/*
 * Variables used to keep track of the number of memory-system events
 * that are simulated.
 */
int page_faults = 0;
int mem_refs    = 0;
int swap_outs   = 0;
int swap_ins    = 0;


/*
 * Page-table information. You may want to modify this in order to
 * implement schemes such as SECONDCHANCE. However, you are not required
 * to do so.
 */
struct page_table_entry *page_table = NULL;
struct page_table_entry {
    long page_num;
    int dirty;
    int free;
    long age;
    int sc_bit;
};


/*
 * These global variables will be set in the main() function. The default
 * values here are non-sensical, but it is safer to zero out a variable
 * rather than trust to random data that might be stored in it -- this
 * helps with debugging (i.e., eliminates a possible source of randomness
 * in misbehaving programs).
 */

int size_of_frame = 0;  /* power of 2 */
int size_of_memory = 0; /* number of frames */
int page_replacement_scheme = REPLACE_NONE;

long current_pos = 0;
/*
 * Function to convert a logical address into its corresponding 
 * physical address. The value returned by this function is the
 * physical address (or -1 if no physical address can exist for
 * the logical address given the current page-allocation state.
 */

long resolve_address(long logical, int memwrite)
{
    int i;
    long page, frame;
    long offset;
    long mask = 0;
    long effective;

    /* Get the page and offset */
    page = (logical >> size_of_frame);

    for (i=0; i<size_of_frame; i++) {
        mask = mask << 1;
        mask |= 1;
    }
    offset = logical & mask;

    /* Find page in the inverted page table. */
    frame = -1;
    for ( i = 0; i < size_of_memory; i++ ) {
        if (!page_table[i].free && page_table[i].page_num == page) {
            frame = i;
	    if(memwrite == TRUE){
	    	page_table[i].dirty = memwrite;
	    }
	    //There was a page hit, so set its second_chance bit to 1.
	    page_table[i].sc_bit = 1;
	    //Set its new age
	    page_table[i].age = mem_refs;
            break;
        }
    }

    /* If frame is not -1, then we can successfully resolve the
     * address and return the result. */
    if (frame != -1) {
        effective = (frame << size_of_frame) | offset;
        return effective;
    }


    /* If we reach this point, there was a page fault. Find
     * a free frame. */
    page_faults++;

    for ( i = 0; i < size_of_memory; i++) {
        if (page_table[i].free) {
            frame = i;
            break;
        }
    }

    /* If we found a free frame, then patch up the
     * page table entry and compute the effective
     * address. Otherwise run replacement scheme.
     */
    if (frame != -1) {
        page_table[frame].page_num = page;
        page_table[i].free = FALSE;
	page_table[i].dirty = memwrite;
	page_table[i].sc_bit = 1;
	page_table[i].age = mem_refs;
        swap_ins++;
        effective = (frame << size_of_frame) | offset;
        return effective;
    }

    //Check the replacement scheme, and run appropriately

    //Position 0 will be the first in. Use current_pos to track
    //the first in.
    if (page_replacement_scheme == REPLACE_FIFO){
	    page_table[current_pos].page_num = page;
	    if (page_table[current_pos].dirty == TRUE){
	    	swap_outs++;
	    }
	    page_table[current_pos].dirty = memwrite;
	    swap_ins++;
	    frame = (current_pos << size_of_frame);
	    current_pos = (current_pos + 1) % size_of_memory;
	    effective = frame | offset;
	    return effective;

    } else if (page_replacement_scheme == REPLACE_LRU){
	    frame = lru_replace(page,memwrite);
	    effective = frame | offset;
	    return effective;

    } else if (page_replacement_scheme == REPLACE_SECONDCHANCE){
	    frame = secondchance_replace(page, memwrite);
	    effective = frame | offset;
	    return effective;
    }
	

    return -1;
}

long lru_replace(long page, int memwrite){
	long frame = 0;
	int i, overflowed = FALSE;

	for( i =0; i < size_of_memory; i++){
		if((page_table[i].age < 0) && (overflowed>= 0)){
			overflowed++;
		}
		if((page_table[i].age < page_table[frame].age) &&
				(overflowed <= FALSE)){
			frame = i;
		} else if ((overflowed > FALSE) && 
			   (page_table[i].age > 0) && 
			   (page_table[i].age < page_table[frame].age)){
			frame = i;
			
		}

		if(overflowed == size_of_memory){
			overflowed = -1;
			i = -1;
		}
	}
	
	page_table[frame].page_num = page;
	if(page_table[frame].dirty == TRUE){
		swap_outs++;
	}
	page_table[frame].dirty = memwrite;
	page_table[frame].age = mem_refs;
	swap_ins++;
	frame = ( frame << size_of_frame);
	return frame;
}

long secondchance_replace(long page, int memwrite){
	long frame;
	//Search for a frame to replace. Give second chances to those who deserve
	while(page_table[current_pos].sc_bit == 1){
		page_table[current_pos].sc_bit = 0;
		current_pos = (current_pos + 1) % size_of_memory;
	}
	page_table[current_pos].page_num = page;
	if(page_table[current_pos].dirty == TRUE){
		swap_outs++;
	}
	page_table[current_pos].dirty = memwrite;
	page_table[current_pos].sc_bit = 1;
	swap_ins++;
	frame = (current_pos << size_of_frame);
	current_pos = (current_pos + 1) % size_of_memory;
	return frame;
}

/*
 * Super-simple progress bar.
 */
void display_progress(int percent)
{
    int to_date = PROGRESS_BAR_WIDTH * percent / 100;
    static int last_to_date = 0;
    int i;

    if (last_to_date < to_date) {
        last_to_date = to_date;
    } else {
        return;
    }

    printf("Progress [");
    for (i=0; i<to_date; i++) {
        printf(".");
    }
    for (; i<PROGRESS_BAR_WIDTH; i++) {
        printf(" ");
    }
    printf("] %3d%%", percent);
    printf("\r");
    fflush(stdout);
}


int setup()
{
    int i;

    page_table = (struct page_table_entry *)malloc(
        sizeof(struct page_table_entry) * size_of_memory
    );

    if (page_table == NULL) {
        fprintf(stderr,
            "Simulator error: cannot allocate memory for page table.\n");
        exit(1);
    }

    for (i=0; i<size_of_memory; i++) {
        page_table[i].free = TRUE;
	page_table[i].sc_bit = 0;
    }
	
    current_pos = 0;

    return -1;
}


int teardown()
{

    return -1;
}


void error_resolve_address(long a, int l)
{
    fprintf(stderr, "\n");
    fprintf(stderr, 
        "Simulator error: cannot resolve address 0x%lx at line %d\n",
        a, l
    );
    exit(1);
}


int output_report()
{
    printf("\n");
    printf("Memory references: %d\n", mem_refs);
    printf("Page faults: %d\n", page_faults);
    printf("Swap ins: %d\n", swap_ins);
    printf("Swap outs: %d\n", swap_outs);

    return -1;
}


int main(int argc, char **argv)
{
   /* For working with command-line arguments. */
    int i;
    char *s;

    /* For working with input file. */
    FILE *infile = NULL;
    char *infile_name = NULL;
    struct stat infile_stat;
    int  line_num = 0;
    int infile_size = 0;

    /* For processing each individual line in the input file. */
    char buffer[MAX_LINE_LEN];
    long addr;
    char addr_type;
    int  is_write;

    /* For making visible the work being done by the simulator. */
    int show_progress = FALSE;

    /* Process the command-line parameters. Note that the
     * REPLACE_OPTIMAL scheme is not required for A#3.
     */
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "--replace=", 9) == 0) {
            s = strstr(argv[i], "=") + 1;
            if (strcmp(s, "fifo") == 0) {
                page_replacement_scheme = REPLACE_FIFO;
            } else if (strcmp(s, "lru") == 0) {
                page_replacement_scheme = REPLACE_LRU;
            } else if (strcmp(s, "secondchance") == 0) {
                page_replacement_scheme = REPLACE_SECONDCHANCE;
            } else if (strcmp(s, "optimal") == 0) {
                page_replacement_scheme = REPLACE_OPTIMAL;
            } else {
                page_replacement_scheme = REPLACE_NONE;
            }
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            infile_name = strstr(argv[i], "=") + 1;
        } else if (strncmp(argv[i], "--framesize=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_frame = atoi(s);
        } else if (strncmp(argv[i], "--numframes=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_memory = atoi(s);
        } else if (strcmp(argv[i], "--progress") == 0) {
            show_progress = TRUE;
        }
    }

    if (infile_name == NULL) {
        infile = stdin;
    } else if (stat(infile_name, &infile_stat) == 0) {
        infile_size = (int)(infile_stat.st_size);
        /* If this fails, infile will be null */
        infile = fopen(infile_name, "r");  
    }


    if (page_replacement_scheme == REPLACE_NONE ||
        size_of_frame <= 0 ||
        size_of_memory <= 0 ||
        infile == NULL)
    {
        fprintf(stderr, 
            "usage: %s --framesize=<m> --numframes=<n>", argv[0]);
        fprintf(stderr, 
            " --replace={fifo|lru|optimal} [--file=<filename>]\n");
        exit(1);
    }


    setup();

    while (fgets(buffer, MAX_LINE_LEN-1, infile)) {
        line_num++;
        if (strstr(buffer, ":")) {
            sscanf(buffer, "%c: %lx", &addr_type, &addr);
            if (addr_type == 'W') {
                is_write = TRUE;
            } else {
                is_write = FALSE;
            }

            if (resolve_address(addr, is_write) == -1) {
                error_resolve_address(addr, line_num);
            }
            mem_refs++;
        } 

        if (show_progress) {
            display_progress(ftell(infile) * 100 / infile_size);
        }
    }
    

    teardown();
    output_report();

    fclose(infile);

    exit(0);
}
