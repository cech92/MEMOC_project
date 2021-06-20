#include <iostream>
#include <math.h>
#include <float.h>
#include <numeric>
#include <unordered_set>
#include "simulated_annealing_solver.h"

using namespace std;


SimulatedAnnealingSolver::SimulatedAnnealingSolver(Problem* problem, unsigned int max_iterations, unsigned int max_execution_time) {
    this->problem = problem;
    this->costs = problem->getCosts();
    this->N = problem->getN();
    this->max_iterations = max_iterations;
    this->max_execution_time = max_execution_time;

    this->min_length = DBL_MAX;
    this->stop_time = std::chrono::steady_clock::now() + std::chrono::seconds(this->max_execution_time);

    this->temperature_max = 0.995;
    this->temperature_min = 1e-09;
    this->temperature = temperature_max;

    vector<int> allow_list;
    for (int m = 0; m < N; m++) {
        allow_list.push_back(m);
    }
    best_solution.resize(N);
    int cur_city = rand() % N;
    best_solution[0] = cur_city;
    allow_list.erase(allow_list.begin() + best_solution[0]);
    int count = 0;

    // create first solution selecting cheaper arc ad each iteration
    while (allow_list.size() > 0) {
        int next_city = -1;
        int element_to_remove = -1;
        double min_path_cost = DBL_MAX;
        for (int k = 0; k < allow_list.size(); ++k) {
            if (costs[best_solution[count]][allow_list[k]] < min_path_cost) {
                min_path_cost = costs[best_solution[count]][allow_list[k]];
                next_city = allow_list[k];
                element_to_remove = k;
            }
        }
        count++;
        best_solution[count] = next_city;
        allow_list.erase(allow_list.begin() + element_to_remove);
    }
    return;
}

void SimulatedAnnealingSolver::solve() {
    vector<int> current_solution = best_solution;
    double current_length = min_length;
    int loops = 0;
    // loop if there is enough temperature
    while (std::chrono::steady_clock::now() < this->stop_time && temperature > temperature_min) {
        for (int i = 0; i < max_iterations; ++i) {
            if (std::chrono::steady_clock::now() >= this->stop_time) {
                break;
            }

            loops += 1;
            double new_sa_length = 0.0;
            vector<int> new_sa_solution;
            new_sa_solution = current_solution;
            int index1 = rand() % N;
            int index2 = rand() % N;
            while (index1 == index2) {
                index2 = rand() % N;
            }

            int temp = new_sa_solution[index1];
            new_sa_solution[index1] = new_sa_solution[index2];
            new_sa_solution[index2] = temp;
            new_sa_solution.push_back(new_sa_solution[0]);
            for (int l = 0; l < new_sa_solution.size() - 1; l++) { // calculate new solution
                new_sa_length += costs[new_sa_solution[l]][new_sa_solution[l + 1]];
            }

            double difference_sa_curr = new_sa_length - current_length; // calculate difference between new and current solution

            if (difference_sa_curr < 0.0) {
                new_sa_solution.pop_back();
                current_solution = new_sa_solution;
                current_length = new_sa_length;
                if (new_sa_length < min_length) {
                    min_length = new_sa_length;
                    best_solution = new_sa_solution;
                }
            } else {
                double prob_sa_acceptance = exp(-abs(difference_sa_curr) / temperature); // calculate probability of accept new solution even if worse
                double rand_p = (double) rand() / RAND_MAX;
                if (rand_p < prob_sa_acceptance) {
                    new_sa_solution.pop_back();
                    current_solution = new_sa_solution;
                    current_length = new_sa_length;
                }
            }
        }
        temperature = temperature * temperature_max;
    }
    problem->setMinCost(min_length);
    problem->setSolution(best_solution);
}
