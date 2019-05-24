void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}

void clearSerial()
{
  while (Serial.available() > 0)
    Serial.read();
}

void func(int num, int pinNum)
{
  digitalWrite(pinNum, HIGH);
  delay(num);
  digitalWrite(pinNum, LOW);
  clearSerial();
}

void loop() 
{
  char request;
    if (Serial.available() > 0)
    { 
      request = Serial.read();
      if(request == '4')
      {
        func(2000, 4);
      }
      else if (request == '3')
      {
        func(2000,5);
      }
      else if (request == '2')
      {
        func(2000, 6);
      }
      else if (request == '1')
      {
        func(2000, 7);
      }
      else
      {
        clearSerial();
      }
    }
}
