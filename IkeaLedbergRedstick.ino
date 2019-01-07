/*
 *  TBD 1: HW/SW - Need to add in a button to control mode
 *  TBD 2: SW - Need to add in a serial control option
 *  TBD 3: HW - Need to add a power switch and determine how that will work with TBD 2
 */


/*
 * const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithStartEndMarkers();
    showNewData();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}

 * 
 * 
 */






// Serial Communcation Vars
const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;



// RGB LED Pins
const int RED_PIN   = 3; // Common Anode Pinout
const int GREEN_PIN = 5; // Common Anode Pinout
const int BLUE_PIN  = 6; // Common Anode Pinout

// Button Pin
const int BUTTON_PIN = 2;

volatile byte mode = 1; // Variable to hold the display mode
const byte Mode1   = 1;
const byte Mode2   = 2;
const byte Mode3   = 3;
const byte Mode4   = 4;
const byte Mode5   = 5;
const byte numModes = 5;


void setup() 
{
  // put your setup code here, to run once:
  
  // Setup Serial Port
  Serial.begin(115200);
  Serial.println("RGB Strip booting...");
    
  // Set up the pushbutton pins to be an input:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), modeChange, RISING);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  
  // configure RGB LED Pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Test RGB LED using Temp scale
  for (float i=-1.0; i<= 101.0; i+= 0.5)
  {
      showTempRGB(i);//, 0.0, 0.0);
      delay(10);
  }

  mode = Mode1;
  blinkMode(mode,500);
  Serial.println("*** RGB Strip is ready ***");
  Serial.println("Commands:");
  Serial.println("*RxxxGyyyBzzz; - xxx,yyy,zzz = 000 to 255");
  Serial.println("*M[1-5]; - Set the mode");
  Serial.println("    M1 = Serial Control and Standby");
  Serial.println("    M2 = Random Color");
  Serial.println("    M3 = White");
  Serial.println("    M4 = Purple");
  Serial.println("    M5 = Temp Sweep");
  Serial.println("***************************");
  Serial.print(">");
}

void loop(void) 
{
    // put your main code here, to run repeatedly:

    recvWithStartEndMarkers();
    //showNewData();
    processCommand();
           
    // Mode selection
    switch(mode)
    {
        // Mode 1 = Standby/SerialControlledValue
        case Mode1:
            //setRGBColor(0,0,0);
            break;
        
        // Mode 2 = Random
        case Mode2: // Barometer built in temp sensor
            setRGBColor(random(0,255),random(0,255),random(0,255));
            delay(1000);
            break;

        // Mode 3 = White
        case Mode3:
            setRGBColor(255,255,255);
            break;

        // Mode 4 = Purple
        case Mode4:
            setRGBColor(127,0,255);
            break;

        // Mode 5 = Temp RGB
        case Mode5:
            // Test RGB LED using Temp scale
            for (float i=-1.0; i<= 101.0; i+= 0.5)
            {
                showTempRGB(i);//, 0.0, 0.0);
                delay(50);
            }
            break;
    }// End Of Switch Case

    
        
}// End of Main

