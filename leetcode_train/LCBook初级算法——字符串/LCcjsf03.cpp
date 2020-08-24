
#include <iostream>
#include <queue>
#include <algorithm>
#include <unordered_map>

#define INT_MAX 2147483647
#define INT_MIN -2147483648

using namespace std;

/***
 *  给定一个字符串，找到它的第一个不重复的字符，并返回它的索引。如果不存在，则返回 -1。

示例：

s = "leetcode"
返回 0
s = "loveleetcode"
返回 2

提示：你可以假定该字符串只包含小写字母。
***/

//哈希
class Solution {
public:
    int firstUniqChar(string s) {
        unordered_map<char, int> store;
        for(char c:s){
            store[c]++;
        }
        for(int i=0;i<s.size();i++){
            if(store[s[i]]==1){
                return i;
            }
        }
        return -1;
    }
};

//利用find： 如果该字符第一次出现的位置和最后一次出现的位置一样，就证明不重复。从前到后，遍历输出。
class Solution1 {
public:
    int firstUniqChar(string s) {
        for(int i=0;i<s.size();i++){
            if(s.find(s[i])==s.rfind(s[i]))
            return i;
        }
        return -1;
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