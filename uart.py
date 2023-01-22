from serial import Serial
from time import sleep

ser = Serial("COM13", 115200)
helo = "hello"
print("KURWAAAAAAAAAAAAAAAA_ZAP2")
while True: 
    x = ser.read(4)
    print(x)
