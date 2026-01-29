#include "gm_KNN.h"
#include "gm_creature.h"
#include "gm_creature.h"
#include <math.h>

/**
 * Calculates the Euclidean distance between two genes.
 *
 * @param gene1 The first gene.
 * @param gene2 The second gene.
 * @return The Euclidean distance between the two genes.
 */
double get_distance(Gene* gene1, Gene* gene2) {
    double distance = 0;
    for(int i = 0; i < gene1->num_features; i++) {
        distance += pow(gene1->features[i] - gene2->features[i], 2);
    }

    return sqrt(distance);
}

double KNN(Creature* creature, Creature* test_creature, Gene* genes, int k) {
    //index 0 for distance, index 1 for gene index
    distance_intex_t distance_index[creature->num_genes];

    //find the k nearist neighbors for each point
    int num_genes = creature->num_genes;
    int num_test_genes = test_creature->num_genes;
    for(int creature_index = 0; creature_index < num_genes; creature_index++) {
        //compute distances to all test genes
        for(int test_creature_index = 0; test_creature_index < num_test_genes; test_creature_index++) {
            distance_index[test_creature_index].distance = get_distance(&genes[creature->gene_indices[creature_index]], &genes[test_creature->gene_indices[test_creature_index]]);
            distance_index[test_creature_index].index = test_creature_index;
        }

        //sort distances
        nth_element(distance_index, num_test_genes, k - 1);

        //now use the k nearist neighbors to classify the new point
        //make a hash map of labels to counts
    }
}