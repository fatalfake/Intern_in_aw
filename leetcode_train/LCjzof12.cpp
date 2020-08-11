

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

//DFS

class Solution {
public:
    int x[4] = {0,0,-1,1}; //x[4]、y[4]和dfs中for循环，共同构成遍历字符board[i][j]的“左右上下”四个字符这一功能
    int y[4] = {-1,1,0,0}; // 这里用的特别巧妙
    int rows,cols;
    bool dfs(vector<vector<char>>& board,string word,int i,int j,int num) {
        if (num == word.size()) return true;
        char tmp = board[i][j]; // 用于暂时保存字符
        board[i][j] = '.'; // 代表这个字符已经访问过
        for (int k = 0; k < 4; k++) {
            int d_x = x[k] + i; // 新的i值
            int d_y = y[k] + j; // 新的j值
            if (d_x >= 0 && d_x < rows && d_y >= 0 && d_y < cols && word[num] == board[d_x][d_y]) {
                if (dfs(board,word,d_x,d_y,num + 1)) return true;
            }
        }
        board[i][j] = tmp; // 遍历结束后改回来
        return false;
    }
    bool exist(vector<vector<char>>& board, string word) {
        rows = board.size();
        cols = board[0].size();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (board[i][j] == word[0]) {
                    if (dfs(board,word,i,j,1)) return true;
                }
            }
        }
        return false;
    }
};


int main()
{
    cout << "This is a leetcode train file."<<endl;
    for(int k = 1; k < 50; k++){
        cout<<"K: "<<k<<"    "<<"K&1:  "<<(k&1)<<endl;
    }
    return 0;
}