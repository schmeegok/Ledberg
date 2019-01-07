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
  
  mode = Mode1;
  blinkMode(mode,500);
  Serial.println("*** RGB Strip is ready ***");
  Serial.println("Commands:");
  Serial.println("*RxxxGyyyBzzz; - xxx,yyy,zzz = 000 to 255");
  Serial.println("*M[1-5]; - Set the mode");
  Serial.println("    M1 = Serial Control and Standby");
  Serial.println("    M2 = Candle");
  Serial.println("    M3 = White");
  Serial.println("    M4 = Red, Green");
  Serial.println("    M5 = Random");
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
        
        // Mode 2 = Candle
        case Mode2: 
            fireLight();
            break;

        // Mode 3 = White
        case Mode3:
            setRGBColor(255,255,255);
            break;

        // Mode 4 = RED/GREEN
        case Mode4:
            setRGBColor(255,0,0);
            delay(1000);
            setRGBColor(0,255,0);
            delay(1000);
            break;

        // Mode 5 = RANDOM
        case Mode5:
            setRGBColor(random(0,255),random(0,255),random(0,255));
            delay(2000);
            break;
    }// End Of Switch Case

    
        
}// End of Main

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

void intensityRGB(int red1Intensity, int green1Intensity, int blue1Intensity, int pause)
{
    setRGBColor(red1Intensity, green1Intensity, blue1Intensity);
    delay(pause);
}

void fireLight()
{
    // Use random generators for all 3 RGB colors 
    int randomR1, randomG1, randomB1, randomP;
    int rh, rl, gh, gl, bh, bl;
    rh = 190;
    rl = 170;
    gh = 25;
    gl = 20;
    bh = 2;
    bl = 0;
    /* WORKING
    rh = 120;
    rl = 90;
    gh = 25;
    gl = 20;
    bh = 20;
    bl = 0;
    */
    /* Too white, not enough flicker
    rh = 255;
    rl = 250;
    gh = 147;
    gl = 142;
    bh = 41;
    bl = 36;
    */
    randomR1 = random(rl,rh);
    randomG1 = random(gl,gh);
    randomB1 = random(bl,bh);
    randomP  = random(1,90);

    intensityRGB(randomR1, randomR1-160, randomB1, randomP);
}
