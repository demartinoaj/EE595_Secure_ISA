import serial

s = serial.Serial('COM9')
name="Andrew_SRAM_Dump_"

iter=0;
n=int(input("Enter the number of iterations to preform: "))
while iter<n:
	print("Dumping SRAM contents to "+name+str(iter)+".txt")
	f = open(name+str(iter)+".txt",'w')
	s.write(b'a')
	for i in range(0,0x1FFF):
	    val = s.read().hex()
	    f.write(val)
	    f.write("\n")
	iter+=1;
	print("RESET SRAM")
	input("Press Enter to continue")