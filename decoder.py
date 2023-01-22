from matplotlib.widgets import Button
import numpy.random as np
import matplotlib.pyplot as plt
from time import sleep


def look(a):
    print("XD")


msb_pos = 15
words_amount = 3
packages_amount = 8
byte_length = 8
sample_length = 16




dout = {'A':[],'B':[],'C':[],'D':[]}

tab4 = [12312, 65523, 11313, 22222, 65535, 0, 20100, 10030]
tab4 = [1, 0, 1, 0, 1, 0, 1, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255]
tab4 = list(10000*np.randn(100*8*words_amount))
for k in range(len(tab4)):
    tab4[k] = int(abs(tab4[k]))
#print(tab4)
tab3 = [0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0]
tab2 = list(np.randn(64*words_amount))
for i in range(len(tab2)):
    if tab2[i]>0.5:
        tab2[i] = int(0b1)
    else:
        tab2[i] = int(0b0)
tab = [1,1,1]

position = {'A':0, 'B':0, 'C':0, 'D':0}
voltage_coeff = 2.5

for n in range(int(len(tab2)/(len(dout)*sample_length))):
    for key in dout:
        dout[key].append(0)
    for i in range(int(len(tab2)*n/words_amount),int(len(tab2)*(n+1)/words_amount)):
        if i%2==0:
            if position['A']<15:
                dout['A'][n] += (tab2[i] << (msb_pos - position['A']))
                position['A'] += 1
            elif position['B']<15:
                dout['B'][n] += (tab2[i] << (msb_pos - position['B']))
                position['B'] += 1
        else:
            if position['C']<15:
                dout['C'][n] += (tab2[i] << (msb_pos - position['C']))
                position['C'] += 1
            elif position['D']<15:
                dout['D'][n] += (tab2[i] << (msb_pos - position['D']))
                position['D'] += 1
    for key in position:
        position[key]=0

for key in dout:
    for i in range(len(dout[key])):
        dout[key][i] = round(dout[key][i]/(2**sample_length)*voltage_coeff,3)


#print(dout)
position = {'A':0, 'B':0, 'C':0, 'D':0}
dout = {'A':[],'B':[],'C':[],'D':[]}
for i in range(int(len(tab4)/packages_amount)):
    for key in dout:
        dout[key].append(0)
    for k in range(packages_amount):
        if k<4:
            for n in range(byte_length):
                if n%2==0:
                    #dout['A'][i] += (tab4[k]&(0x01<<int(((4-k)*byte_length/2-n))))
                    dout['A'][i] += ((tab4[int(k+packages_amount*i)]>>(byte_length-n-1))&0x01)<<(int(byte_length*2-position['A']-1))
                    position['A'] += 1
                else:
                    dout['B'][i] += ((tab4[int(k+packages_amount*i)]>>(byte_length-n-1))&0x01)<<(int(byte_length*2-position['B']-1))
                    position['B'] += 1
        else:
            for n in range(byte_length):
                if n%2==0:
                    dout['C'][i] += ((tab4[int(k+packages_amount*i)]>>(byte_length-n-1))&0x01)<<(int(byte_length*2-position['C']-1))
                    position['C'] += 1
                else: 
                    dout['D'][i] += ((tab4[int(k+packages_amount*i)]>>(byte_length-n-1))&0x01)<<(int(byte_length*2-position['D']-1))
                    position['D'] += 1
    for key in position:
        position[key] = 0

for key in dout:
    for i in range(len(dout[key])):
        #print(i)
        dout[key][i] = round(dout[key][i]/(2**sample_length)*voltage_coeff,3)
print(dout)

x = [x for x in range(int(len(tab4)/packages_amount))]
y0 = [0*val for val in x]
y1 = [0*val for val in x]
y2 = [0*val for val in x]
y3 = [0*val for val in x]

plt.ion()
fig, axs = plt.subplots(4)
plt.subplots_adjust(left=0.3)
axcolor = 'lightgoldenrodyellow'
rax = plt.axes([0.05, 0.4, 0.15, 0.30],
               facecolor=axcolor)
axs[0].set_ylim(ymin=0, ymax=3)
axs[1].set_ylim(ymin=0, ymax=3)
axs[2].set_ylim(ymin=0, ymax=3)
axs[3].set_ylim(ymin=0, ymax=3)

radio = Button(rax, 'Add',color="yellow")

line1, = axs[0].plot(x, y0, 'b-')
line2, = axs[1].plot(x, y1, 'r-')
line3, = axs[2].plot(x, y2, 'g-')
line4, = axs[3].plot(x, y3, 'y-')

radio.on_clicked(look)

for i in range(len(dout['A'])):
    y0 = y0[1::]
    y0.append(dout['A'][i])
    y1 = y1[1::]
    y1.append(dout['B'][i])
    y2 = y2[1::]
    y2.append(dout['C'][i])
    y3 = y3[1::]
    y3.append(dout['D'][i])
    line1.set_ydata(y0)
    line2.set_ydata(y1)
    line3.set_ydata(y2)
    line4.set_ydata(y3)
    fig.canvas.draw()
    fig.canvas.flush_events()
        #sleep(0.1)

