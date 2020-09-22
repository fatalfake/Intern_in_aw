#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

string& ClearAllSpace(string &str)
{
    int index = 0;
    if( !str.empty())
    {
        while( (index = str.find(' ',index)) != string::npos)
        {
            str.erase(index,1);
        }
    }
    return str;
}

class Solution1 {
public:
    bool isPalindrome(string s) {
        string sgood;
        for (char ch: s) {
            if (isalnum(ch)) {
                sgood += tolower(ch);
            }
        }
        string sgood_rev(sgood.rbegin(), sgood.rend());
        return sgood == sgood_rev;
    }
};

//双指针  LC125
class Solution2 {
public:
    bool isPalindrome(string s) {
        string sgood;
        for (char ch: s) {
            if (isalnum(ch)) {
                sgood += tolower(ch);
            }
        }
        int n = sgood.size();
        int left = 0, right = n - 1;
        while (left < right) {
           if (sgood[left] != sgood[right]) {
                return false;
            }
            ++left;
            --right;
        }
        return true;
    }
};

//空间复杂度优化：直接在s上判断，而不用新建字符串
class Solution3 {
public:
    bool isPalindrome(string s) {
        int n = s.size();
        int left = 0, right = n - 1;
        while (left < right) {
            while (left < right && !isalnum(s[left])) {
                ++left;
            }
            while (left < right && !isalnum(s[right])) {
                --right;
            }
            if (left < right) {
                if (tolower(s[left]) != tolower(s[right])) {
                    return false;
                }
                ++left;
                --right;
            }
        }
        return true;
    }
};


class Solution {
public:
    bool isPalindrome(string s) {
        string s_temp;
        s_temp = s;

        cout<<s_temp<<endl;

        for(int i=0;i<s_temp.size();i++){
            if(!isdigit(s_temp[i])&& !isalpha(s_temp[i]))
                {
                    s_temp.erase(i,1);
                    i--;
                }

            //if(s_temp[i]==' ')
                //s_temp.erase(i,1);
        }
        transform(s_temp.begin(), s_temp.end(), s_temp.begin(), ::tolower);

        cout<<s_temp<<endl;

        bool flag = true;
        for(int i=0;i<s_temp.size()/2;i++){
            cout<<"s[i] is: "<<s_temp[i]<<endl;
            cout<<"s[size-1-i] is: "<<s_temp[s_temp.size()-1-i]<<endl;
            if(s_temp[i]!=s_temp[s_temp.size()-1-i]){
                flag = false;
                break;
            }
        }
        return flag;
    }
};

int main() {
    string s = "Clare";
    // toUpper
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    // toLower
    //transform(s.begin(),s.end(),s.begin(), ::tolower);
    cout << s << endl;

    Solution s1;

    cout<<s1.isPalindrome("A man, a plan, a canal: Panama");
    return 0;
}