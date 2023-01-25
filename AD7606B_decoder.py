from serial import Serial
import struct
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, Slider, RadioButtons
from enum import Enum
from numpy import linspace

class OsciloscopeInterface:
    def __init__(self):
        self.ser = Serial("COM13", 115200)
        self.start = False
        self.tab4 = []
        self.tab_assist = []
        self.position = {'A':0, 'B':0, 'C':0, 'D':0}
        self.dout = {'A':0,'B':0,'C':0,'D':0}
        self.time_on_div = 5
        self.voltage_coeff = 2.5
        self.range = 0
        self.x = linspace(0, self.time_on_div*5, 100)
        self.y0 = [0*val for val in self.x]
        self.y1 = [0*val for val in self.x]
        self.y2 = [0*val for val in self.x]
        self.y3 = [0*val for val in self.x]
        self.dec : int

        self.live = True
        plt.ion()
        self.fig, self.axs = plt.subplots(4)
        for i in self.axs:
            i.set_xlabel("t[ms]")
            i.set_ylabel("U[V]")
            i.grid(which="both")
            i.grid(which="major",alpha=self.time_on_div/10)
        self.fig.set_size_inches(10,7)
        plt.subplots_adjust(left=0.3)
        axcolor = 'lightgoldenrodyellow'
        self.button1 = plt.axes([0.05, 0.73, 0.15, 0.15],
                    facecolor=axcolor)
        self.slider1 = plt.axes([0.05, 0.43, 0.15, 0.15],
                    facecolor=axcolor)
        self.radiobutton1 = plt.axes([0.05, 0.13, 0.15, 0.15],
                    facecolor=axcolor)

        self.SetLimitsY()
        self.Plot()

        self.fig.canvas.draw()
        self.fig.canvas.flush_events()

        self.START = Button(self.button1, 'START',color="yellow")
        self.TIME = Slider(self.slider1, 'STOP',2, 10, valinit=10,valstep=1,color="red")
        self.RANGE = RadioButtons(self.radiobutton1,labels=["RANGE 2.5V","RANGE 5V", "RANGE 10V"])

        self.START.on_clicked(self.Start)
        self.TIME.on_changed(self.TimeOnDiv)
        self.RANGE.on_clicked(self.RangeSet)

    def Start(self,x):
        #self.ser.write(b'X')
        #self.ser.write(self.range)
        #self.ser.write(self.time_on_div)
        print("START")

    def TimeOnDiv(self,val):
        self.time_on_div = val
        self.x = linspace(0, self.time_on_div*5, 100)
        self.SetGrid()
        self.SetXData()
        self.SetLimitsX()
        print(val)
        print(f'{val}ms/div')

    def RangeSet(self,label):
        print(label)
        if label=="RANGE 2.5V":
            self.range = 1
            self.voltage_coeff = 2.5
            self.SetLimitsY()
        elif label=="RANGE 5V":
            self.range = 2
            self.voltage_coeff = 5
            self.SetLimitsY()
        else:
            self.range = 3
            self.voltage_coeff = 10
            self.SetLimitsY()
        #self.ser.write('1')

    def SetGrid(self):
        for i in self.axs:
            i.grid(which="major",alpha=self.time_on_div/10)

    def SetLimitsY(self):
        for i in self.axs:
            i.set_ylim(ymin=-self.voltage_coeff*1.2, ymax=self.voltage_coeff*1.2)

    def SetLimitsX(self):
        for i in self.axs:
            i.set_xlim(xmin=-0.2*self.time_on_div, xmax=self.time_on_div*5.2)

    def SetXData(self):
        self.line1.set_xdata(self.x)
        self.line2.set_xdata(self.x)
        self.line3.set_xdata(self.x)
        self.line4.set_xdata(self.x)

    def Plot(self):
        self.line1, = self.axs[0].plot(self.x, self.y0, 'b-')
        self.line2, = self.axs[1].plot(self.x, self.y1, 'r-')
        self.line3, = self.axs[2].plot(self.x, self.y2, 'g-')
        self.line4, = self.axs[3].plot(self.x, self.y3, 'y-')
    #self.tab4 = [1, 0, 1, 0, 1, 0, 1, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255]    #trial data 
    msb_pos = 15
    words_amount = 3
    packages_amount = 8
    byte_length = 8
    sample_length = 16

