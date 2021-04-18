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
#include "cplex/cplex_solver.h"
#include "ant_colony/ant_colony_solver.h"

using namespace std;

int main(int argc, char const *argv[]) {
    try {
        int mode = -1;
        string mode_str = "";
        while (mode != 0 && mode != 1) {
            cout << "Select method (0: CPLEX, 1: Ant colony): ";
            cin >> mode_str;
            if (mode_str == "0" || mode_str == "1")
                mode = stoi(mode_str);
        }

        srand((unsigned int)time(NULL));
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
        for(vector<string>::iterator it = std::begin(filesOrdered); it != std::end(filesOrdered); ++it) {
            std::string filePath = *it;
            std::string outputPath = "outputs/" + filePath.substr(0, filePath.size() - 4) + ".sol";
            std::cout << filePath << std::endl;

            Problem* problem = new Problem(filePath);

            if (mode == 1) {
                bool with_sa = true;
                string with_sa_str = "";
                cout << "Select if use Simulated Annealing (0: No, 1: Yes), default 1: ";
                cin >> with_sa_str;
                if (with_sa_str == "0")
                    with_sa = false;

                std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

                AntColonySolver* antColonySolver = new AntColonySolver(problem, problem->getN() * 2, 1.0, 2.0, 0.1, 2.0, problem->getN() * 2, 0, with_sa);
                antColonySolver->solve();

                std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                cout << "Min cost found is: " << problem->getMinCost() << " (";
                vector<int> solution = problem->getSolution();
                for (int i = 0; i < solution.size(); i++) {
                    cout << solution[i];
                    if (i < solution.size() -1)
                        cout << ", ";
                }
                cout << ")" << endl;
                cout << "Time taken by function: " << duration.count() << " milliseconds" << endl << endl;

                delete antColonySolver;
//                break;
            } else {
                std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

                CPLEXSolver* cplexSolver = new CPLEXSolver(problem);
                cplexSolver->solve();

                std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                cout << "Min cost found is: " << problem->getMinCost() << " (";
                vector<int> solution = problem->getSolution();
                for (int i = 0; i < solution.size(); i++) {
                    cout << solution[i];
                    if (i < solution.size() -1)
                        cout << ", ";
                }
                cout << ")" << endl;
                cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;

                delete cplexSolver;
            }

            delete problem;
        }
    } catch(std::exception& e)
    {
        cout << "EXCEPTION: " << e.what() << std::endl;
    }
    return 0;
}
