from serial import Serial
from time import sleep

ser = Serial("COM13", 115200)
helo = "h"
print("KURWAAAAAAAAAAAAAAAA_ZAP2")
while True: 
    x = ser.read()
    print(x)
    ser.write(unicode("h"))