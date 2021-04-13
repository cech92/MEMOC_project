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
        cout << i << "line " << line << endl;
        if (i == -1) {
            iss >> N;
            costs.resize(N); // time taken by drill to move from i to j, ∀ (i, j) ∈ A
            indexes.resize(N);
        } else {
            indexes[i] = i;
//            costs[i].resize(N);
            double input_result;
            while (iss >> input_result)
                costs[i].push_back(input_result);
        }
        ++i;
    }
    cout << "num vars " << N << endl;

//    for (int i = 0; i < N; i++) {
//        for (int j = 0; j < N; j++) {
//            cout << costs[i][j] << " ";
//        }
//        cout << "\n";
//    }
}

vector<vector<double>> Problem::getCosts() {
    cout <<costs[0][1]<<endl;
    return costs;
}

vector<int> Problem::getIndexes() {
    return indexes;
}

unsigned int Problem::getN() {
    return N;
}