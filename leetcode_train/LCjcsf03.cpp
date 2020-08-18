
#include <iostream>
#include <queue>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

// 旋转数组

class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        int n = nums.size();
        if (k>n) k = k%n;
        for (int i=0;i<n-k;i++){
            nums.insert(nums.end(),nums[0]);
            nums.erase(nums.begin());
        }
    }
};

//环状替代解法，解法图解见LCjcsf03.png
class Solution1 {
public:
    void rotate(vector<int>& nums, int k) {
        k = k % nums.size();
        int count = 0;
        for (int start = 0; count < nums.size(); start++) {
            int current = start;
            int prev = nums[start];
            do {
                int next = (current + k) % nums.size();
                int temp = nums[next];
                nums[next] = prev;
                prev = temp;
                current = next;
                count++;
            } while (start != current);
        }
    }
};

/***
* 三次翻转解法：基于观察得来的解法。
* 这个算法基于以下事实：当我们旋转数组k次，k%n个尾部元素会被移动到头部，剩下的元素会被向后移动。
* 在这个方法中，我们首先将所有元素反转。然后反转前k个元素，再反转后面n-k个元素，就能得到想要的结果。

    例子：n=7 , k=3:

    原始数组                  : 1 2 3 4 5 6 7
    反转所有数字后             : 7 6 5 4 3 2 1
    反转前 k 个数字后          : 5 6 7 4 3 2 1
    反转后 n-k 个数字后        : 5 6 7 1 2 3 4 --> 结果

***/
class Solution2 {
public:
    void rotate(vector<int>& nums, int k) {
        k = k % nums.size();
        int count = 0;
        // reverse1(nums, 0, nums.size()-1);
        // reverse1(nums, 0, k-1);
        // reverse1(nums, k, nums.size()-1);
        reverse(nums.begin(),nums.end());
        reverse(nums.begin(), nums.begin()+k);
        reverse(nums.begin()+k, nums.end());
        //cpp标准库就有反转函数，不需要自己再写了，虽然可能比自己写的慢一点点。注意标准库的区间是左闭右开的
    }
    void reverse1(vector<int>& nums, int start, int end){
        while(start<end){
            swap(nums[start],nums[end]);
            start++;
            end--;
        }
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