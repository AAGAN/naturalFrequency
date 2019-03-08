#define ARM_MATH_CM4
#include <arm_math.h>
#include <Adafruit_NeoPixel.h>

//Constants
const int AUDIO_INPUT_PIN = 23;
const int LED_PIN = 13;
const int SOL_PIN = 5;
const int ANALOG_READ_RESOLUTION = 10;
const int FFT_SIZE = 1024;

//Variable Declarations
int tone_error_margin_hz = 50;
int tone_peak_frequency = 1935; //Hz
int tone_threshold_db = 10;

int sample_rate_hz = 6000;
int adc_averaging = 16;
int solenoid_on_time = 6;
int cycle_time = 250;
int num_trials = 5;

//Useful variables
char val;
boolean ledState = LOW; //to toggle our LED
char receivedChars[4096];
int ndx = 0;
char endMarker = '\n';
char rc;
IntervalTimer samplingTimer;
float samples[FFT_SIZE*2];
float magnitudes[FFT_SIZE];
int sampleCounter = 0;

void setup() {
  Serial.begin(32400);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SOL_PIN, OUTPUT);
  
  //Pause until contact has been made
  establishContact();

  //Set up ADC and audio output
  pinMode(AUDIO_INPUT_PIN,INPUT);
  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogReadAveraging(adc_averaging);

  
}

void loop()
{
  if (Serial.available() > 0) { // If data is available to read,
    val = Serial.read(); // read it and store it in val

    //Determine what value has been sent
    switch (val) {
      case 'C':  //Calibration State
        Connection(); //switches the onboard LED's state
        break;
      case 'S':  //Start Test State
        Serial.println("Start Test");
        startTest();
        break;
      case 'U':  //Update Variables
        Serial.println("Update State");
        updateValues();
        break;
      default:
        Serial.println("Unknown Command");
        break;
    }
  }
}

//Wait until connection is established
void establishContact() {
  while (Serial.available() <= 0) {
    delay(300);
  }
}

//Send values from connection
void Connection() {
  sendValues();
  ledState = !ledState; //flip the ledState
  digitalWrite(LED_PIN, ledState);
}

//Update variables after receiving update command
void updateValues() {
  while (Serial.available() <= 0) {
    delay(300);
  }
  
  ledState = !ledState; //flip the ledState
  digitalWrite(LED_PIN, ledState);

  while (Serial.available() > 0 )
  {
    rc = Serial.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;

    } else {
      receivedChars[ndx] = '\0';
      ndx = 0;
    }
  }
  Serial.println(receivedChars);
  parseData(receivedChars);
}

//parse the csv values from update command
void parseData(char str[])
{
  char* pch;
  int counter = 0;

  pch = strtok(str, ",");

  while (pch != NULL)
  {
    updateVariables(counter, pch);
    counter++;
    pch = strtok(NULL, ",");
  }
}


//send the values to VB program
void sendValues() {
  Serial.print(tone_error_margin_hz);
  Serial.print(',');
  Serial.print(tone_peak_frequency);
  Serial.print(',') ;
  Serial.print(tone_threshold_db);
  Serial.print(',') ;
  Serial.print(FFT_SIZE);
  Serial.print(',') ;
  Serial.print(sample_rate_hz);
  Serial.print(',') ;
  Serial.print(adc_averaging);
  Serial.print(',') ;
  Serial.print(solenoid_on_time);
  Serial.print(',') ;
  Serial.print(cycle_time);
  Serial.print(',') ;
  Serial.println(num_trials);
}

//update the variables
void updateVariables(int index, char* value) {

  switch (index) {
    case 0:
       tone_error_margin_hz = atoi(value);
      break;
    case 1:
      tone_peak_frequency = atoi(value);
      break;
    case 2:
       tone_threshold_db = atoi(value);
      break;
    case 3:
     // FFT_SIZE = atoi(value);
      break;
    case 4:
       sample_rate_hz = atoi(value);
      break;
    case 5:
      adc_averaging = atoi(value);
      break;
    case 6:
      solenoid_on_time = atoi(value);
      break;
    case 7:
      cycle_time = atoi(value);
      break;
    case 8:
      num_trials = atoi(value);
      break;
    default:
      break;
  }
}



