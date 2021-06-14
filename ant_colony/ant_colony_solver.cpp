#include <iostream>
#include <math.h>
#include <float.h>
#include <functional>
#include <numeric>
#include <unordered_set>
#include "ant_colony_solver.h"

using namespace std;


AntColonySolver::AntColonySolver(Problem* problem, unsigned int num_ants, double alpha, double beta, double rho, double q, unsigned int max_iterations, unsigned int max_execution_time, bool with_sa) {
    this->problem = problem;
    this->costs = problem->getCosts();
    this->num_cities = problem->getN();
    this->num_ants = num_ants;
    this->alpha = alpha;
    this->beta = beta;
    this->rho = rho;
    this->q = q;
    this->max_iterations = max_iterations;
    this->max_execution_time = max_execution_time;

    this->min_length = DBL_MAX;
    this->stop_time = std::chrono::steady_clock::now() + std::chrono::seconds(this->max_execution_time);

    // initialize the pheromone matrix (tau) with the same pheromone quantity for each edge
    pheromones_matrix.resize(num_cities);
    for (int i = 0; i < num_cities; i++) {
        pheromones_matrix[i].resize(num_cities);
        for (int j = 0; j < num_cities; j++) {
            pheromones_matrix[i][j] = 1;
        }
    }

    // fill visibility matrix (1 / dij)
    visibility_matrix.resize(num_cities);
    for (int i = 0; i < num_cities; i++) {
        visibility_matrix[i].resize(num_cities);
        for (int j = 0; j < num_cities; j++) {
            if (i == j)
                visibility_matrix[i][j] = 1.0 / DBL_MIN;
            else
                visibility_matrix[i][j] = 1.0 / problem->getCosts()[i][j];
        }
    }

    // initialize the ants paths matrix
    ants_paths.resize(num_ants);
    for (int i = 0; i < num_ants; i++) {
        ants_paths[i].resize(num_cities);
        for (int j = 0; j < num_cities; j++) {
            ants_paths[i][j] = -1;
        }
    }

    generation_results.resize(num_ants);
//    for (int i = 0; i < num_ants; i++) {
//        generation_results[i] = 0.0;
//    }

    // initialize simulated annealing if enabled
    this->with_sa = with_sa;
    if (with_sa) {
        this->temperature_max = 0.995;
        this->temperature_min = 1e-08;
        this->temperature = temperature_max;
        this->num_sa_iterations = num_cities;
    }
}

