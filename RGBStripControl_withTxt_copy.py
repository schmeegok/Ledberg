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
    
def extractDelay(fileCmd="D=0.000,R=0,G=0,B=0;"):
    # Extract the delay parameter
    delayParm = float(fileCmd[fileCmd.find("D=")+2:fileCmd.find(',')])
    fileCmd = fileCmd[fileCmd.find(',')+1:]
    return (fileCmd, delayParm)

def extractRGB(fileCmd="R=0,G=33,B=240;"):
    # Extract the RGB value in a way order shouldn't matter
    v1 = fileCmd[:fileCmd.find(',')]
    fileCmd = fileCmd[fileCmd.find(',')+1:]
    v2 = fileCmd[:fileCmd.find(',')]
    fileCmd = fileCmd[fileCmd.find(',')+1:]
    v3 = fileCmd[:fileCmd.find(';')]
    fileCmd = fileCmd[fileCmd.find(';')+1:]
    
    # First check that all colors are specified, exit if not and turn them all off
    if ( v1.find('R=') == -1 and v1.find('G=') == -1 and v1.find('B=') == -1 or
         v2.find('R=') == -1 and v2.find('G=') == -1 and v2.find('B=') == -1 or
         v3.find('R=') == -1 and v3.find('G=') == -1 and v3.find('B=') == -1):
        print "Malformed command! Result is RGB = 0,0,0."
        return (0,0,0)
    
    # If all are specified, then its worth turning the crank
    if v1.find('R=') != -1:
        rv = int(v1[+2:])
    elif v1.find('G=') != -1:
        gv = int(v1[+2:])
    elif v1.find('B=') != -1:
        bv = int(v1[+2:])
    else:
        rv = 0
        gv = 0
        bv = 0
        
    if v2.find('R=') != -1:
        rv = int(v2[+2:])
    elif v2.find('G=') != -1:
        gv = int(v2[+2:])
    elif v2.find('B=') != -1:
        bv = int(v2[+2:])
    else:
        rv = 0
        gv = 0
        bv = 0
        
    if v3.find('R=') != -1:
        rv = int(v3[+2:])
    elif v3.find('G=') != -1:
        gv = int(v3[+2:])
    elif v3.find('B=') != -1:
        bv = int(v3[+2:])
    else:
        rv = 0
        gv = 0
        bv = 0
    
        
        
    return (rv, gv, bv)

#########################################################

# (fileCmd, delayParm) = extractDelay("D=0.5,R=127,G=18,B=122;")
# print fileCmd, delayParm
# (rv, gv, bv) = extractRGB(fileCmd)
# print rv, gv, bv

fileObj = open('DRGB_CmdList.txt','r')
cmdList = fileObj.readlines()
fileObj.close()

#print cmdList
cmdListLen = len(cmdList)
#(fileCmd, delayParm) = extractDelay(cmdList[0])
#print fileCmd, delayParm
#(rv, gv, bv) = extractRGB(fileCmd)
#print rv, gv, bv

# Serial port Connection
#serPort = 'COM4'
serPort = '/dev/ttyUSB0'

ser = serial.Serial(serPort,115200)  # open serial port
#print(ser.name)         # check which port was really used

x = ""
while (x.find(">") == -1):
    x += ser.read()
    #print x
print x
raw_input("Press Enter")
cmdIndex = 0
Rnow = 0
Gnow = 0
Bnow = 0
# Beginning of Loop where we will go through the file
while(1):
    if cmdIndex == cmdListLen:
        print "Cmd Index = cmdListLen: %i, %i"%(cmdIndex, cmdListLen)
        cmdIndex = 0
    instr = cmdList[cmdIndex]
    (fileCmd, delayVal) = extractDelay(instr)
    (rv, gv, bv) = extractRGB(fileCmd)
    print rv, gv, bv
    while(Rnow != rv or Gnow != gv or Bnow != bv):
        # Adjust Red
        if Rnow < rv:
            Rnow += 1
        elif Rnow > rv:
            Rnow -= 1
        else:
            Rnow = Rnow
        
        # Adjust Green
        if Gnow < gv:
            Gnow += 1
        elif Gnow > gv:
            Gnow -= 1
        else:
            Gnow = Gnow
            
        # Adjust Blue
        if Bnow < bv:
            Bnow += 1
        elif Bnow > bv:
            Bnow -= 1
        else:
            Bnow = Bnow
            
        setRGB(ser,Rnow,Gnow,Bnow)
        time.sleep(delayVal)
    print "Next Command"    
    cmdIndex += 1
    
    
ser.close()             # close port

