# -*-coding:utf-8-*-

f1=open('tel.txt','rb')
f2=open('email.txt','rb')
f1.readline()
f2.readline()
lines1=f1.readlines()
lines2=f2.readlines()
lines1_name=[]
lines1_tel=[]
lines2_name=[]
lines2_email=[]
lines=['姓名\t\t电话\t\t\t\t邮箱\n']
for line in lines1:
    elements=line.split()
    lines1_name.append(str(elements[0].decode('utf-8')))
    lines1_tel.append(str(elements[1].decode('utf-8')))
for line in lines2:
    elements=line.split()
    lines2_name.append(str(elements[0].decode('utf-8')))
    lines2_email.append(str(elements[1].decode('utf-8')))
for i in range(len(lines1_name)):
    s=''
    if lines1_name[i] in lines2_name:
        j=lines2_name.index(lines1_name[i])
        s='\t\t'.join([lines1_name[i],lines1_tel[i],lines2_email[j]])
        s+='\n'
    else:
        s='\t\t'.join([lines1_name[i],lines1_tel[i],str('-----------------')])
        s+='\n'
    lines.append(s)
for i in range(len(lines2_name)):
    s=''
    if lines2_name[i] not in lines1_name:
        s='\t\t'.join([lines2_name[i],str('-----------'),lines2_email[i]])
        s+='\n'
    lines.append(s)
f3=open('merge.txt','w')
f3.writelines(lines)
f1.close()
f2.close()
f3.close()
print('合并完成')