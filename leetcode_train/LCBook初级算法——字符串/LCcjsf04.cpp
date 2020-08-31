
#include <iostream>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <memory.h>

#define INT_MAX 2147483647
#define INT_MIN -2147483648

using namespace std;

/***
 *  给定两个字符串 s 和 t ，编写一个函数来判断 t 是否是 s 的字母异位词。

示例 1:

输入: s = "anagram", t = "nagaram"
输出: true

示例 2:

输入: s = "rat", t = "car"
输出: false

说明:
你可以假设字符串只包含小写字母。

进阶:
如果输入字符串包含 unicode 字符怎么办？你能否调整你的解法来应对这种情况？
***/

//1 排序法
class Solution {
public:
    bool isAnagram(string s, string t) {
        if(s.size()!=t.size()) return false;
        vector<char> s_vec;
        vector<char> t_vec;
        for(char c:s){
            s_vec.push_back(c);
        }
        for(char c:t){
            t_vec.push_back(c);
        }
        sort(s_vec.begin(), s_vec.end());
        sort(t_vec.begin(), t_vec.end());
        for(int i=0;i<s_vec.size();i++){
            if(s_vec[i]!=t_vec[i]) return false;
        }
        return true;
    }
};

//2 哈希法
class Solution {
public:
    bool isAnagram(string s, string t) {
        int hash[128];
        memset(hash, 0, sizeof(int) * 128);
        for (char c : s) {
            hash[c - '0'] ++;
        }
        for (char c : t) {
            hash[c - '0'] --;
            if(hash[c - '0']<0)
                return false;
        }
        return true;
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