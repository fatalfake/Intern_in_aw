#!/usr/bin/python
# -*-coding:utf-8-*-

import os


def get_caselist():
    """
    read caselist
    for each case ,check description
    caselist format: test_{Issue key}.*
    """
    caselist = []
    filename = "./config/regression_caselist"
    with open(filename, "r") as f:
        for line in f:
            caselist.append(line.strip("\n").strip())
    return caselist


def get_caselist_by_group(caselist):
    caselist_by_group = []
    group_id = 0
    temp = []
    for i in range(len(caselist)):
        cur_nonius = 5
        while caselist[i][cur_nonius] != '-':
            cur_nonius = cur_nonius+1
        cur_prefix = caselist[i][5:cur_nonius]
        # print("Cur_prefix is:"+cur_prefix)
        if i == 0:
            print(caselist[i] + " is in group " + str(group_id))
            temp.append(caselist[i])
            if(len(caselist)==1):
                caselist_by_group.append(temp)
        if i == len(caselist)-1:
            if len(temp) == 0:
                temp.append(caselist[i])
                caselist_by_group.append(temp) 
            break
        next_nonius = 5
        while(caselist[i+1][next_nonius] != '-'):
            next_nonius = next_nonius+1
        next_prefix = caselist[i+1][5:next_nonius]
        # print("Next_prefix is:"+next_prefix)
        if cur_prefix == next_prefix :
            print(caselist[i+1] + " is in group " + str(group_id))
            if len(temp) == 0:
                temp.append(caselist[i])
            temp.append(caselist[i+1])
        else :
            group_id = group_id + 1
            print(caselist[i+1] + " is in group " + str(group_id))
            if len(temp) == 0:
                temp.append(caselist[i])
            caselist_by_group.append(temp)
            temp = []
    for group in caselist_by_group:
        for case in group:
            print(case)
            print("**********")



if __name__ == "__main__":

    print("Caselist is:")
    print(get_caselist())
    print("Caselist_by_group is:")
    get_caselist_by_group(get_caselist())
