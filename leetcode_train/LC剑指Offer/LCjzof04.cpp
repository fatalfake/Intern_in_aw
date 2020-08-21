#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class Solution
{
public:
    bool findNumberIn2DArray(vector<vector<int>> &matrix, int target)
    {
        int rows = matrix.size();
        if (rows == 0)
            return false;
        int cols = matrix[0].size();
        if (cols == 0)
            return false;

        int i = cols - 1;
        int j = 0;
        while (1)
        {
            if (matrix[j][i] == target)
                return true;
            else if (matrix[j][i] < target)
                j++;
            else if (matrix[j][i] > target)
                i--;
            if (j >= rows)
                return false;
            if (i < 0)
                return false;
        }
    }
};

int main()
{
    cout << "This is a leetcode train file." << endl;
    return 0;
}