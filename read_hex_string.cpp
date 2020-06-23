#pragma GCC diagnostic error "-std=c++11"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory.h>
#include <stdlib.h>
#include <cmath>
#include <cstdlib>
#include <iomanip>

using namespace std;


string string_to_hex(const string &str)
{
    //string result="0x";
    string result;
    string tmp;
    stringstream ss;
    for (int i = 0; i < str.size(); i++)
    {
        ss << hex << int((unsigned char)str[i]) << endl; //unsigned char 很关键，如果符号位是1，不转换为unsigned，就会多一大堆f
        ss >> tmp;
        // cout<<"temp: "<<tmp<<endl;
        // cout<<"len:  "<<tmp.length()<<endl;
        // cout<<"new:  "<<"0"+tmp<<endl;
        if (tmp.length() == 1)
        {
            tmp = "0"+tmp;
        }
        
        result += tmp;
    }
    return result;
}

long long string_to_bit(const string& str)//transfer hex-string to bit
{
   
    long long result=strtoll(str.c_str(), NULL, 16);//第三个参数base为合法字符范围，base=2,为0、1，base=16，合法字符则为0-F，开头的0x自动忽略
    return result;
}

int main(int argc, char *argv[])
{
    //文件名
    string buf;
    // string toss;
    string message;
    ifstream infile;
    infile.open(argv[1]);
    // string message = argv[1];
    if (infile.is_open())
    {
        // memset(buf, 0, 1024);
        // getline(infile, toss);
        // cout<<"toss:   "<<toss<<endl;
        getline(infile, buf);
        // cout<<"buf:   "<<buf<<endl;
        message = buf;
        // cout<<"message:   "<<message<<endl;
    }
    // cout << "size:   " << message.size() << endl;
    int position = message.find("time=");
    // cout << "pos:    " << position << endl;
    position += 5;
    string message1;
    message1 = message.substr(position, position + 8);
    // cout << "mes1:   " << message1 << endl;
    string message2 = string_to_hex(message1);
    // cout << message2 << endl;
    string message3 = message2.substr(0, 16);
    // cout << message3 << endl;

    char x[] = "0x";

    string message4;
    string message5;

    message4 += x;
    message5 += x;

    for(int i=3;i>=0;--i){
        message4 += message3[i*2];
        message4 += message3[i*2+1];
        message5 += message3[i*2+8];
        message5 += message3[i*2+9];
    }
    
    // cout<<message4<<endl;
    // cout<<message5<<endl;
    
    int time_s = string_to_bit(message4);
    int time_ns = string_to_bit(message5);

    // cout<<time_s<<endl;
    // cout<<time_ns<<endl;

    int len=0;
    int t_time_ns=time_ns;
    while(t_time_ns){
        len++;
        t_time_ns/=10;
    }
    double d_time_ns = time_ns/pow(10,len);
    d_time_ns = round(d_time_ns*100)/100;
    double d_time_all = time_s + d_time_ns;
    cout<<fixed<<setprecision(2)<<d_time_all<<endl;

    return 0;
}