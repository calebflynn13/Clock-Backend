#include <FastLED.h>
#include <EEPROM.h>

#define LED_PIN     12
#define NUM_LEDS    80
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define MAXBUFFER   500
CRGB leds[NUM_LEDS];
byte colors[80 * 3];

// declare global variables here
#define MINUTE 60000
unsigned long currentTime;
unsigned long previousTime = 0;
byte currentMinute = 0;
byte currentHour = 12;
byte am = 1; // 0 = false 1 = true;

char* firstDigit;
char* secondDigit;
char* thirdDigit;
char* fourthDigit;

//protoypes
void incrementMinute();
void incrementHour();
int setDigit(int clockDigit, int setTo);
char* getDigitArray(int clockDigit);
void printDigits();
void printTime();
void setTime();
void setAM();
void setPM();
void checkForCommand();
void changeTime();
void getLit();

void setup() {
  // use for debugging
  Serial.begin(9600);
  // check if serial port is connected:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("startup begin");

  // set pins:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(12, OUTPUT);

  // initalize memory for didits
  firstDigit = (char*)malloc(sizeof(char) * 7);
  secondDigit = (char*)malloc(sizeof(char) * 7);
  thirdDigit = (char*)malloc(sizeof(char) * 7);
  fourthDigit = (char*)malloc(sizeof(char) * 7);
  setDigit(1, 1);
  setDigit(2, 2);
  setDigit(3, 0);
  setDigit(4, 0);
  setPM();

  // get time from EEPROM
//  currentHour =   EEPROM.read(0);
//  currentMinute = EEPROM.read(1);
//  am =            EEPROM.read(2);
  setTime(); // set to the real time
  
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxRefreshRate(1, true); // set to 1 hz to allow messages to get through
  printTime();
  // read in colors from memory
  for (int i = 0; i < 80; i++) {
    colors[i * 3]     = EEPROM.read(i * 3 + 3); // red
    colors[i * 3 + 1] = EEPROM.read(i * 3 + 4); // green
    colors[i * 3 + 2] = EEPROM.read(i * 3 + 5); // blue
  }
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("startup end");
}

void loop() {
  // get current time
  currentTime = millis();
  // currentently accounts for millis() overflow by calculating duration
  if (currentTime - previousTime >= MINUTE) {
    // TODO: CHANGE but rn just now blink  builtin light
    if (digitalRead(LED_BUILTIN) == HIGH) {
      digitalWrite(LED_BUILTIN, LOW);
    }
    else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    // TODO: END OF NEEDED CHANGE
    incrementMinute();
    printTime();

    // last thing to do
    previousTime = currentTime;
  }
  checkForCommand(); // used to set the time
  Serial.flush();
  getLit();
}

void changeTime() {
  setTime();
  previousTime = currentTime;
  printTime();
}

void incrementMinute() {
  currentMinute++; currentMinute %= 60; // return to 0 if we increment
  if (currentMinute == 0) {
    incrementHour();
  }
  setTime();
}

void incrementHour() {
  currentHour++; currentHour %= 12;
  if (currentHour == 0) {
    currentHour = 12;
    am = !am;
  }
}

void setTime() {
  // set the clock digits
  setDigit(1, currentHour / 10);
  setDigit(2, currentHour - (currentHour / 10));
  setDigit(3, currentMinute / 10);
  setDigit(4, currentMinute - (currentMinute / 10));
}

void setAM() {
  am = 1;
}
void setPM() {
  am = 0;
}

/**
 * inputs:
 *     clockDigit: which digit are we editing? 1, 2, 3, or 4 from left to right
 *     setTo:      the value we want that clockDigit to be set to 0-12 for most
 * return 0 on success and 1 on failure
 */