void showTempRGB(float currentTemp)//, float tempThresholdLo, float tempThresholdHi)
{
  int redIntensity;
  int greenIntensity;
  int blueIntensity;

  float slopeRed;
  float slopeBlue;
  float slopeGreen;
  
  // Python equation: RedColorValue = slopeRed1*temp + (256 - slopeRed1*(t=0.0))
  // Python equation: BlueColorValue = 255;
  if (currentTemp <= 32)          // zone 1
  {
    slopeRed   = (0.00-255.00)/(32.00-0.00);
    slopeBlue  = 0;
    slopeGreen = 0;
    redIntensity = (int) (slopeRed*currentTemp + (256 - slopeRed*0));    // As Temp increases, Red Decreases
    blueIntensity = 255;           // blue is always on
    greenIntensity = 0;        // green is always off
  }
  
  // Python equation: RedColorValue = Off
  // y1.append(mb2*t[i] + (256.00 - mb2*t[t.index(32.0)]))
  // y2.append(mg1*t[i] + (0.0 - mg1*t[t.index(32.0)]))
  
  else if (currentTemp > 32.0 && currentTemp <= 70.0)          // zone 2
  {
    slopeRed   = 0;
    slopeBlue  = (0.00-255.00)/(70.00-32.00);
    slopeGreen = (255.00-0.00)/(70.00-32.00);
    redIntensity = 0;                                                       // As Temp increases, Keep Zero
    blueIntensity = (int) (slopeBlue*currentTemp + (256 - slopeBlue*32.0));           // As Temp increases, blue fades out
    greenIntensity = (int) (slopeGreen*currentTemp + (0.0 - slopeGreen*32.0));        // As Temp decreases, green fades in
  }

  // mr2*t[i] +(0.0 - mr2*t[t.index(70.0)])
  // Blue OFF
  // mg2*t[i] + (256.00 - mg2*t[t.index(70.0)])
  
  else if (currentTemp > 70.0 && currentTemp <= 90.0)          // zone 2
  {
    slopeRed   = (255.00-0.00)/(90.00-70.00);
    slopeBlue  = 0;
    slopeGreen = (0.00-255.00)/(90.00-70.00);
    redIntensity = (int) (slopeRed*currentTemp + (0.0 - slopeRed*70.0));              // As Temp increases, red fades in
    blueIntensity = 0;                                                        // As Temp increases, blue stays off
    greenIntensity = (int) (slopeGreen*currentTemp + (256 - slopeGreen*70.0));        // As Temp decreases, green fades out
  }

  
  else if (currentTemp > 90.0 && currentTemp <= 100.0)          // zone 2
  {
    slopeRed   = 0;
    slopeBlue  = (255.00-0.00)/(100.00-90.00);
    slopeGreen = (255.00-0.00)/(100.00-90.00);
    redIntensity = 255;              // As Temp increases, red fades in
    blueIntensity = (int) (slopeBlue*currentTemp + (0.0 - slopeBlue*90.0));                                                        // As Temp increases, blue stays off
    greenIntensity = (int) (slopeGreen*currentTemp + (0.0 - slopeGreen*90.0));        // As Temp decreases, green fades out
  }

  else if (currentTemp > 100.0)
  {
    redIntensity = 255;
    blueIntensity = 255;
    greenIntensity = 255;  
  }

  analogWrite(RED_PIN, redIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
}

void setRGBColor(int redIntensity, int greenIntensity, int blueIntensity)
{
  analogWrite(RED_PIN, redIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
}

void modeChange()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce
  if (interrupt_time - last_interrupt_time > 400)
  {
    // Do your thing
    if (mode < numModes)
    {
      mode += 1;
    }
    else if (mode == numModes)
    {
        mode = 1;
    }
    blinkMode(mode, 30000);
  }
  last_interrupt_time = interrupt_time;
}

void blinkMode(int currentMode, unsigned long pause)
{
    for (int i=0; i < currentMode; i++)
    {
        setRGBColor(0,0,0);
        delay(pause);
        setRGBColor(255,255,255);
        delay(pause);
        setRGBColor(0,0,0);
        delay(pause);
    }
    
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '*';
    char endMarker = ';';
    char rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//void showNewData() {
//    if (newData == true) {
//        Serial.print("Command Received: ");
//        Serial.println(receivedChars);
//        newData = false;
//    }
//}

void processCommand()
{
    int redValue, greenValue, blueValue;
    char valueBuff[4];
    if (newData == true)
    {
       if (receivedChars[0] == 'M')
       {
          mode = atoi(&receivedChars[1]);
          Serial.print("Changing Mode to ");
          Serial.print(mode);
          Serial.println(";");
          blinkMode(mode,250);
       }
       else if (receivedChars[0] == 'R')
       {
           valueBuff[0] = receivedChars[1];
           valueBuff[1] = receivedChars[2];
           valueBuff[2] = receivedChars[3];
           valueBuff[3] = '\0';
           redValue = atoi(valueBuff);
           //recevedChars[4] should be 'G'
           valueBuff[0] = receivedChars[5];
           valueBuff[1] = receivedChars[6];
           valueBuff[2] = receivedChars[7];
           valueBuff[3] = '\0';
           greenValue = atoi(valueBuff);
           //recevedChars[8] should be 'B'
           valueBuff[0] = receivedChars[9];
           valueBuff[1] = receivedChars[10];
           valueBuff[2] = receivedChars[11];
           valueBuff[3] = '\0';
           blueValue = atoi(valueBuff);

           Serial.print("Setting RGB to ");
           Serial.print(redValue);
           Serial.print(", ");
           Serial.print(greenValue);
           Serial.print(", ");
           Serial.print(blueValue);
           Serial.println(";");
           
           setRGBColor(redValue,greenValue,blueValue);
       }
       newData = false;
       Serial.print(">");
    }
}