void startTest(){
  char data;  
  
  for (int i = 0; i < num_trials; i++)
  {
    // strike the cylinder
    digitalWrite(SOL_PIN, HIGH);
    delay(solenoid_on_time);
    digitalWrite(SOL_PIN,LOW);
    samplingBegin();
    
    
    delay(cycle_time-solenoid_on_time);
    //while might be blocking the code and another way might need to be use 
    while (!samplingIsDone())
    {

    }
    
    

      // Run FFT on sampled data.
      arm_cfft_radix4_instance_f32 fft_inst;
      arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
      arm_cfft_radix4_f32(&fft_inst, samples);
      // Calculate magnitude of complex numbers output by the FFT.
      arm_cmplx_mag_f32(samples, magnitudes, FFT_SIZE);

      detectFreq();
      

    // emergency stop by pressing s and enter on the VB interface
    if(Serial.available() > 0)
      data = Serial.read();
      if(data == 's'){
        Serial.println("TEST STOPPED");
        break;
      }
    delay(500);    
  }
  Serial.println("s");
  
}

void detectFreq(){
  //Calculate low and high frequency bins for the currently expected tones +/- the error margin
  int lowBin = frequencyToBin(tone_peak_frequency - tone_error_margin_hz);
  int highBin = frequencyToBin(tone_peak_frequency + tone_error_margin_hz);
  
// Get the average intensity of frequencies inside and outside the tone window.
  float window, other;
  windowMean(magnitudes, lowBin, highBin, &window, &other);

  float windowIntensity, otherIntensity; 
  windowIntensity = intensityDb(window);
  otherIntensity = intensityDb(other);
  
  //Sending data back to the computer (VB interface)
    Serial.print(windowIntensity);
  Serial.print(',');
  Serial.print(otherIntensity);
    Serial.print(',');
  for(int i=1; i< FFT_SIZE/2; ++i){
    Serial.print(magnitudes[i]);
    Serial.print(',');
  }
  Serial.print(window);
  Serial.print(',');
  Serial.print(other);
  Serial.print(',');


  Serial.print(highBin);
  Serial.print(',');
  Serial.println(lowBin);
  

}


////////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


void windowMean(float* magnitude, int lowBin, int highBin, float* windowMean, float* otherMean){
  *windowMean = 0;
  *otherMean = 0;
  //First mag bin is skipped because it represents the average power of the signal
  for(int i =1; i < FFT_SIZE/2; ++i)
  {
    if (i >= lowBin && i <= highBin){
      *windowMean += magnitudes[i];
    }
    else
    {
      *otherMean += magnitudes[i];
    }
  }
  *windowMean /= (highBin - lowBin) + 1;
  *otherMean /= (FFT_SIZE/2 - (highBin - lowBin));
  
  
}



// Convert a frequency to the appropriate FFT bin it will fall within.
int frequencyToBin(float frequency) {
  float binFrequency = float(sample_rate_hz) / float(FFT_SIZE);
  return int(frequency / binFrequency);
}

// Convert intensity to decibels
float intensityDb(float intensity) {
  return 20.0*log10(intensity);
}



////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void samplingCallback() {
  // Read from the ADC and store the sample data
  samples[sampleCounter] = (float32_t)analogRead(AUDIO_INPUT_PIN);
  // Complex FFT functions require a coefficient for the imaginary part of the input.
  // Since we only have real data, set this coefficient to zero.
  samples[sampleCounter+1] = 0.0;
  // Update sample buffer position and stop after the buffer is filled
  sampleCounter += 2;
  if (sampleCounter >= FFT_SIZE*2) {
    samplingTimer.end();
  }
}

void samplingBegin() {
  // Reset sample buffer position and start callback at necessary rate.
  sampleCounter = 0;
  samplingTimer.begin(samplingCallback, 1000000/sample_rate_hz);
}


boolean samplingIsDone(){
  return sampleCounter >= FFT_SIZE*2;
}
