import xml.etree.ElementTree as rxml
from pprint import pprint
import matplotlib.pyplot as plt

tree = rxml.parse('udp.xml')
root = tree.getroot()

result=[]
thput=[]
delay=[]
for r in root[0]:
    dum=[]
    # print(r.tag, r.attrib)
    dum.append(int(r.attrib['timeFirstRxPacket'][:-4]))
    dum.append(int(r.attrib['timeLastRxPacket'][:-4]))
    dum.append(int(r.attrib['delaySum'][:-4]))
    dum.append(int(r.attrib['rxBytes']))
    dum.append(int(r.attrib['rxPackets']))
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
plt.title('Delay in seconds')
plt.savefig('delay_only_udp')
plt.close()

plt.figure()
plt.plot(psize,thput)
plt.xlabel('packet size in bytes')
plt.ylabel('Throughput')
plt.title('Throughput only udp')
plt.savefig('tput_only_udp')
plt.close()