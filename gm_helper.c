#include "gm_helper.h"
#include "errors.h"




/**
 * @brief Returns the size of the header of afile in bytes.
 *
 *
 * @param filename The name of the file to get the header size of.
 * @return The size of the header file in bytes.
 */
size_t get_attribute_size(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file = NULL) {
        fprintf(stderr, FILE_ERROR);
        exit(1);
    }

    //skip to the end of the first line
    while(fgetc(file) != "\n");

    size_t size = ftell(file);
    fclose(file);
    return size;
}

/**
 * @brief Returns the number of attributes in a file.
 *
 * Opens the file and counts the number of commas on the first line. This is
 * assumed to be the number of attributes.
 *
 * @param filename The name of the file to get the number of attributes of.
 * @return The number of attributes in the file.
 */
int get_num_attributes(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file = NULL) {
        fprintf(stderr, FILE_ERROR);
        exit(1);
    }

    int num_attributes = 0;
    char c;
    while((c = fgetc(file)) != '\n') {
        if(c == ',') {
            num_attributes++;
        }
    }
    fclose(file);

    if(num_attributes == 0) {
        fprintf(stderr, EMPTY_FILE_ERROR);
        exit(1);
    }

    //last attribute doesn't have a comma
    num_attributes++;

    return num_attributes;
}

/**
 * @brief Gets the attributes of a file and stores them in the attributes_t struct.
 *
 * Opens the file and reads the first line into a buffer. It then tokenizes the buffer
 * and stores the tokens in the attributes_t struct.
 *
 * @param filename The name of the file to get the attributes of.
 * @param attributes A pointer to an attributes_t struct to store the attributes in.
 *
 * This function returns void.
 */
void get_attributes(char* filename, attributes_t* attributes) {
    FILE* file = fopen(filename, "r");
    if (file = NULL) {
        fprintf(stderr, FILE_ERROR);
        exit(1);
    }

    size_t size = get_attribute_size(filename);
    char* buffer = (char*)malloc(size * sizeof(char));
    if(buffer == NULL) {
        fprintf(stderr, MALLOC_ERROR);
        exit(1);
    }

    fread(buffer, size, 1, file);
    fclose(file);

    char* token = strtok(buffer, ",\n");
    attributes->num_attributes = get_num_attributes(filename);
    attributes->attributes = (char**)malloc(attributes->num_attributes * sizeof(char*));
    for(int i = 0; i < attributes->num_attributes; i++) {
        attributes->attributes[i] = (char*)malloc(strlen(token) * sizeof(char));
        strcpy(attributes->attributes[i], token);
        token = strtok(NULL, ",\n");
    }

    free(buffer);

    return (void)0;
}

/**
 * Frees the memory associated with an attributes_t struct.
 *
 * @param attributes The attributes_t struct to free.
 *
 * This function returns void.
 */
void free_attributes(attributes_t* attributes) {
    for(int i = 0; i < attributes->num_attributes; i++) {
        free(attributes->attributes[i]);
    }
    free(attributes->attributes);
    return (void)0;
}

void swap(distance_intex_t* a, distance_intex_t* b) {
    distance_intex_t swap = *a;
    *a = *b;
    *b = swap; 
}

int select_pivot(distance_intex_t* distance_intex_t, int low, int high) {
    int middle = low + (high - low) / 2;
    double p1 = distance_intex_t[low].distance;
    double p2 = distance_intex_t[middle].distance;
    double p3 = distance_intex_t[high].distance;
    if ((p1 <= p2 && p2 <= p3) || (p3 <= p2 && p2 <= p1)) {
        return middle;
    } else if ((p2 <= p1 && p1 <= p3) || (p3 <= p1 && p1 <= p2)) {
        return low;
    } else {
        return high;
    }
}

/**
 * Partitions the array such that the pivot element ends up in its correct place.
 * All elements to the left of the pivot are less than the pivot, and all elements to
 * the right of the pivot are greater than the pivot.
 *
 * @param distance_index the distance type array to be partitioned.
 * @param low The index of the first element of the sub-array to partition.
 * @param high The index of the last element of the sub-array to partition.
 *
 * @return The index of the pivot element after partitioning.
 */
int partition(distance_intex_t* distance_index, int low, int high) {
    //select pivot
    int pivot_index = select_pivot(distance_index, low, high);
    double pivot = distance_index[pivot_index].distance;
    //swap pivot with the last element
    swap(&distance_index[high], &distance_index[pivot_index]);

    //lomuto partition (so pivot ends up in the correct place)
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (distance_index[j].distance < pivot) {
            i++;
            swap(&distance_index[i], &distance_index[j]);
        }
    }
    //swap pivot with the correct place
    swap(&distance_index[i + 1], &distance_index[high]);
    return i + 1;
}


/**
 * Sorts the array such that all elements to the left of the nth element are less than the nth element,
 * and all elements to the right of the nth element are greater than the nth element.
 *
 *
 * @param distance_index The array to sort.
 * @param length The length of the array.
 * @param n The index of the nth element.
 */
void nth_element(distance_intex_t* distance_index, int length, int n) {
    int low = 0;
    int high = length - 1;
    while (low <= high) {
        int pivot = partition(distance_index, low, high);
        if (pivot == n) {
            return;
        } else if (pivot < n) {
            low = pivot + 1;
        } else {
            high = pivot - 1;
        }
    }
}