import numpy
from matplotlib import pyplot as plt
import re
file=open("second.txt")
content=file.read()
line=content.split("\n")
for i in range (2,1002):
	print(float(re.search('time=(.*) ms', line[i]).group(1)))
# temp=map(float,numpy.linspace(0,20,201))
# plt.hist(time,bins=temp)
# plt.show()
