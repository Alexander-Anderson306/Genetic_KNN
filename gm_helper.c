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