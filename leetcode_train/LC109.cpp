
#include <iostream>
#include <queue>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

/*
 *Definition for singly-linked list.
 */
struct ListNode
{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

/**
 * Definition for a binary tree node.
 */
struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

/*
用两个指针，一块一慢，快的每次走两步，慢的每次走一步，这样当快指针遍历结束时，慢指针指向的也就是链表的中间位置。这时候把中间位置的结点的值作为二叉搜索树当前结点的值
*/

class Solution
{
public:
    TreeNode *sortedListToBST(ListNode *head)
    {
        TreeNode *root;
        if (!head)
            return nullptr;

        if (!head->next)
        {
            root = new TreeNode(head->val);
            return root;
        }

        ListNode *slow = head;
        ListNode *fast = head;
        ListNode *prev = head;

        while (fast != NULL && fast->next != NULL)
        {
            fast = fast->next->next;
            slow = slow->next;
        }

        while (prev->next != slow)
            prev = prev->next;
        root = new TreeNode(slow->val);
        ListNode *headRight = slow->next;
        prev->next = nullptr;
        root->left = sortedListToBST(head);
        root->right = sortedListToBST(headRight);

        return root;
    }
};

int main()
{
    vector<int> b;
    b = vector<int>();
    cout << "This is a leetcode train file." << endl;
    for (int k = 1; k < 50; k++)
    {
        cout << "K: " << k << "    "
             << "K&1:  " << (k & 1) << endl;
    }
    return 0;
}