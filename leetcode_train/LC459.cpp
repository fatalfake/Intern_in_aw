
#include <iostream>
#include <queue>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

/***
 * 给定一个非空的字符串，判断它是否可以由它的一个子串重复多次构成。给定的字符串只含有小写英文字母，并且长度不超过10000。
 * 输入: "abab"
    输出: True
    解释: 可由子字符串 "ab" 重复两次构成。

    输入: "aba"
    输出: False

    输入: "abcabcabcabc"
    输出: True
    解释: 可由子字符串 "abc" 重复四次构成。 (或者子字符串 "abcabc" 重复两次构成。)
***/

//方法一：枚举 时间复杂度O(n^2) 空间O(1)
class Solution {
public:
    bool repeatedSubstringPattern(string s) {
        int n = s.size();
        for (int i = 1; i * 2 <= n; ++i) {
            if (n % i == 0) {
                bool match = true;
                for (int j = i; j < n; ++j) {
                    if (s[j] != s[j - i]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    return true;
                }
            }
        }
        return false;
    }
};

//方法二：
class Solution1 {
public:
    bool repeatedSubstringPattern(string s) {
        return (s + s).find(s, 1) != s.size();
    }
};


int main()
{
    vector<int> b;
    b = vector<int>();
    cout << "This is a leetcode train file."<<endl;
    for(int k = 1; k < 50; k++){
        cout<<"K: "<<k<<"    "<<"K&1:  "<<(k&1)<<endl;
    }
    return 0;
}