int setDigit(int clockDigit, int setTo) {
    // verify we have good parameters
    if (setTo < 0 || setTo > 9)
        return 1;
    if (clockDigit == 1 && (setTo < 0 || setTo > 1))
        return 1;

    char* array = getDigitArray(clockDigit);

    switch (setTo) {
        case 0:
            array[0] = '1';
            array[1] = '1';
            array[2] = '1';
            array[3] = '0';
            array[4] = '1';
            array[5] = '1';
            array[6] = '1';
            break;
        case 1:
            array[0] = '0';
            array[1] = '0';
            array[2] = '1';
            array[3] = '0';
            array[4] = '0';
            array[5] = '0';
            array[6] = '1';
            break;
        case 2:
            array[0] = '0';
            array[1] = '1';
            array[2] = '1';
            array[3] = '1';
            array[4] = '1';
            array[5] = '1';
            array[6] = '0';
            break;
        case 3:
            array[0] = '0';
            array[1] = '1';
            array[2] = '1';
            array[3] = '1';
            array[4] = '0';
            array[5] = '1';
            array[6] = '1';
            break;
        case 4:
            array[0] = '1';
            array[1] = '0';
            array[2] = '1';
            array[3] = '1';
            array[4] = '0';
            array[5] = '0';
            array[6] = '1';
            break;
        case 5:
            array[0] = '1';
            array[1] = '1';
            array[2] = '0';
            array[3] = '1';
            array[4] = '0';
            array[5] = '1';
            array[6] = '1';
            break;
        case 6:
            array[0] = '1';
            array[1] = '1';
            array[2] = '0';
            array[3] = '1';
            array[4] = '1';
            array[5] = '1';
            array[6] = '1';
            break;
        case 7:
            array[0] = '0';
            array[1] = '1';
            array[2] = '1';
            array[3] = '0';
            array[4] = '0';
            array[5] = '0';
            array[6] = '1';
            break;
        case 8:
            array[0] = '1';
            array[1] = '1';
            array[2] = '1';
            array[3] = '1';
            array[4] = '1';
            array[5] = '1';
            array[6] = '1';
            break;
        case 9:
            array[0] = '1';
            array[1] = '1';
            array[2] = '1';
            array[3] = '1';
            array[4] = '0';
            array[5] = '0';
            array[6] = '1';
            break;
        default:
            return 1;
            break;
    }
    return 0;
}

char* getDigitArray(int clockDigit) {
    switch(clockDigit) {
        case 1:
            return firstDigit;
            break;
        case 2:
            return secondDigit;
            break;
        case 3:
            return thirdDigit;
            break;
        case 4:
            return fourthDigit;
            break;
    }
    return firstDigit;
}



void getLit() {
  // set to proper colors:
  for (int i = 0; i < 80; i++) {
    leds[i].red = getRed(i);
    leds[i].green = getGreen(i);
    leds[i].blue = getBlue(i);
  }
  // turn ones we don't want off
  // first digit : LEDs 0 - 5
  if (currentHour < 10) {
    // we dont' need the 10's place for the hour
    for (int i = 0; i < 6; i++) {
      leds[i] = CRGB::Black; // turn off LEDs 0 - 5.
    }
  }
  // second digit : LEDs 6 - 26
  for (int i = 0; i < 7; i++) {
    if (secondDigit[i] == '0') {
      leds[i * 3 + 6] = CRGB::Black;
      leds[i * 3 + 7] = CRGB::Black;
      leds[i * 3 + 8] = CRGB::Black;
    }
  }
  // third digit : LEDs 29 - 49
  for (int i = 0; i < 7; i++) {
    if (thirdDigit[i] == '0') {
      leds[i * 3 + 29] = CRGB::Black;
      leds[i * 3 + 30] = CRGB::Black;
      leds[i * 3 + 31] = CRGB::Black;
    }
  }
  // fourth digit : LEDs 50 - 70
  for (int i = 0; i < 7; i++) {
    if (thirdDigit[i] == '0') {
      leds[i * 3 + 50] = CRGB::Black;
      leds[i * 3 + 51] = CRGB::Black;
      leds[i * 3 + 52] = CRGB::Black;
    }
  }
  
  FastLED.show();
}

byte getRed(int i) {
  return colors[i * 3]; // red part of LED in memory
}

byte getGreen(int i) {
  return colors[i * 3 + 1]; // green part of LED  
}

byte getBlue(int i) {
  return colors[i * 3 + 2]; // blue part of LED in memory
}


