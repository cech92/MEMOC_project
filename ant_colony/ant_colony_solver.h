#ifndef ANT_COLONY_SOLVER_H
#define ANT_COLONY_SOLVER_H

#include "../problem.h"

class AntColonySolver {
private:
    Problem* problem;
    vector<vector<double>> costs;
    unsigned int num_cities;
    unsigned int num_ants;

    unsigned int max_iterations;
    unsigned int max_execution_time;

    double alpha;
    double beta;
    double rho;

    vector<vector<double>> pheromones_matrix;
    vector<vector<double>> visibility_matrix;
    vector<vector<int>> ants_paths;
    vector<vector<double>> ants_distances;

    double min_length;
    vector<int> solution;
public:
    AntColonySolver(Problem* problem, unsigned int num_ants, double alpha, double beta, double rho, unsigned int max_iterations, unsigned int max_execution_time);
    void solve();
};

#endif