AD7606B = OsciloscopeInterface()
print("dzialam")

while True: 
    if AD7606B.live:
        AD7606B.fig.canvas.draw()
    AD7606B.fig.canvas.flush_events()
    #x = 0
    #if ser.in_waiting:
    x = AD7606B.ser.read(1)
    #print(f'oppening data {x}')
    if(x == b'W' and not start):
        start = True
    # elif(start):
    #     x = bytearray(ser.read(8))
    #     print(x)
    #     print("DANA")

    #     ser.reset_input_buffer()
    #     ser.reset_output_buffer()
    elif(x and start):
        print("DANA")
        dec = int.from_bytes(x, "big",signed=False)
        print(dec)
        AD7606B.tab_assist.append(dec)
        if(len(AD7606B.tab_assist)==8):   
            tab4 = [x for x in AD7606B.tab_assist]
            for key in AD7606B.dout:
                AD7606B.dout[key] = 0
            for k in range(AD7606B.packages_amount):
                if k<4:
                    for n in range(AD7606B.byte_length):
                        if n%2==0:
                            AD7606B.dout['A'] += ((tab4[k]>>(AD7606B.byte_length-n-1))&0x01)<<(int(AD7606B.byte_length*2-AD7606B.position['A']-1))
                            AD7606B.position['A'] += 1
                        else:
                            AD7606B.dout['B'] += ((tab4[k]>>(AD7606B.byte_length-n-1))&0x01)<<int(AD7606B.byte_length*2-AD7606B.position['B']-1)
                            AD7606B.position['B'] += 1
                else:
                    for n in range(AD7606B.byte_length):
                        if n%2==0:
                            AD7606B.dout['C'] += ((tab4[k]>>(AD7606B.byte_length-n-1))&0x01)<<int(AD7606B.byte_length*2-AD7606B.position['C']-1)
                            AD7606B.position['C'] += 1
                        else: 
                            AD7606B.dout['D'] += ((tab4[k]>>(AD7606B.byte_length-n-1))&0x01)<<int(AD7606B.byte_length*2-AD7606B.position['D']-1)
                            AD7606B.position['D'] += 1
            for key in AD7606B.position:
                AD7606B.position[key] = 0
            AD7606B.tab_assist.clear()
            print(f'TAB ASSIST: {AD7606B.tab_assist}\tTAB4: {tab4}')
            
            for key in AD7606B.dout:
                print(f'before unpack:{AD7606B.dout[key]}')
                AD7606B.dout[key] = struct.unpack('h', struct.pack('H',int(AD7606B.dout[key])))[0]
                print(AD7606B.dout[key])
                AD7606B.dout[key] = round(AD7606B.dout[key]/(2**(AD7606B.sample_length))*AD7606B.voltage_coeff,6)

            y0 = y0[1::]
            y0.append(AD7606B.dout['A'])
            y1 = y1[1::]
            y1.append(AD7606B.dout['B'])
            y2 = y2[1::]
            y2.append(AD7606B.dout['C'])
            y3 = y3[1::]
            y3.append(AD7606B.dout['D'])
            if AD7606B.live:
                AD7606B.line1.set_ydata(y0)
                AD7606B.line2.set_ydata(y1)
                AD7606B.line3.set_ydata(y2)
                AD7606B.line4.set_ydata(y3)
            print(y0,y1,y2,y3)
            start = False
            iter += 1
            #ser.flush()
            AD7606B.ser.reset_input_buffer()
            AD7606B.ser.reset_output_buffer()