
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
//#include <TeensyView.h>  // Include the SFE_TeensyView library


#define HWSERIAL Serial1
///////////////////////////////////
// TeensyView Object Declaration //
///////////////////////////////////
//Standard
//#define PIN_RESET 15
//#define PIN_DC    5
//#define PIN_CS    10
//#define PIN_SCK   14
//#define PIN_MOSI  11
//TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

#define tempControlPin 11
#define tempPin A1
#define solPin 5
//---------------------------------------------------------------------------------------
AudioInputI2S i2s1;
AudioAnalyzeNoteFrequency notefreq;
AudioOutputI2S         i2s2;

AudioConnection patchCord1(i2s1,0,i2s2,0);
AudioConnection patchCord2(i2s1,1,i2s2,1);
AudioConnection patchCord3(i2s1,0,notefreq,0);
//---------------------------------------------------------------------------------------
elapsedMillis sinceLast; 
//float previousNote = 2000;

int solOnTime = 10; //how many milliseconds will the solenoid be powered
int solOffTime = 200; //how many milliseconds will the solenoid be off before measurement

void setup() 
{
  HWSERIAL.begin(9600);
  AudioMemory(30);
  notefreq.begin(.15);

  pinMode(solPin, OUTPUT);
  digitalWrite(solPin, LOW);
  //oled.begin();    // Initialize the OLED
  //oled.clear(ALL); // Clear the display's internal memory
  //oled.display();  // Display what's in the buffer (splashscreen)
  //delay(1000);     // Delay 1000 ms
  //oled.clear(PAGE); // Clear the buffer.

  analogReadResolution(12);
  //pinMode(tempPin, INPUT);
  pinMode(tempControlPin, OUTPUT);
  digitalWrite(tempControlPin, LOW);
}

// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
//void printTitle(float number, int font)
//{
//  int middleX = oled.getLCDWidth() / 2;
//  int middleY = oled.getLCDHeight() / 2;
//
//  oled.clear(PAGE);
//  oled.setFontType(font);
//  // Try to set the cursor in the middle of the screen
//  oled.setCursor(middleX - (oled.getFontWidth() * (7 / 2)),
//                 middleY - (oled.getFontWidth() / 2));
//  //oled.setCursor(0,0);
//  // Print the title:
//  oled.print(number);
//  oled.display();
//  delay(25);
//  oled.clear(PAGE);
//}

void clearSerial()
{
  while (HWSERIAL.available() > 0)
    HWSERIAL.read();
}

void strike(int number)
{
 for(int i = 0;i<number;i++)
 {
  digitalWrite(solPin,HIGH);
  delay(solOnTime);
  digitalWrite(solPin,LOW);
  delay(2 * solOnTime);
 }
 delay(solOffTime);
}

void func(int n)
{
  strike(n);
        // read back fundamental frequency
        sinceLast = 0;
        while(sinceLast < 500)
        {
          if (notefreq.available()) 
          {
              float note = notefreq.read();
              float prob = notefreq.probability();
              //if (prob > 0.98) Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
              if(prob>0.98 && note > 1000 && note < 4400) 
              {
                //if (abs(note-previousNote) > 10) HWSERIAL.println("-------");
                //previousNote = note;
                HWSERIAL.println(note);
                //printTitle(note,2);
                
              }
              sinceLast = 0;
          }
        }
          clearSerial();
}

void func2(int n)
{
  while(HWSERIAL.read() != 'x')
  {
        strike(n);
        // read back fundamental frequency
        sinceLast = 0;
        while(sinceLast < 500)
        {
          if (notefreq.available()) 
          {
              float note = notefreq.read();
              float prob = notefreq.probability();
              //if (prob > 0.98) Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
              if(prob>0.98 && note > 1000 && note < 4400) 
              {
                //if (abs(note-previousNote) > 10) HWSERIAL.println("-------");
                //previousNote = note;
                HWSERIAL.println(note);
                //printTitle(note,2);
                
              }
              sinceLast = 0;
          }
        }
  }
  clearSerial();
}

void readTemp()
{
  digitalWrite(tempControlPin, HIGH);
  delay(50);
  analogRead(tempPin);
  int reading = 0;
  delay(15);
  for (int i = 0 ; i < 5 ; i++ ){
    reading += analogRead(tempPin);
    delay(15);
  } 
  digitalWrite(tempControlPin, LOW);

  float voltage = reading / 5 * 330.0;
  voltage /= 4095.0;
  float temperatureC = (voltage)+5.0;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  HWSERIAL.println(temperatureC);
  
  clearSerial();
}

void loop() 
{
  char request;
    if (HWSERIAL.available() > 0)
    { 
      request = HWSERIAL.read();
      if(request == 'a')
      {
        func(1);
      }
      else if (request == 'b')
      {
        func(2);
      }
      else if (request == 'c')
      {
        func(5);
      } 
      else if (request == 'd')
      {
        func2(1);
      }
      else if (request == 'e')
      {
        func2(2);
      }
      else if (request == 'f')
      {
        func2(5);
      }
      else if (request == 't')
      {
        readTemp();
      }
      else
      {
        clearSerial();
      }
    }
}
