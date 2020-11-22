#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"

using namespace std;

int status;
char errmsg[BUF_SIZE];

const int NAME_SIZE = 512;
char name[NAME_SIZE];

vector<vector<double> > costs; //
vector<vector<int> > map_y; // 1 if arc from i to j used, 0 otherwise, ∀ (i, j) ∈ A
vector<vector<int> > map_x; // amount of the flow shipped from i to j, ∀ (i, j) ∈ A
vector<int> indexes; // amount of the flow shipped from i to j, ∀ (i, j) ∈ A
int first_index = 0;

double getRandomValue(double min, double max) {
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

char nth_letter(int n)
{
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"[n % 50];
}

void setupProblem(CEnv env, Prob lp, int &numVars ) {
    // data
    const int N = numVars; // number of nodes (holes)

    costs.reserve(N); // time taken by drill to move from i to j, ∀ (i, j) ∈ A
    indexes.reserve(N);
    for (int i = 0; i < N; i++) {
        costs[i].reserve(N);
        indexes[i] = i;
        for (int j = 0; j < N; j++) {
            if (i == j)
                costs[i].push_back(0.0);
            else
                costs[i].push_back(getRandomValue(10, 100));
        }
    }

    // print generated arcs
    srand(time(0));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << costs[i][j] << " ";
        }
        cout << "\n";
    }

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
            double ub = CPX_INFBOUND;
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
    const int x_init = CPXgetnumcols(env, lp); // first index for y vars
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
        int matbeg = 0;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
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
        snprintf(name, NAME_SIZE, "in_%d",j+1);
        char* cname = (char*)(&name[0]);
        int matbeg = 0;
        CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, &cname);
        /// status = CPXaddrows ( env, lp, colcnt, rowcnt, nzcnt     , rhs  , sense , rmatbeg, rmatind, rmatval , newcolname, newrowname);
    }

    for (int k = 1; k < N; k++) {
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

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j || j == first_index)
                continue;
            std::vector<int> idx(2);
            idx[0] = map_x[i][j];
            idx[1] = map_y[i][j];
            std::vector<double> coef(2);
            coef[0] = 1.0;
            coef[1] = -(N - 1);
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

// check this: why is a multi commodity problem?




int main(int argc, char const *argv[]) {
    try {
        DECL_ENV(env);
        DECL_PROB(env, lp);

        int numVars;
        cout << "Choose the number of holes: ";
        cin >> numVars;
        cout << "The board has " << numVars << " holes\n";
        // setup problem
        setupProblem(env, lp, numVars);

        // solve problem
        CHECKED_CPX_CALL( CPXmipopt, env, lp );

        // print
        double objval;
        CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
        std::cout << "Objval: " << objval << std::endl;

        int num = CPXgetnumcols(env, lp);
        std::vector<double> vars;
        vars.resize(num);
        int fromIdx = 0;
        int toIdx = numVars - 1;
        CHECKED_CPX_CALL( CPXgetx, env, lp, &vars[0], fromIdx, toIdx );

        CHECKED_CPX_CALL( CPXsolwrite, env, lp, "solution.sol" );
    } catch(std::exception& e)
    {
        cout << "EXCEPTION: " << e.what() << std::endl;
    }
    return 0;
}
