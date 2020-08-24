
#include <iostream>
#include <queue>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

// 删除排序数组中的重复项

//思路1:可以将重复的元素用一个特殊值替代，再统一删除这个特殊值。
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        if(nums.size()==0)
            return 0;
        int res=1;
        int flag=nums[0];
        for(int i=1;i<nums.size();i++)
        {
            if(nums[i]==flag)
                nums[i]=INT_MAX;
            else
            {
                flag=nums[i];
            }        
        }
        nums.erase(remove(nums.begin(),nums.end(),INT_MAX),nums.end());
        return nums.size();
    }
};

//思路2:双指针法:下标j指向的永远是当前得到的无重复部分的最后一位，然后我们不关心下标j与下标i之间的内容
int removeDuplicates(vector<int>& nums) {
	if (nums.size() < 2) return nums.size();
	int j = 0;
	for (int i = 1; i < nums.size(); i++)
		if (nums[j] != nums[i]) nums[++j] = nums[i];
	return ++j;
}

//泛型编程
int removeDuplicates(vector<int>& nums) {
        auto w = unique(nums.begin(),nums.end());
        nums.erase(w, nums.end());
        return nums.size();
    }

//同思路2
class Solution1 {
public:
    int removeDuplicates(vector<int>& nums)    
    {        
        if((int)nums.size() == 0) return 0;
        int a = 0;
        for(int i = 1; i < nums.size(); i++)            
        {
            if(nums[i] > nums[a])
            {                
                a++;
                nums[a] = nums[i];
            }
        }
        return a+1;
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