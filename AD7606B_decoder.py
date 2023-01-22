from serial import Serial
import matplotlib.pyplot as plt
from matplotlib.widgets import Button

ser = Serial("COM3", 115200)
start = False

def Reset(x):
    ser.write('A')
    print("RESET")

def Stop(x):
    ser.write(0x20)
    print("STOP")

def SpeedOne(x):
    ser.write('1')
    print("SPEED ONE")

#tab4 = [1, 0, 1, 0, 1, 0, 1, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255]    #trial data 
tab4 = []
x = [x for x in range(100)]
y0 = [0*val for val in x]
y1 = [0*val for val in x]
y2 = [0*val for val in x]
y3 = [0*val for val in x]
position = {'A':0, 'B':0, 'C':0, 'D':0}
dout = {'A':[],'B':[],'C':[],'D':[]}
iter = 0

msb_pos = 15
words_amount = 3
packages_amount = 8
byte_length = 8
sample_length = 16
voltage_coeff = 2.5

plt.ion()
fig, axs = plt.subplots(4)
plt.subplots_adjust(left=0.3)
axcolor = 'lightgoldenrodyellow'
button1 = plt.axes([0.05, 0.73, 0.15, 0.15],
               facecolor=axcolor)
button2 = plt.axes([0.05, 0.43, 0.15, 0.15],
               facecolor=axcolor)
button3 = plt.axes([0.05, 0.13, 0.15, 0.15],
               facecolor=axcolor)

axs[0].set_ylim(ymin=0, ymax=3)
axs[1].set_ylim(ymin=0, ymax=3)
axs[2].set_ylim(ymin=0, ymax=3)
axs[3].set_ylim(ymin=0, ymax=3)

RESET = Button(button1, 'RESET',color="yellow")
STOP = Button(button2, 'STOP',color="red")
SPEED = Button(button3, 'SPEED',color="blue")

line1, = axs[0].plot(x, y0, 'b-')
line2, = axs[1].plot(x, y1, 'r-')
line3, = axs[2].plot(x, y2, 'g-')
line4, = axs[3].plot(x, y3, 'y-')

RESET.on_clicked(Reset)
STOP.on_clicked(Stop)
SPEED.on_clicked(SpeedOne)

while True: 
    fig.canvas.draw()
    fig.canvas.flush_events()

    x = ser.read(1)
    if(x == 'w'):
        start = True
    if(x and start):
        tab4.append(x)
        if(len(tab4)==8):   
            for key in dout:
                dout[key].append(0)
            for k in range(packages_amount):
                if k<4:
                    for n in range(byte_length):
                        if n%2==0:
                            dout['A'][iter] += ((tab4[k+packages_amount]>>(byte_length-n-1))&0x01)<<(byte_length*2-position['A']-1)
                            position['A'] += 1
                        else:
                            dout['B'][iter] += ((tab4[k+packages_amount]>>(byte_length-n-1))&0x01)<<(byte_length*2-position['B']-1)
                            position['B'] += 1
                else:
                    for n in range(byte_length):
                        if n%2==0:
                            print(position['C'])
                            dout['C'][iter] += ((tab4[k+packages_amount]>>(byte_length-n-1))&0x01)<<(byte_length*2-position['C']-1)
                            position['C'] += 1
                        else: 
                            dout['D'][iter] += ((tab4[k+packages_amount]>>(byte_length-n-1))&0x01)<<(byte_length*2-position['D']-1)
                            position['D'] += 1
            for key in position:
                position[key] = 0
            tab4.clear()
            iter += 1
            for key in dout:
                for i in range(len(dout[key])):
                    print(i)
                    dout[key][i] = round(dout[key][i]/(2**sample_length)*voltage_coeff,3)

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
            start = False
            print(dout)