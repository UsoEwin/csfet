import urllib.request
import time
import numpy as np
import matplotlib.pyplot as plt
import tkinter
import matplotlib.axes as axe
import os
from collections import deque

length = 1000
alpha = 1
files_now=os.listdir("data/")
file_num = len(files_now) + 1
#device id, hardcoded do not modify
device_id = 3


#check device id
print("check id now begin")
correct_url = "http://192.168.1.1"
for x in range(1,256):
	correct_url = 'http://192.168.1.'+str(x)
	try:
		id_list = urllib.request.urlopen(correct_url).read().split()
		if int(id_list[0]) == device_id:
			break
		else:
			continue
	except:
		continue
print(correct_url)

namestring = "data/"+str(file_num).zfill(6)
file = open(namestring,"w")
t0 = time.time()
file.seek(0,2)
counter = 1
file.write("Dev_id\t Date\t Time\t Sensor1\t Sensor2\t Sensor3\t Sensor4\n")
curr = 1
last = 0

#plt.axis([0, 21, 0, 0.1])
plt.ion()
t_array=deque()
data1=deque()
# data2=deque()
# data3=deque()
# data4=deque()

list0 = urllib.request.urlopen(correct_url).read().split()
for i in range(1,2):
	list0[i] = int(list0[i])/1024.0*3.3/0.30000

t_array.append(0)
data1.append(list0[1])
# data2.append(list0[2])
# data3.append(list0[3])
# data4.append(list0[4])
x = 1


while True:
	
	t = time.time() - t0
	#t_array.append(t)
	#save the file every 5 cycles
	if counter == 5:
		counter = 1
		file.close();
		file = open(namestring,"a")
		file.seek(0,2)
	counter = counter + 1
	teststr = time.strftime('%x\t%X')
	
	list1 = urllib.request.urlopen(correct_url).read().split()

	for i in range(1,2):
		list1[i] = int(list1[i])/1024.0*3.3/0.32934 #uA
		teststr = teststr +"\t"+ str(list1[i])
	#plotting
	list0[1] = alpha*list1[1]+(1-alpha)*list0[1]
	# list0[2] = alpha*list1[2]+(1-alpha)*list0[2]
	# list0[3] = alpha*list1[3]+(1-alpha)*list0[3]
	# list0[4] = alpha*list1[4]+(1-alpha)*list0[4]
	data1.append(list0[1])
	# data2.append(list0[2])
	# data3.append(list0[3])
	# data4.append(list0[4])
	t_array.append(t)
	if x>=length:
		data1.popleft()
		# data2.popleft()
		# data3.popleft()
		# data4.popleft()
		t_array.popleft()

	plt.figure(1)
	# plt.subplot(221)
	plt.plot(t_array,data1,"b.--")
	plt.title('Channel 1 Signal')
	plt.ylabel('Current(uA)')
	plt.xlabel('Time(s)')
	gap = 0.6*(max(data1)-min(data1))
	if x>length:
		plt.xlim( t_array[-length],t_array[-1])
		plt.ylim(min(data1)-gap,max(data1)+gap)
	
	# plt.subplot(222)
	# plt.plot(t_array,data2,"r.--")
	# plt.title('Channel 2 Signal')
	# plt.ylabel('Current(uA)')
	# plt.xlabel('Time(s)')
	# gap = 0.6*(max(data2)-min(data2))
	# if x>length:
	# 	plt.xlim( t_array[-length],t_array[-1])
	# 	plt.ylim(min(data2)-gap,max(data2)+gap)
	
	# plt.subplot(223)
	# plt.plot(t_array,data3,"k.--")
	# plt.title('Channel 3 Signal')
	# plt.ylabel('Current(uA)')
	# plt.xlabel('Time(s)')
	# gap = 0.6*(max(data3)-min(data3))
	# if x>length:
	# 	plt.xlim( t_array[-length],t_array[-1])
	# 	plt.ylim(min(data3)-gap,max(data3)+gap)
	
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

	teststr = teststr + "\n"
	
	if curr != last:
		file.write(teststr)
	#print(len(t_array))
	#print(last)
	last = curr
	curr = int(list1[0])
	x +=1
	time.sleep(2)

file.close()





