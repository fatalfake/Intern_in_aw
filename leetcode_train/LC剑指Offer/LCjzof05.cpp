#pragma GCC diagnostic error "-std=c++11"

#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

//剑指offer原书题解
class Solution
{
public:
    string replaceSpace(string s)
    {
        int num_of_spaces = 0;
        for (auto c : s)
        {
            if (c == ' ')
                num_of_spaces++;
        }

        int slength = s.size();
        int nlength = slength + 2 * num_of_spaces;
        char newstr[nlength + 1];
        newstr[nlength] = '\0';
        int anchor = nlength - 1;
        for (int i = slength - 1; i >= 0; --i)
        {
            if (s[i] != ' ')
            {
                newstr[anchor] = s[i];
                anchor--;
            }
            else
            {
                newstr[anchor] = '0';
                anchor--;
                newstr[anchor] = '2';
                anchor--;
                newstr[anchor] = '%';
                anchor--;
            }
        }
        string res = newstr;
        return res;
    }
};

//C++ string特性利用题解
class Solution1 {
public:
    string replaceSpace(string s) {     //字符数组
        string array;   //存储结果
        
        for(auto &c : s){   //遍历原字符串
            if(c == ' '){
                array.push_back('%');
                array.push_back('2');
                array.push_back('0');
            }
            else{
                array.push_back(c);
            }
            /* 或者下面写法。写成上面只是为了显示出，C++的string类似于vector的本质
            if(c == ' '){
                array += "%20"；
            }
            else{
                array += 'c'；
            }
            */
        }
        return array;
    }
};


int main()
{
    cout << "This is a leetcode train file." << endl;
    Solution s;
    cout << s.replaceSpace("We are happy.");
    return 0;
}