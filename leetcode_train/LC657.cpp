
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

/***
 * 机器人能够返回原点
***/


class Solution {
public:
    bool judgeCircle(string moves) {
        int record[4] = {0,0,0,0};
        for(char c:moves){
            switch (c)
            {
            case 'L':
                record[0]++;
                break;
            case 'R':
                record[1]++;
                break;
            case 'U':
                record[2]++;
                break;
            case 'D':
                record[3]++;
                break;
            default:
                break;
            }
        }
        if(record[0]==record[1]&&record[2]==record[3]) return true;
        else return false;
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