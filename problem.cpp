#include <iostream>
#include <fstream>
#include <sstream>
#include "problem.h"

using namespace std;

Problem::Problem(string path) {
    string token;

    string fp = path;
    std::ifstream infile("inputs/" + fp);

    int i = -1;
    string line;
    double input_result;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (i == -1) {
            iss >> N;
            costs.resize(N); // time taken by drill to move from i to j, ∀ (i, j) ∈ A
            indexes.resize(N);
        } else {
            indexes[i] = i;
            while (iss >> input_result)
                costs[i].push_back(input_result);
        }
        ++i;
    }
}

vector<vector<double>> Problem::getCosts() {
    return costs;
}

vector<int> Problem::getIndexes() {
    return indexes;
}

unsigned int Problem::getN() {
    return N;
}

void Problem::setMinCost(double min_cost) {
    this->min_cost = min_cost;
}

double Problem::getMinCost() {
    return this->min_cost;
}

void Problem::setSolution(vector<int> solution) {
    this->solution = solution;
}

vector<int> Problem::getSolution() {
    return solution;
}