void checkForCommand() {
  if (Serial.available() > 0)
  {
    Serial.flush();
    String input = Serial.readString();
    //String input = Serial.readStringUntil('\n');
    input.trim(); // remove leading and trailing whitespace
    Serial.print("input = ");
    Serial.println(input);
    int i = 0;
    while (input.charAt(i) != ':' && input.charAt(i) != '\0' && input.charAt(i) != '\n') {
      i++;
    }
    if (input.substring(0, i).equals("1")) { // time:
      Serial.println(freeRam());
      input.remove(0, i+1);
      input.replace(" ", "");
      input.toLowerCase();
      // if 1:23pm
      if (input.charAt(1) == ':') {
        currentHour = (byte)input.substring(0, 1).toInt();
        currentMinute = (byte)input.substring(2,4).toInt();
        Serial.print("hour = ");
        Serial.println(currentHour);
        Serial.print("minute = ");
        Serial.println(currentMinute);

        if (input.substring(4,6).equals("am"))
          am = 1;
        else
          am = 0;
      }
      // if 12:34pm
      else {
        currentHour = (byte)input.substring(0, 2).toInt();
        currentMinute = (byte)input.substring(3,5).toInt();
        if (input.substring(5,7).equals("am"))
          am = 1;
        else
          am = 0;
      }
      //potentially override time here:
      EEPROM.update(0, currentHour); // hour
      EEPROM.update(1, currentMinute); // minute
      EEPROM.update(2, am); // 1 for am 0 for pm
      Serial.println(input);
      changeTime();
    }
    // command format: LED: LED#, redValue, greenValue, blueValue
    else if (input.substring(0, i).equals("2")) { // LED:
      Serial.println("got an LED command");
      input.replace(" ", "");
      input.remove(0, i+1); // remove command
      Serial.println("input: ");
      Serial.println(input);
      // get args
      //start loop
      for (int j = 0; j < 80; j++){ // go through all LEDs
        i = 0;
        while (input.charAt(i) != ',') {
          i++;
        }
        byte ledNum = (byte)input.substring(0, i).toInt();
        input.remove(0, i+1);
        i = 0;
        while (input.charAt(i) != ',') {
          i++;
        }
        byte red = (byte)input.substring(0, i).toInt();
        input.remove(0, i+1);
        i = 0;
        while (input.charAt(i) != ',') {
          i++;
        }
        byte green = (byte)input.substring(0, i).toInt();
        input.remove(0, i+1);
        i = 0;
        while (input.charAt(i) != ',') {
          i++;
        }
        int blue = input.substring(0, i).toInt();
        input.remove(0, i+1);

        Serial.print("LED: ");
        Serial.print(ledNum);
        Serial.print(", ");
        Serial.print("red = ");
        Serial.print(red);
        Serial.print(", ");
        Serial.print("green = ");
        Serial.print(green);
        Serial.print(", ");
        Serial.print("blue = ");
        Serial.println(blue);
        
        EEPROM.update(ledNum * 3 + 3, (byte)red);
        EEPROM.update(ledNum * 3 + 4, (byte)green);
        EEPROM.update(ledNum * 3 + 5, (byte)blue);
        colors[ledNum * 3] = (byte)red;
        colors[ledNum * 3 + 1] = (byte)green;
        colors[ledNum * 3 + 2] = (byte)blue;
        
      }
      // end loop
    }
    else if (input.substring(0, i).equals("3")) { // presets
      input.remove(0, i+1);
      input.replace(" ", "");
      // TODO: 
    }
    else if (input.substring(0, i).equals("9")) { // debugging
      // print LED EEPROM
      for (int i = 0; i < 80; i++) {
        Serial.print("LED: ");
        Serial.print(i);
        Serial.print(", ");
        Serial.print("red = ");
        Serial.print(EEPROM.read(i * 3 + 3));
        Serial.print(", ");
        Serial.print("green = ");
        Serial.print(EEPROM.read(i * 3 + 4));
        Serial.print(", ");
        Serial.print("blue = ");
        Serial.println(EEPROM.read(i * 3 + 5));
  }
    }
  }
}

void printTime() {
  Serial.print("current time = ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.print(currentMinute);
  if (am)
    Serial.println(" AM");
  else
    Serial.println(" PM");
  // printDigits();
}

void printDigits() {
    // print variables:
    Serial.print("Digit 1: ");
    for (int i = 0; i < 7; i++) {
      Serial.print(firstDigit[i]);
      Serial.print(", ");
    }
    Serial.print("\n");

    Serial.print("Digit 2: ");
    for (int i = 0; i < 7; i++) {
      Serial.print(secondDigit[i]);
      Serial.print(", ");
    }
    Serial.print("\n");

    Serial.print("Digit 3: ");
    for (int i = 0; i < 7; i++) {
      Serial.print(thirdDigit[i]);
      Serial.print(", ");
    }
    Serial.print("\n");

    Serial.print("Digit 4: ");
    for (int i = 0; i < 7; i++) {
      Serial.print(fourthDigit[i]);
      Serial.print(", ");
    }
    Serial.print("\n");
}

int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

//void getString(String *input) {
//  int charsRead;
//  charsRead = Serial.readBytesUntil('\n', (char*)input, MAXBUFFER - 1);
//  (*input).setCharAt(charsRead, '\0');      // Make it a string -- note lowercase 's'
//}
