
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

// 给定一个整数数组，判断是否存在重复元素

/*** 
* 最基础的解法就是暴力线性查找：对于第i个数，搜索它前面i-1个数是不是和他有重复的，若有则退出循环。这样可以保证循环不变式：在下一次搜索之前，搜索过的数中没有重复的。
* 这个算法在LC上显然是无法AC的。他的复杂度为O(n^2)
* 一般而言，如果一个算法的时间复杂度为 O(n^2)，它最多能处理 n 大约为 10^4 的数据。当 n 接近 10^5 时就会超时

***/


//使用set
class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        set<int> s = set<int>();
        bool flag = false;
        for(auto n:nums){
            if(s.count(n)==0){
                s.insert(n);
            }
            else
            {
                flag = true;
                break;
            }
            
        }
        return flag;
    }
};

//unordered_map
class Solution1 {
public:
    bool containsDuplicate(vector<int>& nums) {

        unordered_map <int,int>mp;
        for(int i:nums)
        {
            mp[i]++;  //i对应的value值++
            if(mp[i]>1)     return true;    //i对应的value值大于1，则说明存在重复元素  
        }
        return false;
    }
};


//排序，排序完成后重复元素应该相邻。复杂度取决于排序算法复杂度
class Solution2 {
public:
    bool containsDuplicate(vector<int>& nums) {
        sort(nums.begin(),nums.end());
        for(int i=1;i<nums.size();i++){
            if(nums[i]==nums[i-1])
                return true;
        }
        return false;
    }
};

//利用集合的性质，与原数组比较大小
class Solution3 {
public:
    bool containsDuplicate(vector<int>& nums) {

         return nums.size() > unordered_set<int>(nums.begin(), nums.end()).size(); 
         //如果原数组的大小>集合的大小，则说明存在重复元素
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