void AntColonySolver::solve() {
    vector<vector<double>> probability_matrix;

    if (this->max_iterations > 0) {
        for (int i = 0; i < max_iterations; ++i) {
            if (std::chrono::steady_clock::now() >= this->stop_time) {
                break;
            }

            this->best_generation_result = DBL_MAX;
            // loop over ants
            for (int j = 0; j < num_ants; j++) {
                vector<int> allow_list;
                for (int m = 0; m < num_cities; m++) {
                    allow_list.push_back(m);
                }

                int start_city = rand() % num_cities;
                ants_paths[j][0] = start_city;
                allow_list.erase(allow_list.begin() + ants_paths[j][0]);

                int count = 0;
                probability_matrix.resize(num_cities);
                while (allow_list.size() > 0) {
                    double probability_sum = 0.0;
                    probability_matrix[ants_paths[j][count]].resize(num_cities);

                    for (int k = 0; k < allow_list.size(); ++k) {
                        probability_matrix[ants_paths[j][count]][allow_list[k]] =
                                pow(pheromones_matrix[ants_paths[j][count]][allow_list[k]], alpha) *
                                pow(visibility_matrix[ants_paths[j][count]][allow_list[k]], beta);
                        probability_sum += probability_matrix[ants_paths[j][count]][allow_list[k]];
                    }

                    for (int k = 0; k < allow_list.size(); ++k) {
                        probability_matrix[ants_paths[j][count]][allow_list[k]] =
                                probability_matrix[ants_paths[j][count]][allow_list[k]] / probability_sum;
                    }

                    double random = ((double) rand() / (RAND_MAX));

                    double prob_tot = 0.0;
                    int next_city = -1;
                    int element_to_remove = -1;
                    for (int k = 0; k < allow_list.size(); ++k) {
                        prob_tot += probability_matrix[ants_paths[j][count]][allow_list[k]];
                        if (random <= prob_tot) {
                            next_city = allow_list[k];
                            element_to_remove = k;
                            break;
                        }
                    }

                    count++;
                    ants_paths[j][count] = next_city;
                    allow_list.erase(allow_list.begin() + element_to_remove);
                }

                double current_length = 0.0;
                vector<int> current_solution;
                current_solution.reserve(num_cities + 1);
                for (int k = 0; k < ants_paths[j].size() - 1; k++) {
                    current_length += costs[ants_paths[j][k]][ants_paths[j][k + 1]];
                    current_solution.push_back(ants_paths[j][k]);
                }
                current_solution.push_back(ants_paths[j][num_cities - 1]);
                current_solution.push_back(ants_paths[j][0]);

                current_length += costs[ants_paths[j][ants_paths[j].size() - 1]][ants_paths[j][0]];
                generation_results[j] = current_length;
                if (current_length < best_generation_result) {
                    best_generation_result = current_length;
                    best_generation_index = j;
                }

                if (current_length < min_length) {
                    min_length = current_length;
                    solution = current_solution;
                }
            }

            if (temperature >= temperature_min && with_sa && std::chrono::steady_clock::now() < this->stop_time) {
                for (int k = 0; k < num_sa_iterations; k++) {
                    double new_sa_length = 0.0;
                    vector<int> new_sa_solution;
                    new_sa_solution = ants_paths[best_generation_index];
                    int index1 = rand() % num_cities;
                    int index2 = rand() % num_cities;
                    while (index1 == index2) {
                        index2 = rand() % num_cities;
                    }

                    int temp = new_sa_solution[index1];
                    new_sa_solution[index1] = new_sa_solution[index2];
                    new_sa_solution[index2] = temp;
                    new_sa_solution.push_back(new_sa_solution[0]);
                    for (int l = 0; l < new_sa_solution.size() - 1; l++) {
                        new_sa_length += costs[new_sa_solution[l]][new_sa_solution[l + 1]];
                    }

                    double difference_sa_curr = new_sa_length - generation_results[best_generation_index];
                    if (difference_sa_curr < 0.0) {
                        new_sa_solution.pop_back();
//                            if (k == 0) {
//                                ants_paths.push_back(ants_paths[j]);
//                                generation_results.push_back(generation_results[j]);
//                                num_ants++;
//                            }
                        ants_paths[best_generation_index] = new_sa_solution;
                        generation_results[best_generation_index] = new_sa_length;
                        if (new_sa_length < min_length) {
                            min_length = new_sa_length;
                            solution = new_sa_solution;
                        }
                    } else {
                        double prob_sa_acceptance = exp(-difference_sa_curr / temperature);
                        double rand_p = (double) rand() / RAND_MAX;
                        if (rand_p < prob_sa_acceptance) {
                            new_sa_solution.pop_back();
                            ants_paths[best_generation_index] = new_sa_solution;
                            generation_results[best_generation_index] = new_sa_length;
                        }
                    }
                }
                temperature = temperature * temperature_max;
            }


            // calculating new pheromone levels
            vector<vector<double>> delta_tau_table;
            delta_tau_table.resize(num_cities);
            for (int j = 0; j < num_cities; j++) {
                delta_tau_table[j].resize(num_cities);
                for (int k = 0; k < num_cities; k++) {
                    delta_tau_table[j][k] = 0;
                }
            }
            for (int j = 0; j < ants_paths.size(); j++) {
                for (int k = 0; k < ants_paths[j].size() - 1; k++) {
                    delta_tau_table[ants_paths[j][k]][ants_paths[j][k+1]] += this->q / costs[ants_paths[j][k]][ants_paths[j][k + 1]];
                }
                delta_tau_table[ants_paths[j][num_cities - 1]][ants_paths[j][0]] += this->q / costs[ants_paths[j][ants_paths[j].size() - 1]][ants_paths[j][0]];
            }

            for (int j = 0; j < num_cities; j++) {
                for (int k = 0; k < num_cities; k++) {
                    pheromones_matrix[j][k] = (1 - rho) * pheromones_matrix[j][k] + delta_tau_table[j][k];
                }
            }
        }
        problem->setMinCost(min_length);
        problem->setSolution(solution);
    }
}