
#include <iostream>
#include <queue>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

// 买卖股票的最佳时机II

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int res = 0;
        for(int i=1; i<prices.size(); i++){
            if(prices[i]>prices[i-1])
                res += prices[i]-prices[i-1];
        }
        return res;
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