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

class Solution {
public:
    int fib(int n) {
        if(n<=0)
            return 0;
        if(n==1)
            return 1;
        int fibMinusTwo = 0;
        int fibMinusOne = 1;
        int res;
        for(int i=2;i<=n;i++){
            res = (fibMinusOne + fibMinusTwo)%1000000007;
            fibMinusTwo = fibMinusOne;
            fibMinusOne = res;
        }
        return res;
    }
};

class Solution1 {
public:
    int fib(int n) {
        int arr[2] = {0, 1};
        for(int i = 2; i <= n; ++i) {
            arr[i & 1] = (arr[0] + arr[1]) % (int)(1e9 + 7);
            int k = i&1;
            cout << k <<endl;
        }
        return arr[n & 1];
    }
};


int main()
{
    cout << "This is a leetcode train file."<<endl;
    Solution1 s;
    cout<<s.fib(50);
    return 0;
}