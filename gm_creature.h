#ifndef GM_CREATURE_H
#define GM_CREATURE_H

#include "gm_helper.h"
#include "gm_init.h"
#include "gm_KNN.h"
#include "gm_main.h"
#include "gm_routine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
//#include <mpi.h>
#include <stdbool.h>

//a "creature" has 2 levels, the gene level and the creature level
//the gene level are the genes of the creatures (I.E for MNIST, individual digits)
//the creature level is the total collection of genes of a creature


//a gene contains a float array of features, and int for the number of features, and a string label
typedef struct Gene {
    float* features;
    int num_features;
    char* label;
} Gene;

//a creature is a collection of genes
//it contains an array of ints which are the indices of the genes it contains
typedef struct Creature {
    int* gene_indices;
    int num_genes;
} Creature;


//Gene functions
Gene* gene_init();
void gene_set(Gene* gene, int num_features, char* label);
//uses omp for large datasets
void gene_fill(Gene* genes[], char* file_name, int num_genes, int num_features);
void gene_free(Gene* gene);



//Creature functions
Creature* creature_init();
void creature_set(Creature* creature, int num_genes);
//uses mpi and omp
void creature_fill(Creature* creatures[], int num_creatures, Gene* genes, int num_features);
void creature_free(Creature* creature);

//helper function
int tokfill(char* buffer, Gene* gene, int num_features);

#endif