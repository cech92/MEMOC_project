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
    std::chrono::steady_clock::time_point stop_time;

    double alpha;
    double beta;
    double rho;
    double q;

    vector<vector<double>> pheromones_matrix;
    vector<vector<double>> visibility_matrix;
    vector<vector<int>> ants_paths;
    vector<vector<double>> ants_distances;
    vector<double> generation_results;
    double best_generation_result;
    int best_generation_index;

    double min_length;
    vector<int> solution;

    //simulated annealing
    bool with_sa;
    double temperature;
    double temperature_max;
    double temperature_min;
    unsigned int num_sa_iterations;
public:
    AntColonySolver(Problem* problem, unsigned int num_ants, double alpha, double beta, double rho, double q, unsigned int max_iterations, unsigned int max_execution_time, bool with_sa);
    void solve();
};

#endif
