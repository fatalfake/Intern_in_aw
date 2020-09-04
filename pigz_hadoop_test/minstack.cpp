#include<iostream>
#include<stack>
using namespace std;
//包含min函数的栈
class Solution
{
public:
//定义栈的push函数，当有value进来时，先放入data_stack,
//再判断min_stack是否为空或者value<min_stack.top()，若是，则放入min_stack
//否则，把min_stack.top()放入min_stack中
	void push(int value)
	{
		data_stack.push(value);
		if (min_stack.empty() || value < min_stack.top())
		{
			min_stack.push(value);
		}
		else
			min_stack.push(min_stack.top());
	}
//定义栈的pop函数，若data_stack和min_stack都不为空时，将两个栈的栈顶元素pop出来
	void pop()
	{
		if (!data_stack.empty() && !min_stack.empty())
		{
			data_stack.pop();
			min_stack.pop();
		}
	}
//定义栈的top元素，返回data_stack的top
	int top()
	{
		return data_stack.top();
	}
//定义栈的min函数，返回min_stack的top
	int min()
	{
		return min_stack.top();
	}
//定义栈的empty函数，若data_stack和min_stack均为空时，返回true,否则返回false
	bool empty()
	{
		if (data_stack.empty() && min_stack.empty())
			return true;
		else
			return false;
	}
private:
//定义一个数据栈，一个辅助栈，辅助栈的栈顶永远指向最小的元素
	stack<int> data_stack;
	stack<int> min_stack;
};
 
int main()
{
	Solution min_stack;
	min_stack.push(1);
	min_stack.push(2);
	min_stack.push(3);
 
	printf("%d\n", min_stack.min());
 
	min_stack.pop();
 
	printf("%d\n", min_stack.min());
 
	min_stack.push(0);
	printf("%d\n", min_stack.top());
	printf("%d\n", min_stack.min());
 
	min_stack.pop();
	min_stack.pop();
	
	if (min_stack.empty())
		printf("最小栈为空！\n");
	else
		printf("最小栈不为空！\n");
 
	min_stack.pop();
 
	if (min_stack.empty())
		printf("最小栈为空！\n");
	else
		printf("最小栈不为空！\n");
 
	return 0;
}