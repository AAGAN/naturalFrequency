//Variable Declarations
int TONE_ERROR_MARGIN_HZ = 50;
int TONE_PEAK_FREQUENCY = 2100; //Hz
int TONE_THRESHOLD_DB = 10;
int FFT_SIZE = 256;
int SAMPLE_RATE_HZ = 9000;
int ADC_AVERAGING = 16;
int SOLENOID_ON_TIME = 50;
int CYCLE_TIME = 250;
int NUM_TRIALS = 10;

String updateString;

char val;
boolean ledState = LOW; //to toggle our LED
int ledPin = 13;
char receivedChars[64];
int ndx = 0;
char endMarker = '\n';
char rc;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  //Pause until contact has been made
  establishContact();
}

void loop()
{
  if (Serial.available() > 0) { // If data is available to read,
    val = Serial.read(); // read it and store it in val

    //Determine what value has been sent
    switch (val) {
      case 'C':  //Calibration State
        Connection();
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
    Serial.println("A");
    delay(300);
  }
}

void Connection() {
  sendValues();
  ledState = !ledState; //flip the ledState
  digitalWrite(ledPin, ledState);
}




void updateValues() {
  while (Serial.available() <= 0) {
    delay(300);
  }
  ledState = !ledState; //flip the ledState
  digitalWrite(ledPin, ledState);

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



void sendValues() {
  Serial.print(TONE_ERROR_MARGIN_HZ);
  Serial.print(',');
  Serial.print(TONE_PEAK_FREQUENCY);
  Serial.print(',') ;
  Serial.print(TONE_THRESHOLD_DB);
  Serial.print(',') ;
  Serial.print(FFT_SIZE);
  Serial.print(',') ;
  Serial.print(SAMPLE_RATE_HZ);
  Serial.print(',') ;
  Serial.print(ADC_AVERAGING);
  Serial.print(',') ;
  Serial.print(SOLENOID_ON_TIME);
  Serial.print(',') ;
  Serial.print(CYCLE_TIME);
  Serial.print(',') ;
  Serial.println(NUM_TRIALS);
}

void updateVariables(int index, char* value) {

  switch (index) {
    case 0:
       TONE_ERROR_MARGIN_HZ = atoi(value);
      break;
    case 1:
      TONE_PEAK_FREQUENCY = atoi(value);
      break;
    case 2:
       TONE_THRESHOLD_DB = atoi(value);
      break;
    case 3:
      FFT_SIZE = atoi(value);
      break;
    case 4:
       SAMPLE_RATE_HZ = atoi(value);
      break;
    case 5:
      ADC_AVERAGING = atoi(value);
      break;
    case 6:
      SOLENOID_ON_TIME = atoi(value);
      break;
    case 7:
      CYCLE_TIME = atoi(value);
      break;
    case 8:
      NUM_TRIALS = atoi(value);
      break;
    default:
      break;
  }

}



void startTest(){
  char data;  
    
  for (int i = 0; i < NUM_TRIALS; i++)
  {
    Serial.println(analogRead(0));
    
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

