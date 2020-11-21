#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"

using namespace std;

int status;
char errmsg[BUF_SIZE];

const int NAME_SIZE = 512;
char name[NAME_SIZE];

vector<vector<int> > map_y; // 1 if arc from i to j used, 0 otherwise, ∀ (i, j) ∈ A
vector<vector<int> > map_x; // amount of the flow shipped from i to j, ∀ (i, j) ∈ A;
vector<int> indexes; // amount of the flow shipped from i to j, ∀ (i, j) ∈ A;

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

    double C[N][N]; // time taken by drill to move from i to j, ∀ (i, j) ∈ A
    indexes.resize(N);
    for (int i = 0; i < N; i++) {
        indexes[i] = i;
        for (int j = 0; j < N; j++) {
            C[i][j] = getRandomValue(10, 100);
        }
    }

    // print generated arcs
    srand(time(0));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << C[i][j] << " ";
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
    const int y_init = CPXgetnumcols(env, lp); // first index for y vars
    int current_y_var_position = y_init;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j)
                continue;
            char y_type = 'B';
            double obj, lb = 0.0;
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
    const int x_init = CPXgetnumcols(env, lp); // first index for y vars
    int current_x_var_position = x_init;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j)
                continue;
            char x_type = 'I';
            double obj, lb = 0.0;
            double ub = 1.0;
            snprintf(name, NAME_SIZE, "x_%d_%d", indexes[i], indexes[j]);
            char* x_name = (char*) &name[0];
            CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &obj, &lb, &ub, &x_type, &x_name);
            map_x[i][j] = current_x_var_position;
            ++current_x_var_position;
        }
    }
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
        setupProblem(env, lp, numVars);
    } catch(std::exception& e)
    {
        cout << "EXCEPTION: " << e.what() << std::endl;
    }
    return 0;
}
