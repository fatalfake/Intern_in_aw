#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <ctime>
#include <cstring>
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

int main(){
	Times t = stamp_to_standard(1589457580);
	return 0;
}