#pragma GCC diagnostic error "-std=c++11"

#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <algorithm>

using namespace std;


struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        //递归分治
        return recursionBuild(preorder.begin(),preorder.end(),inorder.begin(),inorder.end());
    }

    //递归分治
    TreeNode* recursionBuild(vector<int>::iterator preBegin, vector<int>::iterator preEnd,vector<int>::iterator inBegin, vector<int>::iterator inEnd )
    {
        //迭代器的位置：preBegin指向前序遍历的第一个数字，preEnd指向前序遍历最后一个数字之后的一个位置。inPreEnd同理。在下面的递归中迭代器的计算由此为基础
        if(inEnd==inBegin) return NULL;
        TreeNode* cur = new TreeNode(*preBegin);
        auto root = find(inBegin,inEnd,*preBegin);  //在中序遍历中找到根节点的位置，左子树在中序遍历的范围是：[inBegin,root)，右子树在中序遍历中的范围：[root+1，inEnd）
        cur->left = recursionBuild(preBegin+1,preBegin+1+(root-inBegin),inBegin,root); //root-inBegin计算出的结果就是左子树的大小。
        //左子树在前序遍历中的范围：[preBegin+1,preBegin+1+(root-inBegin) )，右子树在前序遍历中的范围：[preBegin+1+(root-inBegin),preEnd）
        cur->right = recursionBuild(preBegin+1+(root-inBegin),preEnd,root+1,inEnd);
        return cur;
    }
};

class Solution1 {
public:
    unordered_map<int,int> map;
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        // 将中序序列用哈希表存储，便于查找根节点
        for(int i = 0;i < inorder.size();i++)
            map[inorder[i]] = i;
        // 传入参数：前序,中序，前序序列根节点，中序序列左边界，中序序列右边界
        return build(preorder,inorder,0,0,inorder.size()-1);
    }
    TreeNode* build(vector<int>& preorder, vector<int>& inorder,int pre_root,int in_left,int in_right){
        if(in_left > in_right)
            return NULL;
        TreeNode* root = new TreeNode(preorder[pre_root]);
        // 根节点在中序序列中的位置，用于划分左右子树的边界
        int in_root = map[preorder[pre_root]];
        // 左子树在前序中的根节点位于：pre_root+1,左子树在中序中的边界：[in_left,in_root-1]
        root->left = build(preorder,inorder,pre_root+1,in_left,in_root-1);
        // 右子树在前序中的根节点位于：根节点+左子树长度+1 = pre_root+in_root-in_left+1
        // 右子树在中序中的边界：[in_root+1,in_right]
        root->right = build(preorder,inorder,pre_root+in_root-in_left+1,in_root+1,in_right);
        return root;
    }
};

int main()
{
    cout << "This is a leetcode train file." << endl;
    return 0;
}