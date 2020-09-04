#include<iostream>
#include<queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

/***
 * 我们可以将字符串s按照0和1的连续段分组，存在counts数组中，例如s = 00111011，可以得到这样的counts数组：counts={2,3,1,2}。

这里counts数组中两个相邻的数一定代表的是两种不同的字符。假设 counts 数组中两个相邻的数字为u 或者v，它们对应着u 个0 和v 个 1，或者 u 个 1 和 v 个0。它们能组成的满足条件的子串数目为 min⁡{u,v}，即一对相邻的数字对答案的贡献。

我们只要遍历所有相邻的数对，求它们的贡献总和，即可得到答案。

 * **/
class Solution {
public:
    int countBinarySubstrings(string s) {
        vector<int> counts;
        int ptr = 0, n = s.size();
        while (ptr < n) {
            char c = s[ptr];
            int count = 0;
            while (ptr < n && s[ptr] == c) {
                ++ptr;
                ++count;
            }
            counts.push_back(count);
        }
        int ans = 0;
        for (int i = 1; i < counts.size(); ++i) {
            ans += min(counts[i], counts[i - 1]);
        }
        return ans;
    }
};

/**
 * 改进版：
 * 对于某一个位置i，其实我们只关心 i−1 位置的counts 值是多少，所以可以用一个last 变量来维护当前位置的前一个位置，这样可以省去一个counts 数组的空间。

**/
class Solution1 {
public:
    int countBinarySubstrings(string s) {
        int ptr = 0, n = s.size(), last = 0, ans = 0;
        while (ptr < n) {
            char c = s[ptr];
            int count = 0;
            while (ptr < n && s[ptr] == c) {
                ++ptr;
                ++count;
            }
            ans += min(count, last);
            last = count;
        }
        return ans;
    }
};


int main()
{
    cout << "This is a leetcode train file.";
    return 0;
}