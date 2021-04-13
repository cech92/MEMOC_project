#include "../cpxmacro.h"
#include "../problem.h"

class CPLEXSolver {
private:
    Env env;
    Prob lp;

    int status;
    char errmsg[BUF_SIZE];

    static const int NAME_SIZE = 512;
    char name[NAME_SIZE];

    vector<vector<int> > map_y; // 1 if arc from i to j used, 0 otherwise, ∀ (i, j) ∈ A
    vector<vector<int> > map_x; // amount of the flow shipped from i to j, ∀ (i, j) ∈ A
    vector<int> indexes; // amount of the flow shipped from i to j, ∀ (i, j) ∈ A
    int first_index = 0;
public:
    CPLEXSolver(Problem *problem);
    void solve();
};