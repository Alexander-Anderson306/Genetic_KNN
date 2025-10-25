#include "gm_creature.h"
#define MAX_LABEL_LENGTH 16

//this lets the Make file specify the size of the file reading buffer
#ifndef BUFF_SIZE
    #define BUFF_SIZE (32 * 1024) 
    //32KB (make cool feature to get optimal size based on num features later)
#endif
#define LABEL_ERROR "Label too long\n"
#define MALLOC_ERROR "Failed to allocate memory\n"
#define FILE_ERROR "Failed to open file\n"
#define BUFF_ERROR "Buffer too small to hold line.\n Set BUFF_SIZE to a larger value when compiling using -D BUFF_SIZE=<value>\n"
#define GENE_CREATURE_ERROR "Not enough creatures to hold all genes\n"
//seed for random number generation (found in gm_main.c)
extern int seed;

//--------------------------------Gene functions----------------------------------

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
 * Fills an array of genes with data from a file.
 *
 * This function takes in an array of genes, a file name, the number of genes to fill, and the number of features per gene.
 * It opens the file and reads it in chunks defined by the buffer size. It then parses the buffer and fills in the genes with the data from the buffer.
 * The function will exit if there is an error opening the file or if the buffer is too small to hold a line.
 *
 * @param genes The array of genes to fill.
 * @param file_name The name of the file to read from.
 * @param num_genes The number of genes to fill.
 * @param num_features The number of features per gene.
 *
 * This function returns void.
 */
void gene_fill(Gene* genes[], char* file_name, int num_genes, int num_features) {
    FILE* file = fopen(file_name, "r");

    //check that the file was opened successfully
    if (file == NULL) {
        fprintf(stderr, FILE_ERROR);
        exit(1);
    }

    char* buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    if(buffer == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }
    int copied_data_index = 0;
    //index of the current gene being worked on
    int cur_gene = 0;

    //read until the end of the file
    while (fread(buffer + copied_data_index, sizeof(char), BUFF_SIZE - copied_data_index, file) != 0 && cur_gene < num_genes) {
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
            buff_index += tokfill(buffer + buff_index, genes[cur_gene], num_features);
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
        tokfill(buffer, genes[cur_gene], num_features);
    }

    //close the file
    fclose(file);

    //free the buffer
    free(buffer);

    return (void)0;
}


/**
 * Frees a gene and its associated memory.
 *
 * This function takes a gene as input and frees the memory associated with
 * the gene's features and label. It then frees the gene itself.
 *
 * @param gene The gene to be freed.
 */
void gene_free(Gene* gene){
    //free the features and label
    free(gene->features);
    free(gene->label);
    //free the gene itself
    free(gene);
    return (void)0;
}

//--------------------------------Creature functions----------------------------------

/**
 * Allocates memory for a new creature and initializes its fields.
 *
 * This function allocates memory for a new creature using malloc and checks if
 * the allocation was successful. If the allocation fails, an error message is
 * printed and the program exits. The number of genes of the creature is set to
 * 0 and its genes array is set to NULL.
 *
 * @return A pointer to the newly allocated creature.
 */
Creature* creature_init() {
    //allocate memory for the creature
    Creature* new_creature = (Creature*)malloc(sizeof(Creature));

    //check that the allocation was successful
    if (new_creature == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }

    //set the number of genes to 0
    new_creature->num_genes = 0;

    //set the genes array to NULL (no genes yet)
    new_creature->gene_indices = NULL;

    //return the new creature
    return new_creature;
}


/**
 * Sets the number of genes for a creature.
 *
 * @param creature The creature for which to set the number of genes.
 * @param num_genes The number of genes to set for the creature.
 */
void creature_set(Creature* creature, int num_genes) {
    //set the number of genes
    creature->num_genes = num_genes;

    //allocate memory for the genes array if needed otherwise realloc
    if (creature->gene_indices == NULL) {
        creature->gene_indices = (int*)malloc(num_genes * sizeof(int));
    } else {
        creature->gene_indices = (int*)realloc(creature->gene_indices, num_genes * sizeof(int));
    }

    //check that the allocation was successful
    if (creature->gene_indices == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }

    return (void)0;
}



