import urllib.request
import time
import numpy as np
import matplotlib.pyplot as plt
import tkinter
import matplotlib.axes as axe
import os
from collections import deque

length = 200
folder_name = "data/"
files_now=os.listdir(folder_name)
files_now.sort()
file_num = int(files_now[-1])+1 if files_now else 1
file_num_list = [file_num]
#device id, hardcoded do not modify
device_id = 00
write_file_cycle = 10
write_files_multiplier = 20
first_line = "Dev_id\tIndex\tDate\tTime\tSensor1\tSensor1_Filter\tSensor1_LL\tSensor1_Diff\tSensor2\tSensor2_Filter\tSensor2_LL\tSensor2_Diff\tSensor3\tSensor3_Filter\tSensor3_LL\tSensor3_Diff\tSensor4\tSensor4_Filter\tSensor4_LL\tSensor4_Diff\tHeater_Status\tFSM_Status\n"

#check device id
print("check id now begin")
correct_url = "http://192.168.1.1"
for ip in range(1,256):
	correct_url = 'http://192.168.1.'+str(ip)
	print(correct_url+"\n")
	try:
		id_list = urllib.request.urlopen(correct_url).read().split()
		if int(id_list[0]) == device_id:
			break
		else:
			continue
	except:
		continue

file_name = folder_name+str(file_num).zfill(6)
file = open(file_name,"w")
file.seek(0,2)
file.write(first_line)

curr = 1
last = 0

#plt.axis([0, 21, 0, 0.1])
plt.ion()
t_array=deque()
data1=deque()
data2=deque()
data3=deque()
data4=deque()

filter1 = deque()
filter2 = deque()
filter3 = deque()
filter4 = deque()

t0 = time.time()

x = 0

while True:
	list1 = urllib.request.urlopen(correct_url).read().split()
	for i in range(2,18):
		list1[i] = float(list1[i])
	
	list1[18] = int(list1[18])
	list1[1] = int(list1[1])
	for i in range(2,8):
		list1[i] = list1[i]/1024.0*3.3/0.032934 #uA
	teststr = "PROTOTYPE"+"\t"+str(list1[1]).zfill(6)+"\t"+time.strftime('%x\t%X')+"\t"+str(list1[2])+"\t"+str(list1[3])+"\t"+str(list1[10])+"\t"+str(list1[11])+"\t"+str(list1[4])+"\t"+str(list1[5])+"\t"+str(list1[12])+"\t"+str(list1[13])+"\t"+str(list1[6])+"\t"+str(list1[7])+"\t"+str(list1[14])+"\t"+str(list1[15])+"\t"+str(list1[8])+"\t"+str(list1[9])+"\t"+str(list1[16])+"\t"+str(list1[17])+"\t"+str(list1[18])+"\t"+str(list1[19])
	t = time.time() - t0

	data1.append(list1[2])
	data2.append(list1[4])
	data3.append(list1[6])
	data4.append(list1[8])

	filter1.append(list1[3])
	filter2.append(list1[5])
	filter3.append(list1[7])
	filter4.append(list1[9])

	t_array.append(t)
	
	if x>=length:
		data1.popleft()
		filter1.popleft()
		data2.popleft()
		filter2.popleft()
		data3.popleft()
		filter3.popleft()
		data4.popleft()
		filter4.popleft()
		t_array.popleft()
	
	#plot results
	try:
		plt.figure(1)
		plt.subplot(311)
		plt.plot(t_array,data1,"b.--")
		plt.plot(t_array,filter1,"r.--")
		plt.title('Channel 1 Signal')
		plt.ylabel('Current(uA)')
		plt.xlabel('Time(s)')
		gap = 0.6*(max(data1)-min(data1))
		if x>length:
			plt.xlim(t_array[-length],t_array[-1])
			plt.ylim(min(data1)-gap,max(data1)+gap)
		
		plt.subplot(312)
		plt.plot(t_array,data2,"b.--")
		plt.plot(t_array,filter2,"r.--")
		plt.title('Channel 2 Signal')
		plt.ylabel('Current(uA)')
		plt.xlabel('Time(s)')
		gap = 0.6*(max(data2)-min(data2))
		if x>length:
			plt.xlim(t_array[-length],t_array[-1])
			plt.ylim(min(data2)-gap,max(data2)+gap)
		
		plt.subplot(313)
		plt.plot(t_array,data3,"b.--")
		plt.plot(t_array,filter3,"r.--")
		plt.title('Channel 3 Signal')
		plt.ylabel('Current(uA)')
		plt.xlabel('Time(s)')
		gap = 0.6*(max(data3)-min(data3))
		if x>length:
			plt.xlim(t_array[-length],t_array[-1])
			plt.ylim(min(data3)-gap,max(data3)+gap)
		
		# plt.subplot(224)
		# plt.plot(t_array,data4,"c.--")
		# plt.title('Channel 4 Signal')
		# plt.ylabel('Current(uA)')
		# plt.xlabel('Time(s)')
		# gap = 0.6*(max(data4)-min(data4))
		# if x>length:
		# 	plt.xlim( t_array[-length],t_array[-1])
		# 	plt.ylim(min(data4)-gap,max(data4)+gap)
		# # plt.plot(t_array,data3,"b.--")
		# # plt.title('Channel 3 Signal')
		# # plt.ylabel('Current(uA)')
		# # plt.xlabel('Time(s)')
		plt.tight_layout()
		plt.pause(0.05)

	except Exception as e:
		file.close()
		break

	teststr = teststr + "\n"
	
	if curr != last:
		file.write(teststr)
	#print(len(t_array))
	#print(last)
	last = curr
	curr = int(list1[1])
	x +=1
	
	#save the file every n cycles
	if x % write_file_cycle == 0:
		file.close()
		if x % (write_files_multiplier * write_file_cycle) == 0:
			file_num += 1
			file_num_list.append(file_num)
			file_name = folder_name+str(file_num).zfill(6)
			file = open(file_name,"w")
			file.seek(0,2)
			file.write(first_line)
		else:
			file = open(file_name,"a")
		file.seek(0,2)

	time.sleep(1)

#merge file here
file_name = folder_name+str(file_num_list[0]).zfill(6)
for i in range(1, len(file_num_list)):
	file = open(file_name, "a")
	with open(folder_name+str(file_num_list[i]).zfill(6)) as subfile:
		subdata = subfile.read()
	subdata = subdata[len(first_line):]
	file.write(subdata)
	file.close()
	os.remove(folder_name+str(file_num_list[i]).zfill(6))
print("Exit Normally")





