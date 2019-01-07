import serial
import time
import random

def convertIntToNumStr(intValue):
    if intValue < 10:
        strOut = "00%i"%intValue
    elif intValue < 100:
        strOut = "0%i"%intValue
    else:
        strOut = "%i"%intValue
    return strOut

def setRGB(serStrm,redValue=0,greenValue=0,blueValue=0):
    redStr = convertIntToNumStr(redValue)
    grnStr = convertIntToNumStr(greenValue)
    bluStr = convertIntToNumStr(blueValue)
    
    command = "*R%sG%sB%s;"%(redStr,grnStr,bluStr)
    serStrm.write(command)
    
def function1(serStrm):
    # Walk Red
    for i in range(0,256):
        setRGB(serStrm,i,0,0)
        time.sleep(0.01)

    # Fade to Green
    for i in range(0,256):
        setRGB(serStrm,255-i,i,0)
        time.sleep(0.01)
    
    # Fade to Blue    
    for i in range(0,256):
        setRGB(serStrm,0,255-i,i)
        time.sleep(0.01)

    # Fade to Off
    for i in range(0,256):
        setRGB(serStrm,0,0,255-i)
        time.sleep(0.01)

    # Walk Green
    for i in range(0,256):
        setRGB(serStrm,0,i,0)
        time.sleep(0.01)
 
    # Fade from Green
    for i in range(0,256):
        setRGB(serStrm,i,255-i,i)
        time.sleep(0.01)

    # Fade from Purple
    for i in range(0,256):
        setRGB(serStrm,255-i,i,255)
        time.sleep(0.01)

    # Fade from Teal
    for i in range(0,256):
        setRGB(serStrm,i,255,255-i)
        time.sleep(0.01)

    # Fade from Orange to White
    for i in range(0,256):
        setRGB(serStrm,255,255,i)
        time.sleep(0.01)
    
    # Fade from White to off
    for i in range(0,256):
        setRGB(serStrm,255-i,255-i,255-i)
        time.sleep(0.01)

def function2(serStrm):
    x = random.randint(1,255)
    y = random.randint(1,255)
    z = random.randint(1,255)
    for i in range(0,x):
        setRGB(serStrm, i, 0, 0)
        time.sleep(0.01)
        
    for i in range(0,y):
        setRGB(serStrm, x,i,0)
        time.sleep(0.01)
        
    for i in range(0,z):
        setRGB(serStrm, x,y,i)
        time.sleep(0.01)
        
    for i in range(0,x):
        setRGB(serStrm, x-i,y,z)
        time.sleep(0.01)
        
    for i in range(0,y):
        setRGB(serStrm, 0,y-i,z)
        time.sleep(0.01)
        
    for i in range(0,z):
        setRGB(serStrm, 0,0,z-i)
        time.sleep(0.01)
        
def function3(serStrm):
    for i in range(0,256):
        setRGB(serStrm, i,0,0)
        time.sleep(0.005)
    
    time.sleep(0.5)    
    for i in range(0,256):
        setRGB(serStrm, 255,i,0)
        time.sleep(0.005)
    
    time.sleep(0.5)    
    for i in range(0,256):
        setRGB(serStrm, 255,255,i)
        time.sleep(0.005)
        
    time.sleep(5)
    
    for i in range(0,256):
        setRGB(serStrm, 255,255-i,255)
        time.sleep(0.005)
    
    time.sleep(0.5)    
    for i in range(0,256):
        setRGB(serStrm, 255-i,0,255)
        time.sleep(0.005)
    
    time.sleep(0.5)    
    for i in range(0,256):
        setRGB(serStrm, 0,0,255-i)
        time.sleep(0.005)

    time.sleep(5)
    

def flashColor(serStrm,rgbbrghtLvl=[255,255,255],delayInS=1):
        setRGB(serStrm,0,0,0)
        time.sleep(delayInS)
        setRGB(serStrm,rgbbrghtLvl[0],rgbbrghtLvl[1],rgbbrghtLvl[2])
        time.sleep(delayInS)
        setRGB(serStrm,0,0,0)
        
def function4(serStrm):
    delay = 0.05
    for i in range(0,256):
        flashColor(serStrm,[i,0,0],delay)
        flashColor(serStrm,[i,i,0],delay)
        flashColor(serStrm,[0,i,i],delay)
        flashColor(serStrm,[0,0,i],delay)
def function5(serStrm):
    delay = 0.05
    flashColor(serStrm,[random.randint(0,255),random.randint(0,255),random.randint(0,255)],delay)
    
def function6(serStrm):
    for i in range(1,101,1):
        flashColor(serStrm,[random.randint(0,255),random.randint(0,255),random.randint(0,255)],1.00/float(i))
    for i in range(100,0,-1):
        flashColor(serStrm,[random.randint(0,255),random.randint(0,255),random.randint(0,255)],1.00/float(i))
    for i in range(1,101,1):
        flashColor(serStrm,[random.randint(0,255),random.randint(0,255),random.randint(0,255)],1.00/float(i))
    for i in range(100,0,-1):
        flashColor(serStrm,[random.randint(0,255),random.randint(0,255),random.randint(0,255)],1.00/float(i))
#########################################################

#serPort = 'COM4'
serPort = '/dev/ttyUSB0'

ser = serial.Serial(serPort,115200)  # open serial port
#print(ser.name)         # check which port was really used

x = ""
while (x.find(">") == -1):
    x += ser.read()
    #print x
print x
while(1):
    time.sleep(5)
    function6(ser)
    for i in range(0,100):
        function5(ser)
    function4(ser)
    function3(ser)
    function1(ser)
    for j in range(0,15):
        function2(ser)

ser.close()             # close port
