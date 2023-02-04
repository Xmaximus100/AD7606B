from serial import Serial
import struct
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, Slider, RadioButtons
from enum import Enum
import numpy as np

class OsciloscopeInterface:
    def __init__(self):
        self.ser = Serial("COM3", 115200)
        self.time_on_div = 10
        self.voltage_coeff = 2.5
        self.begin = False
        self.range = 1

    def Start(self, x):
        self.ser.write(b'X')
        self.ser.write(ord(str(self.range)).to_bytes(1,"big"))
        self.ser.write(ord(hex(self.time_on_div)[2])).to_bytes(1,"big")
        self.begin = True
        print("START")

        tab_assist = []
        channel = [[],[],[],[]] 
        line = [0,0,0,0]
        iter = 0
        channel_pointer = {'A':True, 'B':False, 'C':False, 'D':False}

        if AD7606B.begin:
            bytesRead = AD7606B.ser.read(100*8)
            print(bytesRead)
            tab_assist.extend(bytesRead)
            print(tab_assist)

        # tab_assist = [0x87, 0x00]*100

        for i in range(100*8):
            if i%2==0:
                if channel_pointer['A']:
                    channel[0].append(tab_assist[i]<<8)
                elif channel_pointer['B']:
                    channel[1].append(tab_assist[i]<<8)
                elif channel_pointer['C']:
                    channel[2].append(tab_assist[i]<<8)
                elif channel_pointer['D']:
                    channel[3].append(tab_assist[i]<<8)
            else:
                if channel_pointer['A']:
                    channel[0][iter] += tab_assist[i]
                    channel_pointer['A'] = False
                    channel_pointer['B'] = True
                elif channel_pointer['B']:
                    channel[1][iter] += tab_assist[i]
                    channel_pointer['B'] = False
                    channel_pointer['C'] = True
                elif channel_pointer['C']:
                    channel[2][iter] += tab_assist[i]
                    channel_pointer['C'] = False
                    channel_pointer['D'] = True
                elif channel_pointer['D']:
                    channel[3][iter] += tab_assist[i]
                    channel_pointer['D'] = False
                    channel_pointer['A'] = True
                    iter += 1
            
        print(f'LEN: {len(channel[0])}\tCONTENT: {channel}')
        x = np.linspace(0, self.time_on_div*len(channel[0]), len(channel[0]))
        print(x)
        y = []
        for i in range(4):
            axs[i].cla()
            axs[i].set_xlabel("t[ms]")
            axs[i].set_ylabel("U[V]")
            axs[i].grid(which="both")
            axs[i].grid(which="major",alpha=self.time_on_div/10)
            y.append([0 for val in x])
            line[i], = axs[i].plot(x,y[i], lw=2)

        self.SetLimitsX()
        self.SetLimitsY()
        
        for k in range(len(channel)):
            for i in range(len(channel[k])):
                print(f'Before unpack:{channel[k][i]}')
                channel[k][i] = struct.unpack('h', struct.pack('H',int(channel[k][i])))[0]
                print(f'After unpack: {channel[k][i]}')
                channel[k][i] = round(channel[k][i]/(2**(16-1)-1)*2.5,6)

        iter = 0
        self.ser.reset_input_buffer
        for ax in axs:
            #ax.cla()
            line[iter].set_ydata(channel[iter])
            iter += 1
            plt.draw()

    def TimeOnDiv(self,val):
        self.time_on_div = val
        print(val)
        print(f'{val}ms/div')

    def RangeSet(self,label):
        print(label)
        if label=="RANGE 2.5V":
            self.range = 1
            self.voltage_coeff = 2.5
        elif label=="RANGE 5V":
            self.range = 2
            self.voltage_coeff = 5
        else:
            self.range = 3
            self.voltage_coeff = 10

    def SetLimitsY(self):
        for i in axs:
            i.set_ylim(ymin=-self.voltage_coeff*1.2, ymax=self.voltage_coeff*1.2)

    def SetLimitsX(self):
        for i in axs:
            i.set_xlim(xmin=-0.2*self.time_on_div, xmax=self.time_on_div*100.2)

AD7606B = OsciloscopeInterface()


fig, axs = plt.subplots(4)
fig.subplots_adjust(left=0.3)
for i in axs:
    i.set_xlabel("t[ms]")
    i.set_ylabel("U[V]")
    i.grid(which="both")
    i.grid(which="major",alpha=AD7606B.time_on_div/10)
fig.set_size_inches(10,7)

axcolor = 'lightgoldenrodyellow'
button1 = plt.axes([0.05, 0.73, 0.15, 0.15],
            facecolor=axcolor)
slider1 = plt.axes([0.05, 0.43, 0.15, 0.15],
                    facecolor=axcolor)
radiobutton1 = plt.axes([0.05, 0.13, 0.15, 0.15],
                    facecolor=axcolor)

START = Button(button1, 'START',color="yellow")
TIME = Slider(slider1, 'STOP',2, 10, valinit=10,valstep=1,color="red")
RANGE = RadioButtons(radiobutton1,labels=["RANGE 2.5V","RANGE 5V", "RANGE 10V"])

START.on_clicked(AD7606B.Start)
TIME.on_changed(AD7606B.TimeOnDiv)
RANGE.on_clicked(AD7606B.RangeSet)

plt.show()
