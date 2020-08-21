
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

// 给定两个数组，编写一个函数来计算它们的交集。
// 输入：nums1 = [1,2,2,1], nums2 = [2,2]
// 输出：[2,2]
// 输入：nums1 = [4,9,5], nums2 = [9,4,9,8,4]
// 输出：[4,9]


//方法1：哈希
// 首先遍历第一个数组，并在哈希表中记录第一个数组中的每个数字以及对应出现的次数，然后遍历第二个数组，对于第二个数组中的每个数字，
// 如果在哈希表中存在这个数字，则将该数字添加到答案，并减少哈希表中该数字出现的次数。

// 为了降低空间复杂度，首先遍历较短的数组并在哈希表中记录每个数字以及对应出现的次数，然后遍历较长的数组得到交集。

class Solution {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        if (nums1.size() > nums2.size()) {
            return intersect(nums2, nums1);
        }
        unordered_map <int, int> m;
        for (int num : nums1) {
            ++m[num];
        }
        vector<int> intersection;
        for (int num : nums2) {
            if (m.count(num)) {
                intersection.push_back(num);
                --m[num];
                if (m[num] == 0) {
                    m.erase(num);
                }
            }
        }
        return intersection;
    }
};


//方法2：排序
class Solution2 {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        vector<int> res = vector<int>();
        sort(nums1.begin(),nums1.end());
        sort(nums2.begin(),nums2.end());
        int nums2Begin = 0;
        for(int i = 0; i<nums1.size(); i++){
            for(int j = nums2Begin; j<nums2.size(); j++){
                if(nums1[i]<nums2[j]){
                    break;
                }
                if(nums1[i]==nums2[j]){
                    res.push_back(nums1[i]);
                    nums2Begin = j+1;
                    break;
                }
                if(nums1[i]>nums2[j]){
                    nums2Begin = j+1;
                    continue;
                }    
            }
        }
        return res;
    }
};

//官方排序写法：
class Solution3 {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        sort(nums1.begin(), nums1.end());
        sort(nums2.begin(), nums2.end());
        int length1 = nums1.size(), length2 = nums2.size();
        vector<int> intersection;
        int index1 = 0, index2 = 0;
        while (index1 < length1 && index2 < length2) {
            if (nums1[index1] < nums2[index2]) {
                index1++;
            } else if (nums1[index1] > nums2[index2]) {
                index2++;
            } else {
                intersection.push_back(nums1[index1]);
                index1++;
                index2++;
            }
        }
        return intersection;
    }
};

/***
 * 如果 nums2​ 的元素存储在磁盘上，磁盘内存是有限的，并且你不能一次加载所有的元素到内存中。那么就无法高效地对 nums2​ 进行排序，
 * 因此推荐使用方法一而不是方法二。在方法一中，nums2 只关系到查询操作，因此每次读取 nums2​ 中的一部分数据，并进行处理即可。
 * 
 * **/


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