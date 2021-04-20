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
        this->temperature_min = 0.000001;
        this->temperature = temperature_max;
        this->num_sa_iterations = 10 * num_cities;
    }
}

void AntColonySolver::solve() {
    vector<vector<double>> probability_matrix;
    if (max_iterations > 0) {
        for (int i = 0; i < max_iterations; ++i) {

//            probability_matrix.resize(num_cities);
//            for (int j = 0; j < num_cities; ++j) {
//                probability_matrix[j].resize(num_cities);
//                for (int k = 0; k < num_cities; ++k) {
//                    probability_matrix[j][k] = pow(pheromones_matrix[j][k], alpha) * pow(visibility_matrix[j][k], beta);
//                }
//            }

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
//                            cout << "next city " << next_city << " | ";
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
//                    cout << "From " << ants_paths[j][k] << " to " << ants_paths[j][k + 1] << " "
//                         << costs[ants_paths[j][k]][ants_paths[j][k + 1]] << endl;
                    current_length += costs[ants_paths[j][k]][ants_paths[j][k + 1]];
                    current_solution.push_back(ants_paths[j][k]);
                }
                current_solution.push_back(ants_paths[j][num_cities - 1]);
                current_solution.push_back(ants_paths[j][0]);

                current_length += costs[ants_paths[j][ants_paths[j].size() - 1]][ants_paths[j][0]];
                generation_results[j] = current_length;

//                cout << "start from: ";
//                for (int l = 0; l < ants_paths[j].size(); l++) {
//                    cout << ants_paths[j][l] << " ";
//                }
//                cout << endl;

                if (current_length < min_length) {
                    min_length = current_length;
                    solution = current_solution;
                    cout << "new min length " << j << " " << min_length << endl;
                }
            }

            if (temperature >= temperature_min && with_sa) {
                for (int j = 0; j < num_ants; j++) {
                    for (int k = 0; k < num_sa_iterations; k++) {
                        double new_sa_length = 0.0;
                        vector<int> new_sa_solution;
                        new_sa_solution = ants_paths[j];
                        int index1 = rand() % num_cities;
                        int index2 = rand() % num_cities;
                        while (index1 == index2) {
                            index2 = rand() % num_cities;
                        }
                        //                    cout << index1 << " " << index2<<endl;
                        int temp = new_sa_solution[index1];
                        new_sa_solution[index1] = new_sa_solution[index2];
                        new_sa_solution[index2] = temp;
                        new_sa_solution.push_back(new_sa_solution[0]);
                        for (int l = 0; l < new_sa_solution.size() - 1; l++) {
                            //                        cout << new_sa_solution[l] << " ";
                            new_sa_length += costs[new_sa_solution[l]][new_sa_solution[l + 1]];
                        }
                        //                    cout << endl;
                        //                    cout << endl;
                        //                    cout << "curr, new : " << current_length << " | " << new_sa_length << endl;

                        if (new_sa_length < min_length) {
                            min_length = new_sa_length;
                            solution = new_sa_solution;
                            cout << "new min length with sa " << j << " " << min_length << endl;
                        }
                        double difference_sa_curr = new_sa_length - generation_results[j];
                        double prob_sa_acceptance = exp(-difference_sa_curr / temperature);
                        double rand_p = (double) rand() / RAND_MAX;
                        if (difference_sa_curr < 0.0 || rand_p < prob_sa_acceptance) {
                            new_sa_solution.pop_back();
                            if (k == 0) {
                                ants_paths.push_back(ants_paths[j]);
                                generation_results.push_back(generation_results[j]);
                                num_ants++;
                                cout << "num ants " << num_ants << " ";
                            }
                            ants_paths[j] = new_sa_solution;
                            generation_results[j] = new_sa_length;
                        }
//                        temperature = temperature * temperature_max;
                    }
                    temperature = temperature * temperature_max;
                }
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
                    delta_tau_table[ants_paths[j][k]][ants_paths[j][k+1]] += 1 / costs[ants_paths[j][k]][ants_paths[j][k + 1]];
                }
                delta_tau_table[ants_paths[j][num_cities - 1]][ants_paths[j][0]] += 1 / costs[ants_paths[j][ants_paths[j].size() - 1]][ants_paths[j][0]];
            }

            for (int j = 0; j < num_cities; j++) {
                for (int k = 0; k < num_cities; k++) {
                    pheromones_matrix[j][k] = (1 - rho) * pheromones_matrix[j][k] + delta_tau_table[j][k];
                }
            }

            alpha = (double)rand()/RAND_MAX * 2.0;
//            cout << "alpha " << alpha << endl;
        }
        problem->setMinCost(min_length);
        problem->setSolution(solution);
    }
}