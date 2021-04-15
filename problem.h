#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include <string>

using namespace std;

class Problem {
private:
    vector<vector<double>> costs;
    vector<int> indexes;
    unsigned int N;
public:
    Problem(string path);
    vector<vector<double>> getCosts();
    vector<int> getIndexes();
    unsigned int getN();
};
#endif