from serial import Serial
from time import sleep

ser = Serial("COM13", 115200)

while True: 
    x = ser.read().decode()
    print(x)
