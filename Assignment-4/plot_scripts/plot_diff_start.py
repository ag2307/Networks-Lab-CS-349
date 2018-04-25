import xml.etree.ElementTree as rxml
from pprint import pprint
import matplotlib.pyplot as plt

tree = rxml.parse('diff_start.xml')
root = tree.getroot()

result=[]
thput=[]
delay=[]
for i in range(0,10):
    dum=[]
    # print(r.tag, r.attrib)
    dum.append(int(root[0][i].attrib['timeFirstRxPacket'][:-4]))
    dum.append(int(root[0][i].attrib['timeLastRxPacket'][:-4]))
    dum.append(int(root[0][i].attrib['delaySum'][:-4]))
    dum.append(int(root[0][i].attrib['rxBytes']))
    dum.append(int(root[0][i].attrib['rxPackets']))
    thput.append((dum[3]*8000)/(dum[1]-dum[0]))
    delay.append(dum[2]/(dum[4]*1000000000))
    result.append(dum)

psize=[]
pprint(thput)
pprint(delay)
psize=[2,4,8,16,32,64,128,256,512,1024]
plt.figure()
plt.plot(psize,delay)
plt.xlabel('packet size in bytes')
plt.ylabel('Delay in seconds per packet')
plt.title('Delay in seconds diff udp')
plt.savefig('delay_diff_udp')
plt.close()

plt.figure()
plt.plot(psize,thput)
plt.xlabel('packet size in bytes')
plt.ylabel('Throughput')
plt.title('Throughput diff udp')
plt.savefig('tput_diff_udp')
plt.close()


result=[]
thput=[]
delay=[]
for i in range(10,20):
    dum=[]
    # print(r.tag, r.attrib)
    dum.append(int(root[0][i].attrib['timeFirstRxPacket'][:-4]))
    dum.append(int(root[0][i].attrib['timeLastRxPacket'][:-4]))
    dum.append(int(root[0][i].attrib['delaySum'][:-4]))
    dum.append(int(root[0][i].attrib['rxBytes']))
    dum.append(int(root[0][i].attrib['rxPackets']))
    thput.append((dum[3]*8000)/(dum[1]-dum[0]))
    delay.append(dum[2]/(dum[4]*1000000000))
    result.append(dum)

psize=[]
pprint(thput)
pprint(delay)
psize=[2,4,8,16,32,64,128,256,512,1024]
plt.figure()
plt.plot(psize,delay)
plt.xlabel('packet size in bytes')
plt.ylabel('Delay in seconds per packet')
plt.title('Delay in seconds diff tcp')
plt.savefig('delay_diff_tcp')
plt.close()

plt.figure()
plt.plot(psize,thput)
plt.xlabel('packet size in bytes')
plt.ylabel('Throughput')
plt.title('Throughput diff tcp')
plt.savefig('tput_diff_tcp')
plt.close()