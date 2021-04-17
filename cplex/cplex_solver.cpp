#include <iostream>
#include <vector>
#include "cplex_solver.h"

using namespace std;

CPLEXSolver::CPLEXSolver(Problem* problem) {
    this->problem = problem;

    this->env = CPXopenCPLEX(&status);
    if (status){
        CPXgeterrorstring(NULL, status, errmsg);
        int trailer = std::strlen(errmsg) - 1;
        if (trailer >= 0) errmsg[trailer] = '\0';
        throw std::runtime_error(std::string(__FILE__) + ":" + STRINGIZE(__LINE__) + ": " + errmsg);
    }
    this->lp = CPXcreateprob(env, &status, "");
    if (status){
        CPXgeterrorstring(NULL, status, errmsg);
        int trailer = std::strlen(errmsg) - 1;
        if (trailer >= 0) errmsg[trailer] = '\0';
        throw std::runtime_error(std::string(__FILE__) + ":" + STRINGIZE(__LINE__) + ": " + errmsg);
    }

    vector<vector<double>> costs = problem->getCosts();
    vector<int> indexes = problem->getIndexes();
    unsigned int N = problem->getN();

    map_y.resize(N);

    for (int i = 0; i < N; i++) {
        map_y[i].resize(N);
        for (int j = 0; j < N; j++) {
            map_y[i][j] = -1;
        }
    }

    int current_y_var_position = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j)
                continue;
            char y_type = 'B';
            double obj = costs[i][j];
            double lb = 0.0;
            double ub = 1.0;
            snprintf(name, NAME_SIZE, "y_%d_%d", indexes[i], indexes[j]);
            char* y_name = (char*) &name[0];
            CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &obj, &lb, &ub, &y_type, &y_name);
            map_y[i][j] = current_y_var_position;
            ++current_y_var_position;
        }
    }

    map_x.resize(N);
    for (int i = 0; i < N; i++) {
        map_x[i].resize(N);
        for (int j = 0; j < N; j++) {
            map_x[i][j] = -1;
        }
    }
    const int x_init = CPXgetnumcols(env, lp);
    int current_x_var_position = x_init;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j)
                continue;
            char x_type = 'I';
            double obj = 0.0;
            double lb = 0.0;
            double ub = CPX_INFBOUND;
            snprintf(name, NAME_SIZE, "x_%d_%d", indexes[i], indexes[j]);
            char* x_name = (char*) &name[0];
            CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &obj, &lb, &ub, &x_type, &x_name);
            map_x[i][j] = current_x_var_position;
            ++current_x_var_position;
        }
    }

    for (int i = 0; i < N; i++) {
        std::vector<int> idx;
        std::vector<double> coef;
        for (int j = 0; j < N; j++) {
            if (i == j)
                continue;
            idx.push_back(map_y[i][j]);
            coef.push_back(1);
        }
        char sense = 'E';
        double rhs = 1.0;
        snprintf(name, NAME_SIZE, "flow_sent_%d", i);
        char* cname = (char*)(&name[0]);
        int matbeg = 0;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, &cname);
        /// status = CPXaddrows ( env, lp, colcnt, rowcnt, nzcnt     , rhs  , sense , rmatbeg, rmatind, rmatval , newcolname, newrowname);
    }

    for (int j = 0; j < N; j++) {
        std::vector<int> idx;
        std::vector<double> coef;
        for (int i = 0; i < N; i++) {
            if (i == j)
                continue;
            idx.push_back(map_y[i][j]);
            coef.push_back(1);
        }
        char sense = 'E';
        double rhs = 1.0;
        snprintf(name, NAME_SIZE, "flow_received_%d", j);
        char* cname = (char*)(&name[0]);
        int matbeg = 0;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, &cname);
        /// status = CPXaddrows ( env, lp, colcnt, rowcnt, nzcnt     , rhs  , sense , rmatbeg, rmatind, rmatval , newcolname, newrowname);
    }

    for (int k = 0; k < N; k++) {
        if (k == first_index)
            continue;
        std::vector<int> idx;
        std::vector<double> coef;
        for (int i = 0; i < N; i++) {
            if (k == i)
                continue;
            idx.push_back(map_x[i][k]);
            coef.push_back(1);
        }
        for (int j = 0; j < N; j++) {
            if (k == j)
                continue;
            idx.push_back(map_x[k][j]);
            coef.push_back(-1);
        }
        char sense = 'E';
        double rhs = 1.0;
        snprintf(name, NAME_SIZE, "flow_diff_%d", k);
        char* cname = (char*)(&name[0]);

        int matbeg = 0;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, &cname);
    }

    // xij <= (N - 1) yij ∀ (i, j) ∈ A, j != 0
    for (int i = 0; i < N; i++) {
        for (int j = 1; j < N; j++) {
            if (i == j)
                continue;
            std::vector<int> idx(2);
            idx[0] = map_x[i][j];
            idx[1] = map_y[i][j];
            std::vector<double> coef(2);
            coef[0] = 1.0;
            coef[1] = (N - 1) * (-1.0);
            char sense = 'L';
            double rhs = 0.0;
            snprintf(name, NAME_SIZE, "att_%d_%d", i, j);
            char* cname = &name[0];
            int matbeg = 0;
            CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, 2, &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, &cname);
        }
    }

    CHECKED_CPX_CALL(CPXwriteprob, env, lp, "problem.lp", 0);
}

void CPLEXSolver::solve() {
    CHECKED_CPX_CALL(CPXmipopt, env, lp);

    double objval;
    CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
    problem->setMinCost(objval);

    int num = CPXgetnumcols(env, lp);
    std::vector<double> vars;
    vars.resize(num);
    int fromIdx = 0;
    int toIdx = num - 1;
    CHECKED_CPX_CALL(CPXgetx, env, lp, &vars[0], fromIdx, toIdx);

    vector<int> solution;
    findSolution(solution, vars, 0);
    problem->setSolution(solution);

    CHECKED_CPX_CALL(CPXsolwrite, env, lp, "solution.sol");
}

void CPLEXSolver::findSolution(vector<int>& solution, vector<double>vars, int start) {
    if (solution.size() > 1 && solution[0] == solution[solution.size() -1])
        return;
    for (int i = 0; i < problem->getN(); i++) {
        if (vars[map_y[start][i]] == 1) {
            solution.push_back(i);
            findSolution(solution, vars, i);
        }
    }
}