
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
//---------------------------------------------------------------------------------------
AudioInputI2S i2s1;
AudioAnalyzeNoteFrequency notefreq;
AudioOutputI2S         i2s2;

AudioConnection patchCord1(i2s1,0,i2s2,0);
AudioConnection patchCord2(i2s1,1,i2s2,1);
AudioConnection patchCord3(i2s1,0,notefreq,0);
//---------------------------------------------------------------------------------------
void setup() 
{
    Serial.begin(9600);
    AudioMemory(30);
    notefreq.begin(.15);
}

void loop() 
{
    // read back fundamental frequency
    if (notefreq.available()) 
    {
        float note = notefreq.read();
        float prob = notefreq.probability();
        //if (prob > 0.98) Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
        if(prob>0.98) Serial.println(note);
    }
}
