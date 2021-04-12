#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <libgen.h>
#include <math.h>
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
int N = 0;

double getRandomValue(double min, double max) {
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

char nth_letter(int n)
{
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"[n % 50];
}

void setCosts(std::string &filePath) {
    std::string token;
    size_t pos = 0;
    int j = 0;
    std::string fp = filePath;
    std::ifstream infile("inputs/" + fp);

//    while ((pos = filePath.find("_")) != std::string::npos) {
//        token = filePath.substr(0, pos);
//        cout << "token " << token << std::endl;
//        if (j == 2) {
//            numVars = std::stoi(token);
//            break;
//        }
//        filePath.erase(0, pos + 1);
//        j += 1;
//    }

    int i = -1;
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        cout << "line " << line << endl;
        if (i == -1) {
            iss >> N;
            costs.reserve(N); // time taken by drill to move from i to j, ∀ (i, j) ∈ A
            indexes.reserve(N);
        } else {
            indexes[i] = i;
            costs[i].reserve(N);
            double input_result;
            while (iss >> input_result)
                costs[i].push_back(input_result);
        }
        ++i;
    }
    cout << "num vars " << N << endl;
//    N = numVars;



//    while (std::getline(infile, line)) {
//        indexes[i] = i;
//        costs[i].reserve(numVars);
//        std::istringstream iss(line);
//        double x1, y1;
//        if (!(iss >> x1 >> y1)) { break; } // error
////        cout << x1 << " " << y1 << std::endl;
//        std::string line2;
//
//        std::ifstream infile2("inputs/" + fp);
//        int j = 0;
//        while (std::getline(infile2, line2)) {
//            std::istringstream iss2(line2);
//            double x2, y2;
//            if (!(iss2 >> x2 >> y2)) { break; } // error
////            cout << "2: " << x2 << " " << y2 << std::endl;
//            // process pair (a,b)
//            if (x1 == x2 && y1 == y2) {
//                costs[i].push_back(0.0);
//            } else {
//                costs[i].push_back(sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2)));
//            }
//            ++j;
//        }
//        ++i;
//    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << costs[i][j] << " ";
        }
        cout << "\n";
    }

//    costs.reserve(N); // time taken by drill to move from i to j, ∀ (i, j) ∈ A
//    indexes.reserve(N);
//    for (int i = 0; i < N; i++) {
//        costs[i].reserve(N);
//        indexes[i] = i;
//        for (int j = 0; j < N; j++) {
//            if (i == j)
//                costs[i].push_back(0.0);
//            else
//                costs[i].push_back(getRandomValue(10, 100));
//        }
//    }


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
void solver(CEnv env, Prob lp) {
    map_y.resize(N);

    cout << "N " << N << endl;
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

int main(int argc, char const *argv[]) {
    try {
        DECL_ENV(env);
        DECL_PROB(env, lp);

        int numVars = 1;
//        cout << "Choose the number of holes: ";
//        cin >> numVars;
//        cout << "The board has " << numVars << " holes\n";
//        // setup problem
//        setupProblem(env, lp, numVars);

        // solve problem
        std::string path = "/inputs";
        DIR *dr;
        struct dirent *dp;
        dr = opendir("./inputs");
        std::vector <std::string> filesOrdered;
        if (dr) {
            while ((dp = readdir(dr)) != NULL) {
                if (std::string(dp->d_name).find("distances") != std::string::npos) {
                    filesOrdered.push_back(std::string(dp->d_name));
                }
            }
            closedir(dr);
            std::sort(filesOrdered.begin(), filesOrdered.end());
        }
        for(std::vector<std::string>::iterator it = std::begin(filesOrdered); it != std::end(filesOrdered); ++it) {
            std::string filePath = *it;
            std::string outputPath = "outputs/" + filePath.substr(0, filePath.size() - 4) + ".sol";
            std::cout << filePath << std::endl;

//            std::cout << "Num vars " << numVars << std::endl;
            setCosts(filePath);
            //                std::cout << entry.path() << std::endl;
            std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
            solver(env, lp);

            CHECKED_CPX_CALL(CPXmipopt, env, lp);

            // print
            double objval;
            CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
            std::cout << "Objval: " << objval << std::endl;

            int num = CPXgetnumcols(env, lp);
            std::vector<double> vars;
            vars.resize(num);
            int fromIdx = 0;
            int toIdx = numVars - 1;
            CHECKED_CPX_CALL(CPXgetx, env, lp, &vars[0], fromIdx, toIdx);

            CHECKED_CPX_CALL(CPXsolwrite, env, lp, "solution.sol");

            std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            cout << "Time taken by function: "
                 << duration.count() << " milliseconds" << endl;
//            break;
        }

    } catch(std::exception& e)
    {
        cout << "EXCEPTION: " << e.what() << std::endl;
    }
    return 0;
}
