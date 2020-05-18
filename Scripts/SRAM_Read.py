import serial
import os
import sys


dirpath = os.path.dirname(os.path.realpath(__file__))
sys.path.append(dirpath)

filepath1 = os.path.join(dirpath,'SRAM_Data10.txt')


file1 = open(filepath1,"w")

s = serial.Serial('COM13')
KeyByte=b''
J=0
i=0
SRAM_Address=0x0000
#send "read all command to state machine"
s.write(b'a')
while(1):

    res = s.read()
    KeyByte+=res
    J=J+1
    if( J==5):
        print(KeyByte)
        file1.write(str(KeyByte)+"\t"+str(hex(SRAM_Address))+"\n")
        file1.flush()
        KeyByte=b''
        i+=1
        J=0
        SRAM_Address+=1

    if(i==8192):
         break

print(KeyByte)
file1.close()