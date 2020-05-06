import serial

s = serial.Serial('COM9')
f = open('output.txt','w')
print("Dumping SRAM contents")
s.write(b'a')
for i in range(0,0x1FFF):
    val = s.read().hex()
    f.write(val)
    f.write("\n")
