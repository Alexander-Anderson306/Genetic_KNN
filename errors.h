#ifndef ERRORS_H
#define ERRORS_H

//this lets the Make file specify the size of the file reading buffer
#ifndef BUFF_SIZE
    #define BUFF_SIZE (32 * 1024) 
    //32KB (make cool feature to get optimal size based on num features later)
#endif

#define MALLOC_ERROR "Failed to allocate memory\n"
#define FILE_ERROR "Failed to open file\n"
#define BUFF_ERROR "Buffer too small to hold line.\n Set BUFF_SIZE to a larger value when compiling using -D BUFF_SIZE=<value>\n"
#define GENE_CREATURE_ERROR "Not enough creatures to hold all genes\n"

#endif