/**
 * Fills the creatures with all the genes in an even distribution, then scrambles them.
 *
 * This function takes an array of creatures, the number of creatures, an array of genes, and the
 * number of features as input. It first checks if there are enough creatures to cover every gene,
 * and if not, it exits with an error message. Then it fills the creatures with all the genes in an
 * even distribution using OpenMP parallel for loops. Finally, it scrambles the creatures using unique
 * seeds for each core, also using OpenMP parallel for loops.
 *
 * @param creatures An array of creatures to be filled.
 * @param num_creatures The number of creatures.
 * @param num_genes The number of genes in the global genes array.
 */
void creature_fill(Creature* creatures[], int num_creatures, int num_genes) {
    //not all genes are getting pointers for some reason
    //also memory leak somewhere somehow
    #pragma omp parallel
    {
        //check if there are enough creatures to cover every gene
        int total_space = 0;

        #pragma omp parallel for reduction(+:total_space)
        for (int i = 0; i < num_creatures; i++) {
            total_space += creatures[i]->num_genes;
        }

        // Only one thread checks and prints
        #pragma omp single
        {
            if (total_space < num_genes) {
                fprintf(stderr, GENE_CREATURE_ERROR);
                exit(1);
            }
        }

        #pragma omp barrier

        //first we fill the creatures with all the genes in an even distribution
        int local_count = (num_genes/num_creatures) * omp_get_thread_num();
        #pragma omp for
        for (int i = 0; i < num_creatures; i++) {
            int local_num_genes = creatures[i]->num_genes;
            for (int j = 0; j < local_num_genes; j++) {
                creatures[i]->gene_indices[j] = (local_count++) % num_genes;
            }
        }

        //the first loop must finish before the second loop
        #pragma omp barrier

        //now we scramble the creatures
        #pragma omp for
        for (int i = 0; i < num_creatures; i++) {
            unsigned int unique_seed = seed + i;
            int local_num_genes = creatures[i]->num_genes;

            for (int j = 0; j < local_num_genes; j++) {
                int random_creature = rand_r(&unique_seed) % num_creatures;
                int random_index = rand_r(&unique_seed) % creatures[random_creature]->num_genes;

                //swap
                creatures[i]->gene_indices[j] ^= creatures[random_creature]->gene_indices[random_index];
                creatures[random_creature]->gene_indices[random_index] ^= creatures[i]->gene_indices[j];
                creatures[i]->gene_indices[j] ^= creatures[random_creature]->gene_indices[random_index];
            }
        }
    }

    return (void)0;
}


/**
 * Frees the memory associated with a creature.
 *
 * This function takes a creature as input and frees the memory associated
 * with the creature. It first frees each gene in the creature using
 * gene_free, then frees the genes array, and finally frees the creature
 * itself.
 *
 * @param creature The creature to be freed.
 */
void creature_free(Creature* creature) {
    //free the genes array
    free(creature->gene_indices);
    //free the creature itself
    free(creature);
    return (void)0;
}



/**
 * Tokenizes the buffer and fills the gene with the tokenized values.
 *
 * This function tokenizes the input buffer and fills the gene with the
 * tokenized values. It first tokenizes the label, then fills the features
 * array with the remaining tokens. The function returns the index of the
 * buffer after the last tokenized value.
 *
 * @param buffer The buffer to tokenize.
 * @param gene The gene to fill with the tokenized values.
 * @param num_features The number of features of the gene.
 * @return The index of the buffer after the last tokenized value.
 */
int tokfill(char* buffer, Gene* gene, int num_features) {
    //tokenize the buffer
    char* token = strtok(buffer, ",\n");
    //set the new gene (this token is the label)
    gene_set(gene, num_features, token);
    //keep track of the buff index (+1 for the comma)
    int buff_index = strlen(token) + 1;

    //fill the features of this gene
    for(int i = 0; i < num_features; i++) {
        token = strtok(NULL, ",\n");
        gene->features[i] = (float)atof(token);

        //keep track of the buff index (+1 for the comma)
        buff_index += strlen(token) + 1;
    }
    return buff_index;
}