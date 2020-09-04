#include <iostream>
#include <queue>

using namespace std;

struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
/**
 * 方法一：双指针
 * 本题询问的是，s是否是t的子序列，因此只要能找到任意一种s在t中出现的方式，即可认为s是t的子序列。
 * 而当我们从前往后匹配，可以发现每次贪心地匹配靠前的字符是最优决策
 *     假定当前需要匹配字符c，而字符c在t中的位置x1与x2出现(x1<x2)，那么贪心取x1是最优解，因为x2后面
 *     能取到的字符，x1也能取到，并且通过x1与x2之间的可选字符，更有希望能匹配成功。
 * 这样，我们初始化两个指针i和j，分别指向s和t的初始位置。每次贪心地匹配。匹配成功则i和j同时右移，匹配s的下一个
 * 位置，匹配失败则j右移，i不变，尝试用t的下一个字符匹配s。
 * 
 * 最终如果i移动到s的末尾，就说明s是t的子序列。
 * 
 * 复杂度分析：时间复杂度：O(n+m)，其中n为s的长度。m为t的长度。每次无论是匹配成功还是失败，都有至少一个指针发生
 * 右移，两指针能够位移的总距离为n+m
 * 
 * 空间复杂度：O(1)
 * 
**/

class Solution {
public:
    bool isSubsequence(string s, string t) {
        int n = s.length(), m = t.length();
        int i = 0, j = 0;
        while (i < n && j < m) {
            if (s[i] == t[j]) {
                i++;
            }
            j++;
        }
        return i == n;
    }
};

/**
 * 方法二：动态规划
 * 
 * 考虑前面的双指针的做法，我们注意到我们有大量的时间用于在t中找到下一个匹配字符。 
 * 
 * 这样我们可以预处理出对于t的每一个位置，从该位置开始往后每一个字符第一次出现的位置。
 * 
 * 我们可以使用动态规划的方法实现预处理，令f[i][j]表示字符串t中从位置i开始往后字符j
 * 第一次出现的位置。在进行状态转移时，如果t中位置i的字符就是j，那么f[i][j]=i，否则
 * j出现在位置i+1开始往后，即f[i][j] = f[i+1][j],因此我们要倒过来进行动态规划，从后
 * 往前枚举i。
 * 
 * 这样我们可以写出状态转移方程：
 * 
 *          f[i][j] = (此处有大括号) i            t[i] = j
 *                    (同上)       f[i+1][j]    ！t[i] = j
 * 
 * 假定下标从0开始，那么f[i][j]中有0<=i<=m-1，对于边界状态f[m-1][..],我们置f[m][..]为
 * m，让f[m-1][..]正常进行转移。这样如果f[i][j]=m，则表示从位置i开始往后不存在字符j。
 * 
 * 这样我们可以利用f数组，每次O(1)地跳转到下一个位置，直到位置变为m或者s中的每一个字符都匹配成功。
 * 
 * 
 * 复杂度分析：
 * 时间复杂度：O(mx|Σ|+n)，其中n为s的长度，m为t的长度，Σ为字符集。预处理时间复杂度O(m)，判断子序列
 * 时间复杂度O(n)。
 *      如果是计算 k 个平均长度为 n 的字符串是否为 t 的子序列，则时间复杂度为 O(m×∣Σ∣+k×n)。
 * 空间复杂度：O(m×∣Σ∣)，为动态规划数组的开销
 * */

class Solution1 {
public:
    bool isSubsequence(string s, string t) {
        int n = s.size(), m = t.size();

        vector<vector<int> > f(m + 1, vector<int>(26, 0));
        for (int i = 0; i < 26; i++) {
            f[m][i] = m;
        }

        for (int i = m - 1; i >= 0; i--) {
            for (int j = 0; j < 26; j++) {
                if (t[i] == j + 'a')
                    f[i][j] = i;
                else
                    f[i][j] = f[i + 1][j];
            }
        }
        int add = 0;
        for (int i = 0; i < n; i++) {
            if (f[add][s[i] - 'a'] == m) {
                return false;
            }
            add = f[add][s[i] - 'a'] + 1;
        }
        return true;
    }
};


int main()
{
    cout << "This is a leetcode train file.";
    return 0;
}