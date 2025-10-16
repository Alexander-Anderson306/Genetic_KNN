#include "gm_creature.h"
#define MAX_LABEL_LENGTH 16

//this lets the Make file specify the size of the file reading buffer
#ifndef BUFF_SIZE
    #define BUFF_SIZE 4096
#endif
#define LABEL_ERROR "Label too long\n"
#define MALLOC_ERROR "Failed to allocate memory\n"
#define FILE_ERROR "Failed to open file\n"
#define BUFF_ERROR "Buffer too small to hold line.\n Set BUFF_SIZE to a larger value when compiling using -D BUFF_SIZE=<value>\n"

//gene functions

//O(1)
/**
 * Allocates memory for a new gene and initializes its fields.
 *
 * @return A pointer to the newly allocated gene.
 *
 * This function allocates memory for a new gene using malloc and checks if the
 * allocation was successful. If the allocation fails, an error message is
 * printed and the program exits. The number of features of the gene is set to
 * 0 and its label is set to NULL.
 */
Gene* gene_init() {
    //allocate memory for the gene
    Gene* new_gene = (Gene*)malloc(sizeof(Gene));

    //check that the allocation was successful
    if (new_gene == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }

    //set the number of features to 0
    new_gene->num_features = 0;

    //set the label to NULL (no label yet)
    new_gene->label = NULL;

    //return the new gene
    return new_gene;
}


//O(1)
/**
 * Sets the number of features and label for a given gene.
 *
 * @param gene A pointer to the gene to be modified.
 * @param num_features The number of features to be set for the gene.
 * @param label The label to be set for the gene.
 *
 * This function first checks whether the gene has already been assigned a number of
 * features. If not, memory is allocated for the features. If the gene already has
 * features, memory is reallocated for the new number of features. Memory allocation
 * errors are checked. The number of features is then set to the input value.
 *
 * The function then checks if the gene has already been assigned a label. If not,
 * memory is allocated for the label. If the gene already has a label, memory is
 * reallocated for the new label. Memory allocation errors are checked. The function
 * then checks if the input label exceeds the maximum length. If it does, an error
 * message is printed and the program exits. The label is then copied over to the
 * gene.
 *
 * This function returns void.
 */
void gene_set(Gene* gene, int num_features, char* label) {

    //if the number of features assigned to the gene is 0 or less allocate memory for the features
    //otherwise realloc the memory
    if (gene->num_features <= 0) {
        gene->features = (float*)malloc(num_features * sizeof(float));
    } else {
        gene->features = (float*)realloc(gene->features, num_features * sizeof(float));
    }

    //check that the allocation was successful
    if (gene->features == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }

    //set the number of features and the label
    gene->num_features = num_features;

    //if the label is NULL allocate memory for the label
    if (gene->label == NULL) {
        gene->label = (char*)malloc(MAX_LABEL_LENGTH * sizeof(char));   
    }

    //check that the allocation was successful
    if (gene->label == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }

    //check that the label does not exceed the maximum length
    if (strlen(label) >= MAX_LABEL_LENGTH) {
        fprintf(stderr, LABEL_ERROR);
        exit(1);
    }

    //copy label over
    strcpy(gene->label, label);

    //return void
    return (void)0;
}


//note does not handle the attribute labels
//TODO make a function to strip the label from the file

/**
 * Fills an array of genes from a file.
 *
 * The function takes a file and fills an array of genes by tokenizing the
 * file and filling the genes with the tokenized data. If the file is too
 * large to fit in memory, the function reads the file in chunks and
 * processes it sequentially. The function also handles the case where
 * the file is not large enough to fit in memory and needs to be
 * processed in parallel.
 *
 * @param gene An array of genes to be filled.
 * @param file_name The name of the file to be read.
 * @param num_genes The number of genes to be filled.
 * @param num_features The number of features for each gene.
 */
void gene_fill(Gene* gene[], char* file_name, int num_genes, int num_features) {
    FILE* file = fopen(file_name, "r");

    //check that the file was opened successfully
    if (file == NULL) {
        fprintf(stderr, FILE_ERROR);
        exit(1);
    }

    char* buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    int copied_data_index = 0;
    //index of the current gene being worked on
    int cur_gene = 0;

    //read until the end of the file
    while (fread(buffer + copied_data_index, sizeof(char), BUFF_SIZE - copied_data_index, file) != 0) {
        //parse the buffer and get the index of the start of the incomplete line
        //if the buffer is too small to hold the line print an error message and exit (issue for a fuck ton of features)
        int inc_line_start = 0;
        for(int i = 0; i < BUFF_SIZE; i++) {
            if (buffer[i] == '\n') {
                inc_line_start = i + 1;
            }
        }

        if(inc_line_start == 0) {
            fprintf(stderr, BUFF_ERROR);
            exit(1);
        }

        //use the content of the buffer up until the start of the incomplete line
        int buff_index = 0;

        //fill the genes with the content from the buffer
        while(buff_index < inc_line_start) {
            buff_index += tokfill(buffer + buff_index, gene, num_features, cur_gene);
            //move to the next gene
            ++cur_gene;
        }

        //move the incomplete line to the start of the buffer
        memmove(buffer, buffer + inc_line_start, BUFF_SIZE - inc_line_start);

        //keep track of the copied data index
        copied_data_index = BUFF_SIZE - inc_line_start;
    }

    //sometimes there is still data in the buffer that needs to be processed
    if (cur_gene < num_genes) {
        tokfill(buffer, gene, num_features, num_genes - 1);
    }

    //close the file
    fclose(file);

    //free the buffer
    free(buffer);
}


void gene_free(Gene* gene){
    return;
}

/**
 * Tokenizes a buffer and fills a gene with the tokenized data.
 *
 * This function takes a buffer and tokenizes it by splitting it into substrings
 * separated by commas or newlines. The first token is used as the label for a
 * gene, and the remaining tokens are used as the features for the gene.
 *
 * The function keeps track of the index of the buffer and returns the index
 * after filling the gene. This is used to keep track of the position in the buffer
 * while filling the genes.
 *
 * @param buffer The buffer to be tokenized.
 * @param gene The array of genes to be filled.
 * @param num_features The number of features to be set for the gene.
 * @param cur_gene The index of the current gene being filled.
 *
 * @return The index of the buffer after filling the gene.
 */
int tokfill(char* buffer, Gene* gene[], int num_features, int cur_gene) {
    //tokenize the buffer
    char* token = strtok(buffer, ",\n");
    //set the new gene (this token is the label)
    gene_set(gene[cur_gene], num_features, token);
    //keep track of the buff index (+1 for the comma)
    int buff_index = strlen(token) + 1;

    //token is null here for some reason... whats wrong?

    //fill the features of this gene
    for(int i = 0; i < num_features; i++) {
        token = strtok(NULL, ",\n");
        gene[cur_gene]->features[i] = (float)atof(token);

        //keep track of the buff index (+1 for the comma)
        buff_index += strlen(token) + 1;
    }
    return buff_index;
}