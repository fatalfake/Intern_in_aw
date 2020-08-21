#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

//原地置换
/**
 * 如果没有重复数字，那么正常排序后，数字i应该在下标为i的位置，
 * 所以思路是重头扫描数组，遇到下标为i的数字如果不是i的话，（假设为m),
 * 那么我们就拿与下标m的数字交换。在交换过程中，如果有重复的数字发生，那么终止返回ture
 **/
class Solution
{
public:
    int findRepeatNumber(vector<int> &nums)
    {
        for (int i = 0; i < nums.size(); i++)
            while (nums[i] != i)
                if (nums[i] == nums[nums[i]])
                    return nums[i];
                else
                    swap(nums[i], nums[nums[i]]);
        return -1;
    }
};

//map
class Solution1
{
public:
    int findRepeatNumber(vector<int> &nums)
    {
        unordered_map<int, int> map;
        for (int i : nums)
            if (map[i]++ == 1)
                return i;
        return -1;
    }
};

//set
class Solution2
{
public:
    int findRepeatNumber(vector<int> &nums)
    {
        unordered_set<int> set;
        for (int i : nums)
        {
            if (set.count(i) == 1)
                return i;
            else
                set.insert(i);
        }
        return -1;
    }
};

int main()
{
    cout << "This is a leetcode train file." << endl;
    return 0;
}