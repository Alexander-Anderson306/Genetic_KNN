#include "gm_KNN.h"
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

double KNN(Creature* creature, Creature* test_creature, int k) {
    
}