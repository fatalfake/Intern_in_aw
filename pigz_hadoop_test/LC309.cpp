#include<iostream>
#include<queue>

using namespace std;



/**
 * dp[i][0]表示第i天手上持有股票，那么第i-1天要么手上有股票，要么不是冷却期
 * dp[i][1]表示第i天是冷却期，那么第i-1天手上肯定有股票
 * dp[i][2]表示第i天没持有股票且不是冷却期，那么第i-1天肯定没持有股票
 * 根据上面三个结论就可以写出状态转移方程了，代码如下
**/

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int n=prices.size();
        vector<vector<int>>dp(n+1,vector<int>(3,0));
        dp[0][0]=-1e8;
        for(int i=0;i<n;i++){
            dp[i+1][0]=max(dp[i][2]-prices[i],dp[i][0]);
            dp[i+1][1]=dp[i][0]+prices[i];
            dp[i+1][2]=max(dp[i][1],dp[i][2]);
        }
        return max(dp[n][1],dp[n][2]);
    }
};

int main(){
    cout<<"This is a leetcode train file.";
    return 0;
}