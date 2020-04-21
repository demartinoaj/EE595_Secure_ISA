import serial

s = serial.Serial('COM13')
while(1):
    res = s.read()
    print((res.decode('UTF-8')))
