import urllib.request
import time
import numpy as np
import os
from collections import deque
import matplotlib
import tkinter
import matplotlib.pyplot as plt
import matplotlib.axes as axe


length = 200
folder_name = "data/"
files_now=os.listdir(folder_name)
files_now.sort()
file_num = int(files_now[-1])+1 if files_now else 1
file_num_list = [file_num]
#device id, hardcoded do not modify
device_id = 2001
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
t_array=deque(range(-length+1, 1))
data1=deque([0]*length)
data2=deque([0]*length)
data3=deque([0]*length)
data4=deque([0]*length)
data_heater = deque([0]*length)
data_FSM = deque([0]*length)
filter1 = deque([0]*length)
filter2 = deque([0]*length)
filter3 = deque([0]*length)
filter4 = deque([0]*length)

fig = plt.figure()

ax1 = plt.subplot(3,2,1)
line1b, = plt.plot(t_array,data1,"b.--")
line1r, = plt.plot(t_array,filter1,"r.--")
ax1.set_xlim(0.4, 0.6)
ax1.set_ylim(-1, 0)
plt.title('Channel 1 Signal')
plt.ylabel('Current(uA)')
plt.xlabel('Time(s)')

ax2 = plt.subplot(3,2,2)
line2b, = plt.plot(t_array,data2,"b.--")
line2r, = plt.plot(t_array,filter2,"r.--")
ax2.set_xlim(0.4, 0.6)
ax2.set_ylim(-1, 0)
plt.title('Channel 2 Signal')
plt.ylabel('Current(uA)')
plt.xlabel('Time(s)')

ax3 = plt.subplot(3,2,3)
line3b, = plt.plot(t_array,data3,"b.--")
line3r, = plt.plot(t_array,filter3,"r.--")
ax3.set_xlim(0.4, 0.6)
ax3.set_ylim(-1, 0)
plt.title('Channel 3 Signal')
plt.ylabel('Current(uA)')
plt.xlabel('Time(s)')

ax4 = plt.subplot(3,2,4)
line4b, = plt.plot(t_array,data4,"b.--")
line4r, = plt.plot(t_array,filter4,"r.--")
ax4.set_xlim(0.4, 0.6)
ax4.set_ylim(-1, 0)
plt.title('Channel 4 Signal')
plt.ylabel('Current(uA)')
plt.xlabel('Time(s)')

ax5 = plt.subplot(3,2,5)
line5_b, = plt.plot(t_array,data_heater,"b.--")
line5_r, = plt.plot(t_array,data_FSM,"r.--")
ax5.set_xlim(0.4, 0.6)
ax5.set_ylim(-0.5, 2.5)
plt.title('Heater(blue,high=1,low=0)\nFSM(red,BAS=1,INC=2,REC=0)\nStatus')
plt.ylabel('Status')
plt.xlabel('Time(s)')

plt.tight_layout()

t0 = time.time()

x = 0

while True:
	list1 = urllib.request.urlopen(correct_url).read().split()
	for i in range(2,18):
		list1[i] = float(list1[i])
	
	list1[18] = int(list1[18])
	list1[19] = int(list1[19])
	list1[1] = int(list1[1])
	for i in range(2,10):
		list1[i] = list1[i]/1024.0*3.3/0.03 #uA
	teststr = str(device_id)+"\t"+str(list1[1]).zfill(6)+"\t"+time.strftime('%x\t%X')+"\t"+str(list1[2])+"\t"+str(list1[3])+"\t"+str(list1[10])+"\t"+str(list1[11])+"\t"+str(list1[4])+"\t"+str(list1[5])+"\t"+str(list1[12])+"\t"+str(list1[13])+"\t"+str(list1[6])+"\t"+str(list1[7])+"\t"+str(list1[14])+"\t"+str(list1[15])+"\t"+str(list1[8])+"\t"+str(list1[9])+"\t"+str(list1[16])+"\t"+str(list1[17])+"\t"+str(list1[18])+"\t"+str(list1[19])+"\n"
	t = time.time() - t0

	data1.append(list1[2])
	data2.append(list1[4])
	data3.append(list1[6])
	data4.append(list1[8])
	data_FSM.append(list1[19])
	data_heater.append(list1[18])
	filter1.append(list1[3])
	filter2.append(list1[5])
	filter3.append(list1[7])
	filter4.append(list1[9])
	t_array.append(t)

	data1.popleft()
	filter1.popleft()
	data2.popleft()
	filter2.popleft()
	data3.popleft()
	filter3.popleft()
	data4.popleft()
	filter4.popleft()
	t_array.popleft()
	data_FSM.popleft()
	data_heater.popleft()
	
	#plot results
	try:
		line1b.set_xdata(t_array)
		line1r.set_xdata(t_array)
		line2b.set_xdata(t_array)
		line2r.set_xdata(t_array)
		line3b.set_xdata(t_array)
		line3r.set_xdata(t_array)
		line4b.set_xdata(t_array)
		line4r.set_xdata(t_array)
		line5_r.set_xdata(t_array)
		line5_b.set_xdata(t_array)

		line1b.set_ydata(data1)
		line2b.set_ydata(data2)
		line3b.set_ydata(data3)
		line4b.set_ydata(data4)
		line1r.set_ydata(filter1)
		line2r.set_ydata(filter2)
		line3r.set_ydata(filter3)
		line4r.set_ydata(filter4)
		line5_r.set_ydata(data_FSM)
		line5_b.set_ydata(data_heater)

		if x>length:
			ax1.set_xlim(t_array[-length], t_array[-1])
			ax2.set_xlim(t_array[-length], t_array[-1])
			ax3.set_xlim(t_array[-length], t_array[-1])
			ax4.set_xlim(t_array[-length], t_array[-1])
			ax5.set_xlim(t_array[-length], t_array[-1])
			temp1 = list(data1)+list(filter1)
			temp2 = list(data2)+list(filter2)
			temp3 = list(data3)+list(filter3)
			temp4 = list(data4)+list(filter4)
		else:
			ax1.set_xlim(t_array[-x], t_array[-1])
			ax2.set_xlim(t_array[-x], t_array[-1])
			ax3.set_xlim(t_array[-x], t_array[-1])
			ax4.set_xlim(t_array[-x], t_array[-1])
			ax5.set_xlim(t_array[-x], t_array[-1])
			temp1 = list(data1)[-x:]+list(filter1)[-x:]
			temp2 = list(data2)[-x:]+list(filter2)[-x:]
			temp3 = list(data3)[-x:]+list(filter3)[-x:]
			temp4 = list(data4)[-x:]+list(filter4)[-x:]
		gap1 = 0.5*(max(temp1)-min(temp1))
		gap2 = 0.5*(max(temp2)-min(temp2))
		gap3 = 0.5*(max(temp3)-min(temp3))
		gap4 = 0.6*(max(temp4)-min(temp4))
		ax1.set_ylim(min(temp1)-gap1,max(temp1)+gap1)
		ax2.set_ylim(min(temp2)-gap2,max(temp2)+gap2)
		ax3.set_ylim(min(temp3)-gap3,max(temp3)+gap3)
		ax4.set_ylim(min(temp4)-gap4,max(temp4)+gap4)

		plt.pause(0.05)

	except Exception as e:
		file.close()
		del fig
		break

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
print("Exit Normally\nFile is saved as: "+file_name+"\n")
