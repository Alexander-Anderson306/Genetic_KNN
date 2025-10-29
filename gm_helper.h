#ifndef GM_HELPER_H
#define GM_HELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

size_t get_attribute_size(char* filename);

int get_num_attributes(char* filename);

int get_attributes(char* filename, char** attributes);

#endif