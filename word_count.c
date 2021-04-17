#include "word_count.h" 

int
real_main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stdout, "program: missing '--infile <filename> [--sort] [--print-words]'\n");
    return 0;
    }
    
    //flag seeker
    
    int sort_flag = 0, print_flag = 0;
    char *infile = "";
    for (int i = 1; i < argc;i++){
        if(!(strcmp("--sort", argv[i]))){
            sort_flag = 1;
        } else if(!(strcmp("--print-words",argv[i]))){
            print_flag = 1;
        } else if(!(strcmp("--infile",argv[i]))){
            i++;
            infile = argv[i];
        }
    }
    if(infile == NULL || !(strcmp("--print-words", infile)) || !(strcmp("--sort",infile))){
        fprintf(stdout, "program: missing '--infile <filename> [--sort] [--print-words]'\n");
        return 0;
    }
    char buffer[MAX_FILESIZE]= {'\0'};
    size_t bytes = read_file(infile,buffer);
    if(!bytes){
        return 0;
    }

   // word_t bucket = {.histogram = {0}, .counter = 0, .sorted_indices = {0},.has_been_sorted =0, .median = 0.0, .all_words = {'\0'}, .sorted_word_pos = {0} };
    word_t bucket;
    word_t *bpoint= &bucket;
    struct_initialize(bpoint);
    tokenize_string(buffer,bytes,bpoint);
    if(sort_flag){
        selection_sort(bpoint);
        print_histogram(bpoint, print_flag);
        if(!print_flag){
            print_median(bpoint, bucket.counter);
        }
    } else {
        print_histogram(bpoint, print_flag);
    }
    return 0;
}


#ifndef TEST
int
main(int argc, char *argv[]) {
    return real_main(argc, argv);
}
#endif
