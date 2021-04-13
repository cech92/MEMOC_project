#include <iostream>
#include <fstream>
#include <sstream>
#include "problem.h"

using namespace std;

Problem::Problem(string path) {
    string token;
    size_t pos = 0;

    string fp = path;
    std::ifstream infile("inputs/" + fp);

    int i = -1;
    string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        cout << "line " << line << endl;
        if (i == -1) {
            iss >> N;
            costs.reserve(N); // time taken by drill to move from i to j, ∀ (i, j) ∈ A
//            indexes.reserve(N);
        } else {
//            indexes[i] = i;
            costs[i].reserve(N);
            double input_result;
            while (iss >> input_result)
                costs[i].push_back(input_result);
        }
        ++i;
    }
    cout << "num vars " << N << endl;
}