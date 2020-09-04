#include<iostream>
#include<queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
    int minDepth(TreeNode* root) {
        if (root==nullptr) 
            return 0;
        return helper(root, 0);
    }
    int helper(TreeNode* root, int d){
        d++;
        if(root->left==nullptr && root->right==nullptr)
            return d;
        else if(root->left==nullptr)
            return helper(root->right,d);
        else if(root->right==nullptr)
            return helper(root->left,d);
        return min(helper(root->left, d), helper(root->right, d));
    }
};

class Solution1 {
public:
    int minDepth(TreeNode* root) {
        // 此题需注意叶子节点的定义，比较抽象，可以想象一下输入为[1,2]的情况，
        // 此时如果单纯返回 1 + min(minDepth(root->left), minDepth(root->right))，则得 1，因为结点1的右子节点深度计算出来为 0，
        // 而实际上，节点2才是符合要求的节点。所以不能单纯的递归比较2个子节点谁的深度小。

        // 首先，节点为空，直接返回0
        if(root == nullptr) return 0;
        
        // 其次，若当前节点的左右子节点均存在，说明***当前节点不是叶子节点，其孩子节点的性质也无法判定***，故返回较小的孩子节点深度
        if(root->left && root->right) return 1 + min(minDepth(root->left), minDepth(root->right));
        
        // 最后，若当前节点的左右子节点有一个不存在，则说明***当前节点的子节点中存在叶子节点***，故返回不为空的孩子节点（叶子节点）的深度
        // 当然了，若当前节点的左右子节点都不存在，那它就是叶子节点，总深度直接 +1 就好了，也符合这个 return 句法。
        return 1 + max(minDepth(root->left), minDepth(root->right));
    }
};

int main(){
    cout<<"This is a leetcode train file.";
    return 0;
}