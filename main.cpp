#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include "cpxmacro.h"
#include "cplex/cplex_solver.h"
#include "ant_colony/ant_colony_solver.h"
#include "simulated_annealing/simulated_annealing_solver.h"

using namespace std;

int main(int argc, char const *argv[]) {
    try {
        int mode = -1;
        bool with_sa = true;
        string mode_str = "";
        string with_sa_str = "";
        string path = "";
        string time_limit = "";
        while (mode != 0 && mode != 1 && mode != 2) {
            cout << "Select method (0: CPLEX, 1: Ant colony, 2: Simulated Annealing) [default = 0]: ";
            std::getline(std::cin, mode_str);
            if (mode_str == "1") {
                mode = stoi(mode_str);
                if (mode == 1) {
                    cout << "Apply Simulated Annealing (0: No, 1: Yes) [default = 1]? ";
                    std::getline(std::cin, with_sa_str);
                    if (with_sa_str == "0")
                        with_sa = false;
                }
            } else if (mode_str == "2") {
                mode = stoi(mode_str);
            } else {
                mode = 0;
            }
            cout << "Insert a time limit in seconds [default = 300]: ";
            std::getline(std::cin, time_limit);
            if (time_limit.empty())
                time_limit = "300";
            cout << "Insert the name of the file inside the inputs folder or leave it blank to test all the instances: ";
            std::getline(std::cin, path);
            cout << endl;
        }

        srand((unsigned int)time(NULL));
        std::string inputs_folder = "/inputs";
        DIR *dr;
        struct dirent *dp;
        dr = opendir("./inputs");
        std::vector <std::string> filesOrdered;
        if (path != "") {
            filesOrdered.push_back(path);
        } else {
            if (dr) {
                while ((dp = readdir(dr)) != NULL) {
                    if (std::string(dp->d_name).find(".txt") != std::string::npos) {
                        filesOrdered.push_back(std::string(dp->d_name));
                    }
                }
                closedir(dr);
                std::sort(filesOrdered.begin(), filesOrdered.end());
            }
        }
        for(vector<string>::iterator it = std::begin(filesOrdered); it != std::end(filesOrdered); ++it) {
            std::string filePath = *it;
            std::string outputPath = "outputs/" + filePath.substr(0, filePath.size() - 4) + ".sol";
            std::cout << filePath << std::endl;

            Problem* problem = new Problem(filePath);
            auto start = std::chrono::high_resolution_clock::now();

            if (mode == 1) {
                AntColonySolver* antColonySolver = new AntColonySolver(problem, problem->getN(), 1.0, 2.0, 0.1, 1.0, problem->getN()*8, stoi(time_limit), with_sa);
                antColonySolver->solve();

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                cout << "Min cost found is: " << problem->getMinCost() << " (";
                vector<int> solution = problem->getSolution();
                for (int i = 0; i < solution.size(); i++) {
                    cout << solution[i];
                    if (i < solution.size() -1)
                        cout << ", ";
                }
                cout << ")" << endl;
                cout << "Time taken: " << duration.count() << " milliseconds" << endl << endl;

                delete antColonySolver;
            } else if (mode == 2) {
                SimulatedAnnealingSolver* simulatedAnnealingSolver = new SimulatedAnnealingSolver(problem, problem->getN() * 20, stoi(time_limit));
                simulatedAnnealingSolver->solve();

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                cout << "Min cost found is: " << problem->getMinCost() << " (";
                vector<int> solution = problem->getSolution();
                for (int i = 0; i < solution.size(); i++) {
                    cout << solution[i];
                    if (i < solution.size() -1)
                        cout << ", ";
                }
                cout << ")" << endl;
                cout << "Time taken: " << duration.count() << " milliseconds" << endl << endl;

                delete simulatedAnnealingSolver;
            } else {
                    CPLEXSolver* cplexSolver = new CPLEXSolver(problem, time_limit);
                    cplexSolver->solve();

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                    cout << "Min cost found is: " << problem->getMinCost() << " (";
                    vector<int> solution = problem->getSolution();
                    for (int i = 0; i < solution.size(); i++) {
                        cout << solution[i];
                        if (i < solution.size() -1)
                            cout << ", ";
                    }
                    cout << ")" << endl;
                    cout << "Time taken: " << duration.count() << " milliseconds" << endl << endl;

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
