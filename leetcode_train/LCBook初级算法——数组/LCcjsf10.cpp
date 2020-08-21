
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

/***
 * 判断一个 9x9 的数独是否有效。只需要根据以下规则，验证已经填入的数字是否有效即可。

    数字 1-9 在每一行只能出现一次。
    数字 1-9 在每一列只能出现一次。
    数字 1-9 在每一个以粗实线分隔的 3x3 宫内只能出现一次。
输入:
[
  ["5","3",".",".","7",".",".",".","."],
  ["6",".",".","1","9","5",".",".","."],
  [".","9","8",".",".",".",".","6","."],
  ["8",".",".",".","6",".",".",".","3"],
  ["4",".",".","8",".","3",".",".","1"],
  ["7",".",".",".","2",".",".",".","6"],
  [".","6",".",".",".",".","2","8","."],
  [".",".",".","4","1","9",".",".","5"],
  [".",".",".",".","8",".",".","7","9"]
]
输出: true


输入:
[
  ["8","3",".",".","7",".",".",".","."],
  ["6",".",".","1","9","5",".",".","."],
  [".","9","8",".",".",".",".","6","."],
  ["8",".",".",".","6",".",".",".","3"],
  ["4",".",".","8",".","3",".",".","1"],
  ["7",".",".",".","2",".",".",".","6"],
  [".","6",".",".",".",".","2","8","."],
  [".",".",".","4","1","9",".",".","5"],
  [".",".",".",".","8",".",".","7","9"]
]

说明：
一个有效的数独（部分已被填充）不一定是可解的。
只需要根据以上规则，验证已经填入的数字是否有效即可。
给定数独序列只包含数字 1-9 和字符 '.' 。
给定数独永远是 9x9 形式的。


解法：
一个简单的解决方案是遍历该 9 x 9 数独 三 次，以确保：

    行中没有重复的数字。
    列中没有重复的数字。
    3 x 3 子数独内没有重复的数字。

实际上，所有这一切都可以在一次迭代中完成。

首先，让我们来讨论下面两个问题：

    如何枚举子数独？

    可以使用 box_index = (row / 3) * 3 + columns / 3，其中 / 是整数除法。

    如何确保行 / 列 / 子数独中没有重复项？

    可以利用 value -> count 哈希映射来跟踪所有已经遇到的值。

现在，我们完成了这个算法的所有准备工作：

    遍历数独。
    检查看到每个单元格值是否已经在当前的行 / 列 / 子数独中出现过：
        如果出现重复，返回 false。
        如果没有，则保留此值以进行进一步跟踪。
    返回 true。


***/

//哈希map简洁写法：
class Solution {
public:
	bool isValidSudoku(vector<vector<char>>& board) {
		unordered_map<char, int>row[9], column[9], sub[9];
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if (board[i][j] != '.' && 
					(row[i][board[i][j]]++ ||
					column[j][board[i][j]]++ || 
					sub[i / 3 * 3 + j / 3][board[i][j]]++))
					return false;
		return true;
	}
};

//由于规模小，也可直接用数组代替哈希map
class Solution1 {
public:
    bool isValidSudoku(vector<vector<char>>& board) {
        vector<vector<int>> row (9, vector<int>(9,0));
        vector<vector<int>> col (9, vector<int>(9,0));
        vector<vector<int>> box (9, vector<int>(9,0));

        for(int i=0;i<9;i++){
            for(int j=0;j<9;j++){
                if(board[i][j] == '.'){
                    continue;
                }
                int val = board[i][j] - '1';
                int box_index = (i/3) * 3 + j/3;
                if(row[i][val] == 0 && col[j][val] == 0 && box[box_index][val] == 0){
                    row[i][val] = 1;
                    col[j][val] = 1;
                    box[box_index][val] = 1;
                }
                else{
                    return false;
                }
            }
        }
        return true;
    }
};

//更极限一点，用一个数组（有点抽象）
class Solution2 {
public:
    bool isValidSudoku(vector<vector<char>>& board) {
        vector<int> wow(9,0);
        int mux1;
        int mux2;
        int mux3;
        int box_index;

        for(int i=0;i<9;i++){
            for(int j=0;j<9;j++){
                if(board[i][j] == '.'){
                    continue;
                }
                mux1 = 0x01 << (board[i][j] - '1');
                mux2 = 0x01 << 9 << (board[i][j] - '1');
                mux3 = 0x01 << 9 << 9 << (board[i][j] - '1');
                box_index = (i/3) * 3 + j/3;
                if((wow[i]&mux1) != mux1 && (wow[j]&mux2) != mux2 && (wow[box_index]&mux3) != mux3){
                    wow[i] = wow[i]|mux1;
                    wow[j] = wow[j]|mux2;
                    wow[box_index] = wow[box_index]|mux3;
                }
                else{
                    return false;
                }
            }
        }
        return true;
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