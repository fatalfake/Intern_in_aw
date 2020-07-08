#include <iostream>
#include <queue>

using namespace std;

class Solution
{
public:
    vector<int> divingBoard(int shorter, int longer, int k)
    {
        vector<int> res;
        if (k == 0)
            return res;
        int base = k * shorter;
        int gap = longer - shorter;
        if (shorter != longer)
        {
            for (int i = 0; i < k + 1; ++i)
            {
                res.push_back(base + i * gap);
                cout << res[i] << "---";
            }
        }
        else
        {
            res.push_back(base);
        }

        return res;
    }
};

int main()
{
    cout << "This is a leetcode train file." << endl;
    Solution *s = new Solution();
    s->divingBoard(1, 2, 3);
    return 0;
}