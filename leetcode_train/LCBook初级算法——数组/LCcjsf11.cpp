
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std;

/***
 * 给定一个 n × n 的二维矩阵表示一个图像。
    将图像顺时针旋转 90 度。 说明：
    你必须在原地旋转图像，这意味着你需要直接修改输入的二维矩阵。请不要使用另一个矩阵来旋转图像。

    给定 matrix = 
            [
            [1,2,3],
            [4,5,6],
            [7,8,9]
            ],

原地旋转输入矩阵，使其变为:
            [
            [7,4,1],
            [8,5,2],
            [9,6,3]
            ]
给定 matrix =
            [
            [ 5, 1, 9,11],
            [ 2, 4, 8,10],
            [13, 3, 6, 7],
            [15,14,12,16]
            ], 

原地旋转输入矩阵，使其变为:
            [
            [15,13, 2, 5],
            [14, 3, 4, 1],
            [12, 6, 8, 9],
            [16, 7,10,11]
            ]
***/

class Solution {
public:
    void rotate(vector<vector<int>>& matrix) {
        int n = matrix.size();
        // cout<<n<<endl;
        for(int i=0;i<n;i++){
            for(int j=0;j<n-i-1;j++)
                // int temp = matrix[i][j];
                // matrix[i][j] = matrix[n-1-j][n-1-i];
                // matrix[n-1-j][n-1-i] = temp;
                swap(matrix[i][j],matrix[n-1-j][n-1-i]);
        }
        for(int i=0;i<n/2;i++){
            for(int j=0;j<n;j++)
                // int temp = matrix[i][j];
                // matrix[i][j] = matrix[n-1-i][j];
                // matrix[n-1-i][j] = temp;
                swap(matrix[i][j],matrix[n-1-i][j]);
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
    vector< vector<int> > m = {{1,2,3},{4,5,6},{7,8,9}};

    Solution s;
    s.rotate(m);
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            cout<<m[i][j];
        cout<<endl;
    }
    return 0;
}