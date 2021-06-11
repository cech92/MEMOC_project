#ifndef SIMULATED_ANNEALING_SOLVER_H
#define SIMULATED_ANNEALING_SOLVER_H

#include "../problem.h"

class SimulatedAnnealingSolver {
private:
    Problem* problem;
    vector<vector<double>> costs;
    unsigned int N;

    unsigned int max_iterations;
    unsigned int max_execution_time;
    time_t stop_time;

//    vector<double> generation_results;

    double min_length;
    vector<int> best_solution;

    //simulated annealing
    double temperature;
    double temperature_max;
    double temperature_min;
public:
    SimulatedAnnealingSolver(Problem* problem, unsigned int max_iterations, unsigned int max_execution_time);
    void solve();
};

#endif
