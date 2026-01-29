#ifndef GM_HELPER_H
#define GM_HELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "gm_creature.h"

typedef struct attributes_t {
    int num_attributes;
    char** attributes;
} attributes_t;

typedef struct distance_intex_t {
    double distance;
    int index;
} distance_intex_t;

size_t get_attribute_size(char* filename);

int get_num_attributes(char* filename);

void get_attributes(char* filename, attributes_t* attributes);

void free_attributes(attributes_t* attributes);

double accuracy(Creature* creatures, Creature* test_creatures, int num_creatures, int num_test_creatures);

void nth_element(distance_intex_t* distance_index, int length, int n);

#endif