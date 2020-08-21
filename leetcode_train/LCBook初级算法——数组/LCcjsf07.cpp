
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

/***
 * 给定一个由整数组成的非空数组所表示的非负整数，在该数的基础上加一。
    最高位数字存放在数组的首位， 数组中每个元素只存储单个数字。
    你可以假设除了整数 0 之外，这个整数不会以零开头。

    输入: [1,2,3]
    输出: [1,2,4]
    解释: 输入数组表示数字 123。
    输入: [4,3,2,1]
    输出: [4,3,2,2]
    解释: 输入数组表示数字 4321。
***/

class Solution {
public:
    vector<int> plusOne(vector<int>& digits) {
        int n = digits.size();
        if(digits[n-1]!=9){
            digits[n-1]+=1;
        }
        else{
            for(int i = n-1;i>=0;i--){
                digits[i]+=1;
                if(digits[i]==10){
                    digits[i]=0;
                    if(i==0)
                        digits.insert(digits.begin(),1);
                    continue;
                }
                else
                    break;
            }
        }
        return digits;
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