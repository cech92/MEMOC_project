#include <vector>
#include <string>

using namespace std;

class Problem {
private:
    vector<vector<double> > costs;
    unsigned int N;
public:
    Problem(string path);
    vector<vector<double> > getCosts();
    unsigned int getN();
};