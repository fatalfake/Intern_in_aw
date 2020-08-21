#include <iostream>
#include <stack>

using namespace std;

class CQueue {
private:
    stack<int> stack1;
    stack<int> stack2;
public:
    CQueue() {}
    void appendTail(int value) {
        stack1.push(value);
    }
    int deleteHead() {
        if (!stack2.empty()) {
            int t = stack2.top();
            stack2.pop();
            return t;
        }
        else {
            while (!stack1.empty()) {
                int t = stack1.top();
                stack2.push(t);
                stack1.pop();
            }
            if (stack2.empty())
                return -1;
            else
            {
                int t1 = stack2.top();
                stack2.pop();
                return t1;
            }
        }
    }
};

class CQueue1{
    stack<int> a;
    stack<int> b;
    CQueue1() {

    }

    void appendTail(int value) {
        a.push(value);
    }

    int deleteHead() {
        if (b.empty() && a.empty())
            return -1;
        else if (b.empty() && !a.empty())
        {
            while (!a.empty())
            {
                b.push(a.top());
                a.pop();
            }
        }
        int temp = b.top();
         b.pop();
        return temp;     
    }
};
/**
 * Your CQueue object will be instantiated and called as such:
 * CQueue* obj = new CQueue();
 * obj->appendTail(value);
 * int param_2 = obj->deleteHead();
 */


int main()
{
    cout << "This is a leetcode train file."<<endl;

    return 0;
}