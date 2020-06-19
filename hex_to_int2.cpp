#pragma GCC diagnostic error "-std=c++11" 

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <memory.h>
#include <cmath>
#include <cstdlib>
#include <iomanip>

using namespace std;
string string_to_hex(const string& str){
    //string result="0x";
    string result;
    string tmp;
    stringstream ss;
    for(int i=0;i<str.size();i++){
        ss<<hex<<int((unsigned char)str[i])<<endl;//unsigned char 很关键，如果符号位是1，不转换为unsigned，就会多一大堆f
        ss>>tmp;
        result+=tmp;
    }
    return result;
}

long long string_to_bit(const string& str)//transfer hex-string to bit
{
   
    long long result=strtoll(str.c_str(), NULL, 16);//第三个参数base为合法字符范围，base=2,为0、1，base=16，合法字符则为0-F，开头的0x自动忽略
    return result;
}

int main(int argc, char *argv[]){
    char buf[1024];
    string message;
    ifstream infile;
    infile.open(argv[1]);
    if(infile.is_open()){
        memset(buf,0,1024);
        infile.getline(buf,1024);
        //cout<<"buf:   "<<buf<<endl;
        message=buf;
        //cout<<"message:   "<<message<<endl;
    }
    //cout<<"size:   "<<message.size()<<endl;
    string message1 = string_to_hex(message);
    //cout<<message1<<endl;
    
    char x[] = "0x";
    
    string message2;
    string message3;

    message2 += x;
    message3 += x;

    for(int i=3;i>=0;--i){
        message2 += message1[i*2];
        message2 += message1[i*2+1];
        message3 += message1[i*2+8];
        message3 += message1[i*2+9];
    }
    
    long long time_s = string_to_bit(message2);
    long long time_ns = string_to_bit(message3);
    //cout<<"time(s):"<<time_s<<endl;
    //cout<<"time(ns):"<<time_ns<<endl;

    int len=0;
    long long t_time_ns=time_ns;
    while(t_time_ns){
        len++;
        t_time_ns/=10;
    }
    double d_time_ns = time_ns/pow(10,len);
    d_time_ns = round(d_time_ns*100)/100;
    double d_time_all = time_s + d_time_ns;
    cout<<fixed<<setprecision(2)<<d_time_all<<endl;
    infile.close();
    return 0;
}