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
/**
 * 方法一：找规律

思路与算法

博弈类的问题常常让我们摸不着头脑。当我们没有解题思路的时候，不妨试着写几项试试：

    N=1 的时候，区间 (0, 1) 中没有整数是 n 的因数，所以此时 Alice 败。
    N=2 的时候，Alice 只能拿 1，NNN 变成 1，Bob 无法继续操作，故 Alice 胜。
    N=3 的时候，Alice 只能拿 1，NNN 变成 2，根据 N=2 的结论，我们知道此时 Bob 会获胜，Alice 败。
    N=4 的时候，Alice 能拿 1 或 2，如果 Alice 拿 1，根据 N=3 的结论，Bob 会失败，Alice 会获胜。
    N=5 的时候，Alice 只能拿 1，根据 N=4 的结论，Alice 会失败。
    ......

写到这里，也许你有了一些猜想。没关系，请大胆地猜想，在这种情况下大胆地猜想是 AC 的第一步。也许你会发现这样一个现象：N 为奇数的时候 Alice（先手）必败，N 为偶数的时候 Alice 必胜。 这个猜想是否正确呢？下面我们来想办法证明它。

证明

    N=1 和 N=2 时结论成立。

    N>2 时，假设 N≤k 时该结论成立，则 N=k+1 时：
        如果 k 为偶数，则 k+1 为奇数，x 是 k+1 的因数，只可能是奇数，而奇数减去奇数等于偶数，且 k+1−x≤k，故轮到 Bob 的时候都是偶数。而根据我们的猜想假设 N≤k 的时候偶数的时候先手必胜，故此时无论 Alice 拿走什么，Bob 都会处于必胜态，所以 Alice 处于必败态。
        如果 k 为奇数，则 k+1 为偶数，x 可以是奇数也可以是偶数，若 Alice 减去一个奇数，那么 k+1−x 是一个小于等于 k 的奇数，此时 Bob 占有它，处于必败态，则 Alice 处于必胜态。

综上所述，这个猜想是正确的。

方法二：递推

在「方法一」中，我们写出了前面几项的答案，在这个过程中我们发现，Alice 处在 N=k 的状态时，他（她）做一步操作，必然使得 Bob 处于 N=m(m<k) 的状态。因此我们只要看是否存在一个 m 是必败的状态，那么 Alice 直接执行对应的操作让当前的数字变成 m，Alice 就必胜了，如果没有任何一个是必败的状态的话，说明 Alice 无论怎么进行操作，最后都会让 Bob 处于必胜的状态，此时 Alice 是必败的。

结合以上我们定义 f[i] 表示当前数字 i 的时候先手是处于必胜态还是必败态，true 表示先手必胜，false 表示先手必败，从前往后递推，根据我们上文的分析，枚举 i 在 (0,i) 中 i 的因数 j，看是否存在 f[i−j] 为必败态即可。

**/
class Solution
{
public:
    bool divisorGame(int N)
    {
        return N % 2 == 0;
    }
};

class Solution1 {
public:
    bool divisorGame(int N) {
        vector<int> f(N + 5, false);

        f[1] = false;
        f[2] = true;
        for (int i = 3; i <= N; ++i) {
            for (int j = 1; j < i; ++j) {
                if (i % j == 0 && !f[i - j]) {
                    f[i] = true;
                    break;
                }
            }
        }

        return f[N];
    }
};


int main()
{
    cout << "This is a leetcode train file.";
    return 0;
}