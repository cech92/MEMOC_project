#include <iostream>
#include <math.h>
#include <float.h>
#include <functional>
#include <numeric>
#include <unordered_set>
#include "ant_colony_solver.h"

using namespace std;


AntColonySolver::AntColonySolver(Problem* problem, unsigned int num_ants, double alpha, double beta, double rho, unsigned int max_iterations, unsigned int max_execution_time) {
    this->problem = problem;
    this->costs = problem->getCosts();
    this->num_cities = problem->getN();
    this->num_ants = num_ants;
    this->alpha = alpha;
    this->beta = beta;
    this->rho = rho;
    this->max_iterations = max_iterations;
    this->max_execution_time = max_execution_time;

    this->min_length = (double) DBL_MAX;

    // initialize the pheromone matrix with the same pheromone quantity for each edge
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
                visibility_matrix[i][j] = 1.0 / 0.00000000001;
            else
                visibility_matrix[i][j] = 1.0 / problem->getCosts()[i][j];
        }
    }

    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            cout << visibility_matrix[i][j] << " ";
        }
        cout <<endl;
    }

    // initialize the ants paths matrix
    ants_paths.resize(num_ants);
    for (int i = 0; i < num_ants; i++) {
        ants_paths[i].resize(num_cities);
        for (int j = 0; j < num_cities; j++) {
            ants_paths[i][j] = -1;
        }
    }
}

void AntColonySolver::solve() {
    vector<vector<double>> probability_matrix;
    if (max_iterations > 0) {
        for (int i = 0; i < max_iterations; ++i) {
            cout << "iteration n: " << i << endl;

//            probability_matrix.resize(num_cities);
//            for (int j = 0; j < num_cities; ++j) {
//                probability_matrix[j].resize(num_cities);
//                for (int k = 0; k < num_cities; ++k) {
//                    probability_matrix[j][k] = pow(pheromones_matrix[j][k], alpha) * pow(visibility_matrix[j][k], beta);
//                }
//            }

            // loop over ants
            for (int j = 0; j < num_ants; j++) {
                cout << "ant n: " << j << endl;

                vector<int> allow_list;
                for (int m = 0; m < num_cities; m++) {
                    allow_list.push_back(m);
                }

                int start_city = rand() % num_cities;
                ants_paths[j][0] = start_city;
                cout << "ant n: " << j << " start from city: " << start_city << endl;
                allow_list.erase(allow_list.begin() + ants_paths[j][0]);

                int count = 0;
                probability_matrix.resize(num_cities);
                while (allow_list.size() > 0) {
                    double probability_sum = 0.0;
                    probability_matrix[ants_paths[j][count]].resize(num_cities);

                    for (int k = 0; k < allow_list.size(); ++k) {
//                        for (int m = 0; m < allow_list.size(); m++) {
//                            cout << allow_list[m] << " ";
//                        }
//                        cout << " | ";
                        probability_matrix[ants_paths[j][count]][allow_list[k]] =
                                pow(pheromones_matrix[ants_paths[j][count]][allow_list[k]], alpha) *
                                pow(visibility_matrix[ants_paths[j][count]][allow_list[k]], beta);
                        probability_sum += probability_matrix[ants_paths[j][count]][allow_list[k]];
                    }

                    double summ = 0.0;
                    for (int k = 0; k < allow_list.size(); ++k) {
                        probability_matrix[ants_paths[j][count]][allow_list[k]] =
                                probability_matrix[ants_paths[j][count]][allow_list[k]] / probability_sum;
//                        summ += probability_matrix[ants_paths[j][count]][allow_list[k]];
//                        cout << summ <<endl;
                    }
                    cout << endl;

                    double random = ((double) rand() / (RAND_MAX));
//                    cout << "random value " << random << endl;

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
                for (int k = 0; k < ants_paths[j].size() - 1; k++) {
                    cout << "From " << ants_paths[j][k] << " to " << ants_paths[j][k + 1] << " "
                         << costs[ants_paths[j][k]][ants_paths[j][k + 1]] << endl;
                    current_length += costs[ants_paths[j][k]][ants_paths[j][k + 1]];
                }
                current_length += costs[ants_paths[j][ants_paths[j].size() - 1]][ants_paths[j][0]];
                if (current_length < min_length) {
                    min_length = current_length;
                    cout << "new min length " << min_length << endl;
                }
                cout << "min length " << min_length << endl;
            }

//                unordered_set<int> tabu_list;
//                for (int k = 0; k < num_cities - 1; ++k) {
//                    //remove current and last city from the list of possible cities to visit
//                    for (int l = k; l >= 0; --l)
//                        tabu_list.insert(ants_paths[j][l]);
//
//                    vector<int> allow_list;
//                    for (int m = 0; m < num_cities; m++)
//                        allow_list.push_back(m);
//
//                    allow_list.erase(std::remove_if(std::begin(allow_list), std::end(allow_list),
//                                                    [&tabu_list](int m) {
//                                                        return std::find(std::begin(tabu_list), std::end(tabu_list),
//                                                                         m) != std::end(tabu_list);
//                                                    }),
//                                     std::end(allow_list));
//
//                    for (int m = 0; m < allow_list.size(); m++) {
//                        cout << allow_list[m] << " ";
//                    }
//                    cout << " | ";
//
//                    vector<double> probability_array = probability_matrix[ants_paths[j][k]];
//                    double sum_prob = 0;
//                    for (int m = 0; m < probability_array.size(); m++) {
//                        sum_prob += probability_array[m];
//                    }
//                    for (int m = 0; m < probability_array.size(); m++) {
//                        probability_array[m] = probability_array[m] / sum_prob;
//                    }
////                    for (int m = 0; m < probability_array.size(); m++) {
////                        cout << probability_array[m]<<endl;
////                    }
//                }


//                for (int k = 0; k < num_cities; k++) {
//                    for (int l = k; l >= 0; l--) {
//                        cout << j << " " << l << " " << ants_paths[j][l] << endl;
//                        auto it = std::find(allow_list.begin(), allow_list.end(), ants_paths[j][l]);
//                        if (it != allow_list.end()) {
//                            allow_list.erase(it);
//                        }
//                    }
//                    cout << allow_list.size() << endl;
//                }

        }
    }
}