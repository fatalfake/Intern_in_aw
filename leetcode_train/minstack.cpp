#include<iostream>
#include<queue>

using namespace std;

struct MinStackElement {
    int data;
    int min;
};

struct MinStack {
    MinStackElement * data;
    int size;
    int top;
};

MinStack MinStackInit(int maxSize) {
    MinStack stack;
    stack.size = maxSize;
    stack.data = (MinStackElement)malloc(sizeof(MinStackElement)*maxSize);
    stack.top = 0;
    return stack;
}
void MinStackFree(MinStack stack) {
    free(stack.data);
}
void MinStackPush(MinStack stack, int d) {
    if (stack.top == stack.size)
        error("out of stack space.");
    MinStackElement* p = stack.data[stack.top];
    p->data = d;
    p->min = (stack.top==0?d : stack.data[top-1]);
    if (p->min > d)
        p->min = d;
    top++;
}
int MinStackPop(MinStack stack) {
    if (stack.top == 0)
        error("stack is empty!");
    return stack.data[--stack.top].data;
}
int MinStackMin(MinStack stack) {
    if (stack.top == 0)
        error("stack is empty!");
    return stack.data[stack.top-1].min;
}

int main() {
    cout<<"This is a leetcode train file.";
    return 0;
}