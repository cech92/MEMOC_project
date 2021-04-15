#include <math.h>
#include "ant_colony_solver.h"


AntColonySolver::AntColonySolver(Problem* problem, unsigned int num_ants, double alpha, double beta, double rho, unsigned int max_iterations, unsigned int max_execution_time) {
    this->problem = problem;
    this->num_cities = problem->getN();
    this->num_ants = num_ants;
    this->alpha = alpha;
    this->beta = beta;
    this->rho = rho;
    this->max_iterations = max_iterations;
    this->max_execution_time = max_execution_time;

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
            visibility_matrix[i][j] = 1.0 / problem->getCosts()[i][j];
        }
    }

    // initialize the ants paths matrix
    ants_paths.resize(num_ants);
    for (int i = 0; i < num_ants; i++) {
        ants_paths[i].resize(num_cities);
        for (int j = 0; j < num_cities; j++) {
            ants_paths[i][j] = 0;
        }
    }
}

void AntColonySolver::solve() {
    vector<vector<double>> probability_matrix;
    if (max_iterations > 0) {
        for (int i = 0; i < max_iterations; ++i) {
            probability_matrix.resize(num_cities);
            for (int j = 0; j < num_cities; ++j) {
                probability_matrix[j].resize(num_cities);
                for (int k = 0; k < num_cities; ++k) {
                    probability_matrix[j][k] = pow(pheromones_matrix[j][k], alpha) * pow(visibility_matrix[j][k], beta);
                }
            }

            // loop over ants
            for (int j = 0; j < num_ants; j++) {
                int start_city = rand() % num_cities;
//                vector<int> tabu_list;
//                for (int k = 0; k < num_cities; ++k) {
//                    //remove current and last city from the list of possible cities to visit
//                    for (int l = k; l >= 0; --l)
//                        tabu_list.push_back(ants_paths[j][k])
//                }
                vector<int> allow_list = ants_paths[j];
                for (int k = 0; k < num_cities; k++) {
                    for (int l = k; l >= 0; l--) {
                        auto it = std::find(allow_list.begin(), allow_list.end(), ants_paths[j][l]);
                        if (it != allow_list.end()) {
                            allow_list.erase(it);
                        }
                    }
                }
            }
        }
    }
}