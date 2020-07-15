#include<iostream>
#include<queue>

using namespace std;



/**
快慢指针法

https://juejin.im/post/5e64a20ff265da570a5d5633
**/

class Solution {
public:
    int findDuplicate(vector<int>& nums) {
        int slow = 0, fast = 0;
        do {
            slow = nums[slow];
            fast = nums[nums[fast]];
        } while (slow != fast);
        slow = 0;
        while (slow != fast) {
            slow = nums[slow];
            fast = nums[fast];
        }
        return slow;
    }
};

int main(){
    cout<<"This is a leetcode train file.";
    return 0;
}