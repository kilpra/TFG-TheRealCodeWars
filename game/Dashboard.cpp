#include "dashboard.h"
using namespace std;
void Dashboard::saveDashboard(const Dashboard& dashboard, const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open()) {
        file << dashboard;
        file.close();
    } else {
        std::cout << "Error: could not open file " << filename << std::endl;
    }
}

bool Dashboard::loadDashboard(Dashboard& dashboard, const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> dashboard;
        file.close();
        return true;
    } else {
        std::cout << "Error: could not open file " << filename << std::endl;
        return false;
    }
}
