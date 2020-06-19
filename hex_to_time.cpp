#pragma GCC diagnostic error "-std=c++11" 

#include <iostream>
#include <sstream>
#include <ctime>
#include <cstring>
#include <string>
#include <fstream>
#include <memory.h>
#include <cmath>
#include <cstdlib>
#include <iomanip>

using namespace std;

typedef struct times
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int second;
} Times;

Times stamp_to_standard(int stampTime){
	time_t tick = (time_t)stampTime;
	struct tm tm;
	char s[100];
	Times standard;
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
	printf("%d: %s\n", (int)tick, s);
	standard.year = atoi(s);
	standard.mon = atoi(s+5);
	standard.day = atoi(s+8);
	standard.hour = atoi(s+11);
	standard.min = atoi(s+14);
	standard.second = atoi(s+17);
	return standard;
}

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
    string message = argv[1];
    string message1 = string_to_hex(message);
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
    
    int time_s = string_to_bit(message2);
    int time_ns = string_to_bit(message3);
    
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
    
    //Times t = stamp_to_standard(time_s);
    return time_s;
}
