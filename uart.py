from serial import Serial
from time import sleep

ser = Serial("COM3", 115200)

while True: 
    x = ser.read(4)
    print(x)
    if KeyboardInterrupt:
        ser.write()


