
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TeensyView.h>  // Include the SFE_TeensyView library


#define HWSERIAL Serial1
///////////////////////////////////
// TeensyView Object Declaration //
///////////////////////////////////
//Standard
#define PIN_RESET 15
#define PIN_DC    5
#define PIN_CS    10
#define PIN_SCK   14
#define PIN_MOSI  11
TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);
//---------------------------------------------------------------------------------------
AudioInputI2S i2s1;
AudioAnalyzeNoteFrequency notefreq;
AudioOutputI2S         i2s2;

AudioConnection patchCord1(i2s1,0,i2s2,0);
AudioConnection patchCord2(i2s1,1,i2s2,1);
AudioConnection patchCord3(i2s1,0,notefreq,0);
//---------------------------------------------------------------------------------------
elapsedMillis sinceLastHit; 
float previousNote = 2000;

void setup() 
{
    HWSERIAL.begin(9600);
    AudioMemory(30);
    notefreq.begin(.15);

    oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  //oled.display();  // Display what's in the buffer (splashscreen)
  //delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
}

// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(float number, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (7 / 2)),
                 middleY - (oled.getFontWidth() / 2));
  //oled.setCursor(0,0);
  // Print the title:
  oled.print(number);
  oled.display();
  delay(25);
  oled.clear(PAGE);
}

void loop() 
{
    // read back fundamental frequency
    if (notefreq.available()) 
    {
        float note = notefreq.read();
        float prob = notefreq.probability();
        //if (prob > 0.98) Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
        if(prob>0.98 && note > 1700 && note < 2400) 
        {
          if (abs(note-previousNote) > 10) HWSERIAL.println("-------");
          previousNote = note;
          HWSERIAL.println(note);
          printTitle(note,2);
          
        }
    }
}
