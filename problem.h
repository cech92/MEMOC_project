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
    vector<int> solution;
    double min_cost;
public:
    Problem(string path);
    vector<vector<double>> getCosts();
    vector<int> getIndexes();
    unsigned int getN();
    void setMinCost(double min_cost);
    double getMinCost();
    void setSolution(vector<int> solution);
    vector<int> getSolution();
};
#endif